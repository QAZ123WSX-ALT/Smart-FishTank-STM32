#include "stm32f10x.h"
#include "Delay.h"

// 移动平均滤波参数
#define FILTER_SIZE 5
static uint16_t distance_buffer[FILTER_SIZE];
static uint8_t buffer_index = 0;
static uint8_t buffer_full = 0;

/**
  * 函    数：HCSR04初始化
  * 参    数：无
  * 返 回 值：无
  */
void HCSR04_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_Period = 0xFFFF;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1; // 72MHz / 72 = 1MHz
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
	TIM_Cmd(TIM2, ENABLE);
	
	// Trig引脚（A6）配置为输出
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// Echo引脚（A5）配置为输入
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// 初始状态，Trig为低电平
	GPIO_ResetBits(GPIOA, GPIO_Pin_6);
	
	// 初始化距离缓冲区
	for (uint8_t i = 0; i < FILTER_SIZE; i++) {
		distance_buffer[i] = 0;
	}
}

/**
  * 函    数：获取原始距离值
  * 参    数：无
  * 返 回 值：距离值（单位：cm），返回0表示测量失败
  */
static uint16_t HCSR04_GetRawDistance(void)
{
	uint32_t start_time, end_time;
	uint16_t distance;
	uint32_t timeout = 0;
	
	// 发送10us的高电平触发信号
	GPIO_SetBits(GPIOA, GPIO_Pin_6);
	Delay_us(10);
	GPIO_ResetBits(GPIOA, GPIO_Pin_6);
	
	// 等待Echo信号变高
	while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5) == 0)
	{
		Delay_us(1);
		timeout++;
		if (timeout > 10000) // 超时处理
		{
			return 0;
		}
	}
	
	// 记录开始时间
	start_time = TIM2->CNT;
	
	// 等待Echo信号变低
	timeout = 0;
	while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5) == 1)
	{
		Delay_us(1);
		timeout++;
		if (timeout > 50000) // 超时处理（最大测量距离约4米）
		{
			return 0;
		}
	}
	
	// 记录结束时间
	end_time = TIM2->CNT;
	
	// 计算时间差（单位：us）
	uint32_t time = end_time - start_time;
	if (time < 0)
	{
		time += 0xFFFF; // 处理定时器溢出
	}
	
	// 计算距离：距离 = 时间 * 声速(340m/s) / 2
	// 340m/s = 0.034cm/us
	distance = (uint16_t)(time * 0.034 / 2);
	
	// 限制距离范围（0-400cm）
	if (distance > 400)
	{
		distance = 400;
	}
	
	return distance;
}

/**
  * 函    数：移动平均滤波
  * 参    数：无
  * 返 回 值：滤波后的距离值（单位：cm）
  */
uint16_t HCSR04_GetDistance(void)
{
	// 获取原始距离值
	uint16_t raw_distance = HCSR04_GetRawDistance();

	// 将新值加入缓冲区
	if(raw_distance>0 && raw_distance<400)
	{
	distance_buffer[buffer_index] = raw_distance;
	buffer_index = (buffer_index + 1) % FILTER_SIZE;
	}
	// 标记缓冲区是否已满
	if (!buffer_full && buffer_index == 0) {
		buffer_full = 1;
	}
	
	// 计算平均值
	uint32_t sum = 0;
	uint8_t count = buffer_full ? FILTER_SIZE : buffer_index;
	
	for (uint8_t i = 0; i < count; i++) {
		sum += distance_buffer[i];
	}
	
	// 防止除零错误
	if (count == 0) {
		return 1;
	}

	// 返回平均值
	return (uint16_t)(sum / count);
}

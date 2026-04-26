#include "stm32f10x.h"

// 按键状态标志
volatile uint8_t Key_Flag = 0;
volatile uint8_t Key_Num = 0;

//按键初始化
void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 |GPIO_Pin_13 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	// 配置GPIO外部中断
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource14);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource13);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource11);
	
	// 配置EXTI
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line14 |EXTI_Line13 | EXTI_Line11;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; // 下降沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	// 配置NVIC
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

// 获取按键编号
uint8_t Key_GetNum(void)
{
	uint8_t temp = Key_Num;
	if (Key_Flag)
	{
		Key_Flag = 0;
		return temp;
	}
	return 0;
}

// 简单的空循环延时（用于中断中）
static void delay_in_irq(uint32_t count)
{
	while (count--);
}

// 外部中断15-10处理函数
void EXTI15_10_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line13) == SET)
	{
		// 使用空循环延时消抖
		delay_in_irq(10000); // 约20ms
		if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == 0)
		{
			Key_Num = 13;
			Key_Flag = 1;
		}
		EXTI_ClearITPendingBit(EXTI_Line13);
	}
	
		if (EXTI_GetITStatus(EXTI_Line14) == SET)
	{
		// 使用空循环延时消抖
		delay_in_irq(10000); // 约20ms
		if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0)
		{
			delay_in_irq(8000000);
			if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0)
			{
				Key_Num = 141;
				Key_Flag = 1;
			}
			else
			{
				Key_Num	= 14;
				Key_Flag = 1;
			}
		}
		EXTI_ClearITPendingBit(EXTI_Line14);
	}
	if (EXTI_GetITStatus(EXTI_Line11) == SET)
	{
		// 使用空循环延时消抖
		delay_in_irq(10000); // 约20ms
		if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == 0)
		{
			Key_Num = 11;
			Key_Flag = 1;
		}
		EXTI_ClearITPendingBit(EXTI_Line11);
	}
}

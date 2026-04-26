#include "stm32f10x.h"
#include "Delay.h"

//光敏传感器初始化
void Light_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

//获取光敏传感器状态
uint8_t Get_LightNum(void)
{
	return GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13);
}

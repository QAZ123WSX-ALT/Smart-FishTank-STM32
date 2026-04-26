#include "stm32f10x.h"

//蜂鸣器初始化
void Buzzer_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

//蜂鸣器开启
void Buzzer_ON(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_12);
}

//蜂鸣器关闭
void Buzzer_OFF(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_12);
}

//蜂鸣器翻转
void Buzzer_turn(void)
{
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == 0)
		GPIO_SetBits(GPIOB, GPIO_Pin_12);
	else
		GPIO_ResetBits(GPIOB, GPIO_Pin_12);
}

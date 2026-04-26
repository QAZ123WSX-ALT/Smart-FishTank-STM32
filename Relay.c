#include "stm32f10x.h"
#include "string.h"
#include "OLED.h"

GPIO_TypeDef *GPIOX;  

void Relay_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOA, GPIO_Pin_7);
	GPIO_SetBits(GPIOB, GPIO_Pin_1);
	GPIO_SetBits(GPIOB, GPIO_Pin_10);
	GPIO_SetBits(GPIOB, GPIO_Pin_0);
}

void Relay_turn(char *String)
{
	int pin_num=(String[1]-'0');
	switch(pin_num)
	{
		case 4:pin_num=10;
					 GPIOX=GPIOB;
					 break;
		case 3:pin_num=1;
					 GPIOX=GPIOB;
					 break;
		case 2:pin_num=0;
					 GPIOX=GPIOB;
					 break;
		case 1:pin_num=7;
					 GPIOX=GPIOA;
					 break;
	}

	uint16_t pin_mask = 1 << pin_num;
	if (GPIO_ReadOutputDataBit(GPIOX,  pin_mask ) == 0)
		GPIO_SetBits(GPIOX, pin_mask);
	else
		GPIO_ResetBits(GPIOX, pin_mask);
}

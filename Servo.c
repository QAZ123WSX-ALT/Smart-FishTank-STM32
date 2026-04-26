#include "stm32f10x.h"
#include "PWM.h"

/**
  * 函    数：舵机初始化
  * 参    数：无
  * 返 回 值：无
  */
void Servo_Init(void)
{
	PWM_Init();
}

/**
  * 函    数：舵机设置角度
  * 参    数：Angle 要设置的舵机角度，范围：0~180度
  * 返 回 值：无
  */
void Servo_SetAngle(float Angle)
{
	PWM_SetCompare2(Angle / 180 * 2000 + 500);
}

#ifndef __HCSR04_H
#define __HCSR04_H

#include "stm32f10x.h"

/**
  * 函    数：HCSR04初始化
  * 参    数：无
  * 返 回 值：无
  */
void HCSR04_Init(void);

/**
  * 函    数：获取距离值
  * 参    数：无
  * 返 回 值：距离值（单位：cm），返回0表示测量失败
  */
uint16_t HCSR04_GetDistance(void);

#endif

#ifndef __SERIAL_H
#define __SERIAL_H

#include <stdio.h>

extern char Serial_RxPacket[];
extern uint8_t Serial_RxFlag;

void Serial_Init(void);
void Serial_SendByte(uint8_t Byte);//串口发送一个字节
void Serial_SendArray(uint8_t *Array, uint16_t Length);//串口发送一个数组
void Serial_SendString(char *String);//串口发送一字符串
void Serial_SendNumber(uint32_t Number, uint8_t Length);//串口发送数字
void Serial_Printf(char *format, ...);//串口发送一个字节

#endif

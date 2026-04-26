#ifndef __DS18B2_H
#define __DS18B2_H

#include "stm32f10x.h"
#include "Delay.h"
   	
uint8_t DS18B20_Init(void);			    
int getTemperture(void);	       
void DS18B20_Start(void);		      
void DS18B20_Write_Byte(u8 dat);  
uint8_t DS18B20_Read_Byte(void);		
uint8_t DS18B20_Read_Bit(void);		   
uint8_t DS18B20_Check(void);			
void DS18B20_Reset(void);		

#endif




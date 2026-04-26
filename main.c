#include "stm32f10x.h"

#include "Delay.h"
#include "LED.h"
#include "Key.h"
#include "Buzzer.h"
#include "Light.h"
#include "OLED.h"
#include "Servo.h"
#include "Motor.h"
#include "PWM.h"
#include "string.h"
#include "Serial.h"
#include "ADC.h"
#include "DS18B20.h"
#include "HC_SR04.h"
#include "RTC.h"
#include "Relay.h"

//变量声明
uint8_t		  KeyNum = 0;        		//定义获取按键值
float  	    Angle = 0;
uint16_t  	AD0_, AD1_=1;       	  //定义AD值变量(AD0 TDS检测，AD1 光敏检测)
uint16_t  	AD0, AD1=1;       	  //定义AD值变量(AD0 TDS检测，AD1 光敏检测)
uint16_t  	AD0_T, AD1_T=30;
int8_t  	  UI=0;     		//定义UI界面页码
int         temp=31; // 
int         temp_TM=30;
int      		temp_TL=10;
int         led=0;  // 移到全局变量，保持LED状态
uint16_t    distance;  // 改为uint16_t类型，与函数返回值一致
uint16_t    distance_TM;
uint16_t    distance_TL=100;

int         Relay[3]={0};
int         Change=0;
uint16_t    Clock_Time[] = {18, 00, 15};
int         i,j,k=0;
uint8_t     EditMode = 0;  // 编辑模式标志，0-正常，1-编辑小时，2-编辑分钟，3-编辑秒钟
uint16_t    blink_counter = 0;  // 闪烁计数器

extern char Serial_RxPacket[100];
extern int  Serial_size;

//函数声明
void jiance(void);
void DisplayUI(void);
void Serial(void);
void key(void);
void change(void);
void dingshi(void);

int main(void)
{
	LED_Init();     //LED灯初始化
	Key_Init();
	Buzzer_Init();
	Light_Init();
	OLED_Init();
	Servo_Init();
	Serial_Init();
	AD_Init();
	DS18B20_Init();
	HCSR04_Init();  // 添加HCSR04初始化
	MyRTC_Init();	
	Relay_Init();
	
	Buzzer_OFF();

	LED(0x0000);
	Servo_SetAngle(Angle);

	DisplayUI(); // 初始化显示	
	
	while (1)
	{

		jiance();     //对环境温度进行检测
		Serial();     //蓝牙模块
		key();        //按键获取与检测
		DisplayUI();  //OLED界面进行控制
		dingshi();	  //进行自动化操作
		Delay_ms(10);  // 延时10ms，控制DisplayUI调用频率
	}
}

void key(void)
{
	KeyNum = Key_GetNum();

		if (KeyNum == 14)
		{
			UI++;
			if (UI > 1) UI = 0; // 循环切换UI
			OLED_Clear();
			DisplayUI();
		}
		// 长按K14进入编辑模式
		else if (KeyNum == 141 && UI == 0)
		{
			EditMode++;
			if (EditMode > 3) EditMode = 0;
			OLED_Clear();
			DisplayUI();
		}
		else if (KeyNum == 141 && UI == 1)
		{
			EditMode++;
			if (EditMode > 6) EditMode = 0;
			OLED_Clear();
			DisplayUI();
		}
		// UI=0编辑模式下的操作
		else if (EditMode > 0 && UI == 0)
		{
			if (KeyNum == 11) // K11加1
			{
				switch (EditMode)
				{
				case 1: // 编辑小时
					Clock_Time[0]++;
					if (Clock_Time[0] > 23) Clock_Time[0] = 0;
					break;
				case 2: // 编辑分钟
					Clock_Time[1]++;
					if (Clock_Time[1] > 59) Clock_Time[1] = 0;
					break;
				case 3: // 编辑秒钟
					Clock_Time[2]++;
					if (Clock_Time[2] > 59) Clock_Time[2] = 0;
					break;
				}
				OLED_Clear();
				DisplayUI();
			}
			else if (KeyNum == 13) // K13减1
			{
				switch (EditMode)
				{
				case 1: // 编辑小时
					if (Clock_Time[0] == 0) Clock_Time[0] = 23;
					else Clock_Time[0]--;
					break;
				case 2: // 编辑分钟
					if (Clock_Time[1] == 0) Clock_Time[1] = 59;
					else Clock_Time[1]--;
					break;
				case 3: // 编辑秒钟
					if (Clock_Time[2] == 0) Clock_Time[2] = 59;
					else Clock_Time[2]--;
					break;
				}
				OLED_Clear();
				DisplayUI();
			}
		}
		// UI=1编辑模式下的操作
		else if (EditMode > 0 && UI == 1)
		{
			if (KeyNum == 11) // K11加1
			{
				switch (EditMode)
				{
				case 1: // 温度下限
					temp_TL++;
					if (temp_TL > 40) temp_TL = 0;
					break;
				case 2: // 温度上限
					temp_TM++;
					if (temp_TM > 50) temp_TM = 0;
					break;
				case 3: // 编辑TDS
					AD0_T=AD0_T+5;
					if (AD0_T > 1000) AD0_T = 0;
					break;
				case 4: // 编辑光敏
					AD1_T=AD1_T+5;
					if (AD1_T > 100) AD1_T = 0;
					break;
				case 5: // 编辑最低水位
					distance_TL++;
					if (distance_TL > 500) distance_TL = 0;
					break;
				case 6: // 编辑最高水位
					distance_TM++;
					if (distance_TM > 500) distance_TM = 0;
					break;
				}
				OLED_Clear();
				DisplayUI();
			}
			else if (KeyNum == 13) // K13减1
			{
				switch (EditMode)
				{
				case 1: // 温度下限
					if (temp_TL == 0) temp_TL = 40;
					else temp_TL--;
					break;
				case 2: // 温度上限
					if (temp_TM == 0) temp_TM = 50;
					else temp_TM--;
					break;
				case 3: // 编辑TDS
					if (AD0_T < 5) AD0_T = 1000;
					else AD0_T=AD0_T-5;
					break;
				case 4: // 编辑光敏
					if (AD1_T < 5) AD1_T = 100;
					else AD1_T=AD1_T-5;
					break;
				case 5: // 编辑最低水位
					if (distance_TL == 0) distance_TL = 500;
					else distance_TL--;
					break;
				case 6: // 编辑最高水位
					if (distance_TM == 0) distance_TM = 500;
					else distance_TM--;
					break;
				}
				OLED_Clear();
				DisplayUI();
			}
		}
		// 正常模式下的操作
		else if (EditMode == 0)
		{
			if (KeyNum == 11)
			{
				LED_turn();	
			}
			if (KeyNum == 13)
			{
				LED_turn();	
			}
		}
}


void jiance(void)
{
	temp = getTemperture();
	
	distance = HCSR04_GetDistance();
	
	AD0_ = AD_GetValue(ADC_Channel_2);		//单次启动ADC，转换通道2，TDS检测
	AD1_ = AD_GetValue(ADC_Channel_3);		//单次启动ADC，转换通道3，光敏检测
	AD0=TDS(AD0_,25);											
	AD1=100-((AD1_*100)/4096);						//对ADC值进行转化计算
}

void dingshi(void)
{
	//定时喂食
	if(Clock_Time[0] == MyRTC_Time[3] && Clock_Time[1] == MyRTC_Time[4] && Clock_Time[2] == MyRTC_Time[5])
	{
		LED_turn();
		Servo_SetAngle(30);
		Delay_ms(500);
		Servo_SetAngle(0);
	}
	//温度控制：低于15度加热（K4），高于25度制冷（K3），区间内关闭
	if(temp < 15)
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_7);  // 关闭K1
		GPIO_SetBits(GPIOB, GPIO_Pin_0);  // 关闭K2
		GPIO_SetBits(GPIOB, GPIO_Pin_1);  // 关闭K3
		GPIO_ResetBits(GPIOB, GPIO_Pin_10); // 开启K4（加热）
	}
	else if(temp > 25)
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_7);  // 关闭K1
		GPIO_SetBits(GPIOB, GPIO_Pin_0);  // 关闭K2
		GPIO_SetBits(GPIOB, GPIO_Pin_10); // 关闭K4
		GPIO_ResetBits(GPIOB, GPIO_Pin_1);  // 开启K3（制冷）
	}
	else
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_7);  // 关闭K1
		GPIO_SetBits(GPIOB, GPIO_Pin_0);  // 关闭K2
		GPIO_SetBits(GPIOB, GPIO_Pin_1);  // 关闭K3
		GPIO_SetBits(GPIOB, GPIO_Pin_10); // 关闭K4
	}

	//水位检测：低于6cm加水（K2），高于10cm排水（K1），区间内关闭
	if(distance < 6)
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_7);  // 关闭K1
		GPIO_SetBits(GPIOB, GPIO_Pin_1);  // 关闭K3
		GPIO_SetBits(GPIOB, GPIO_Pin_10); // 关闭K4
		GPIO_ResetBits(GPIOB, GPIO_Pin_0);  // 开启K2（加水）
	}
	else if(distance > 10)
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_0);  // 关闭K2
		GPIO_SetBits(GPIOB, GPIO_Pin_1);  // 关闭K3
		GPIO_SetBits(GPIOB, GPIO_Pin_10); // 关闭K4
		GPIO_ResetBits(GPIOA, GPIO_Pin_7);  // 开启K1（排水）
	}
	else
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_7);  // 关闭K1
		GPIO_SetBits(GPIOB, GPIO_Pin_0);  // 关闭K2
		GPIO_SetBits(GPIOB, GPIO_Pin_1);  // 关闭K3
		GPIO_SetBits(GPIOB, GPIO_Pin_10); // 关闭K4
	}

	//TDS报警
	if(AD0 > AD0_T)
	{
		Buzzer_ON();
		k++;
		if(k==50)
		{
			k=0;
			LED_turn();
		}
	}
	//光敏
	if(AD1 < AD1_T)
		LED_ON();
	else
		LED_OFF();
}
void Serial(void)
{
	//LED
	if(i==0)
	if (Serial_RxFlag == 1)		//如果接收到数据包
	{
		
		//LED
		if (strcmp(Serial_RxPacket, "1") == 0)			
		{
			LED_turn();											//LED翻转
			if(led==0)
			{
				led=1;
				Serial_SendString("LED_ON \r\n");				//串口回传一个字符串LED_ON_OK
			}
			else 
			{
				led=0;
					Serial_SendString("LED_OFF\r\n");				//串口回传一个字符串LED_ON_OK
			}
		}
		
		
		//舵机
		else if (strcmp(Serial_RxPacket, "2") == 0)	
		{
			Angle += 90;
			if (Angle > 180)
			{
				Angle = 0;
			}
			Servo_SetAngle(Angle);									//
			Serial_SendString("Motor_ON_OK\r\n");			//串口回传一个字符串LED_OFF_OK
			
		}
		//继电器
		else if (Serial_RxPacket[0]== 'K')	
		{
			if(Serial_RxPacket[1]=='1' || Serial_RxPacket[1]=='2' || Serial_RxPacket[1]=='3' || Serial_RxPacket[1]=='4')
			{
				Relay[(Serial_RxPacket[1]-'0')]++;
				Relay_turn( Serial_RxPacket );
			}
		}	
		//进入更改喂食时间
		else if (Serial_RxPacket[0]== 'M')
		{
			Serial_SendString("Change Start\r\nHour\r\n");
			i=1;
			Serial_RxPacket[0]= '0';
		}
		else							//上述所有条件均不满足，即收到了未知指令
		{
			Serial_SendString("ERROR_COMMAND\r\n");			//串口回传一个字符串ERROR_COMMAND
			OLED_ShowString(2, 1, "                ");
			OLED_ShowString(2, 1, "ERROR_COMMAND");			//OLED清除指定位置，并显示ERROR_COMMAND
		}

	}
 	  //更改喂食时间
		if(i==1)
		{
			if (Serial_RxFlag == 1)
			{
			if(Serial_RxPacket[0]== 'T')
			{
				k++;
				if(k==3)k=0;
				if(k==0)Serial_SendString("Hour\r\n");
				if(k==1)Serial_SendString("Win\r\n");
				if(k==2)Serial_SendString("Sec\r\n");
			}
			if(Serial_RxPacket[0]== '+')
			Clock_Time[k]++;
			if(Serial_RxPacket[0]== '-')
			Clock_Time[k]--;
			
			
			Serial_SendString("Clock:");
			Serial_SendNumber(Clock_Time[0],2);
			Serial_SendString(":");	
			Serial_SendNumber(Clock_Time[1],2);
			Serial_SendString(":");	
			Serial_SendNumber(Clock_Time[2],2);
			Serial_SendString("\r\n");	
			
			
			if(Serial_RxPacket[0]== 'M')
			{
				i=0;
				Serial_SendString("Change End\r\n");
			}
			}
	}
}

void DisplayUI( void )
{
	if (Serial_RxFlag == 1)
	{
		OLED_Clear();
		OLED_ShowString(2,3,Serial_RxPacket);
		Delay_ms(100);
		Serial_RxFlag = 0;
	}
	else if(UI==0)
	{
		MyRTC_ReadTime();
		OLED_ShowString(2, 1, "Date:");
		OLED_ShowString(2, 10, "-");
		OLED_ShowString(2, 13, "-");
		OLED_ShowString(3, 1, "Time:");
		OLED_ShowString(3, 8, ":");
		OLED_ShowString(3, 11, ":");
		OLED_ShowString(4, 1, "Clock:");
		OLED_ShowString(4, 9, ":");
		OLED_ShowString(4, 12, ":");
		
		// 显示编辑模式提示
		if (EditMode > 0)
		{
			OLED_ShowString(1, 1, "Edit Mode   ");
			switch (EditMode)
			{
			case 1:
				OLED_ShowString(1, 11, "Hour");
				break;
			case 2:
				OLED_ShowString(1, 11, "Min");
				break;
			case 3:
				OLED_ShowString(1, 11, "Sec");
				break;
			}
		}
		else
		{
			OLED_ShowString(1, 1, "Control Mode");
		}
		
		OLED_ShowNum(2, 6, MyRTC_Time[0], 4);		//显示MyRTC_Time数组中的时间值，年
		OLED_ShowNum(2, 11, MyRTC_Time[1], 2);		//月
		OLED_ShowNum(2, 14, MyRTC_Time[2], 2);		//日
		OLED_ShowNum(3, 6, MyRTC_Time[3], 2);		//时
		OLED_ShowNum(3, 9, MyRTC_Time[4], 2);		//分
		OLED_ShowNum(3, 12, MyRTC_Time[5], 2);		//秒
		OLED_ShowNum(4, 7, Clock_Time[0], 2);		//喂食时
		OLED_ShowNum(4, 10, Clock_Time[1], 2);		//分
		OLED_ShowNum(4, 13, Clock_Time[2], 2);		//秒
		
		// 编辑模式下，闪烁显示当前编辑的字段
		if (EditMode > 0)
		{
			// 使用static变量实现每次调用取反的闪烁效果
			static uint8_t blink = 0;
			blink = !blink;
			
			if (blink)
			{
				// 显示当前值
				switch (EditMode)
				{
				case 1: // 编辑小时
					OLED_ShowNum(4, 7, Clock_Time[0], 2);
					break;
				case 2: // 编辑分钟
					OLED_ShowNum(4, 10, Clock_Time[1], 2);
					break;
				case 3: // 编辑秒钟
					OLED_ShowNum(4, 13, Clock_Time[2], 2);
					break;
				}
			}
			else
			{
				// 隐藏当前值（显示空格）
				switch (EditMode)
				{
				case 1: // 编辑小时
					OLED_ShowString(4, 7, "  ");
					break;
				case 2: // 编辑分钟
					OLED_ShowString(4, 10, "  ");
					break;
				case 3: // 编辑秒钟
					OLED_ShowString(4, 13, "  ");
					break;
				}
			}
		}
		else
		{
			// 退出编辑模式时重置计数器
			blink_counter = 0;
		}
	}
	else if(UI==1)//检测
	{
		// 编辑模式
		if (EditMode > 0)
		{
			OLED_ShowString(1, 1, "Edit Threshold");
			
			// 显示当前编辑的阈值类型
			switch (EditMode)
			{
			case 1:
				OLED_ShowString(2, 1, "Temp Min:");
				break;
			case 2:
				OLED_ShowString(2, 1, "Temp Max:");
				break;
			case 3:
				OLED_ShowString(2, 1, "TDS:");
				break;
			case 4:
				OLED_ShowString(2, 1, "Light:");
				break;
			case 5:
				OLED_ShowString(2, 1, "Dist Min:");
				break;
			case 6:
				OLED_ShowString(2, 1, "Dist Max:");
				break;
			}
			
			// 显示阈值单位
			switch (EditMode)
			{
			case 1:
			case 2:
				OLED_ShowString(2, 13, "C");
				break;
			case 3:
				OLED_ShowString(2, 10, "ppm");
				break;
			case 4:
				OLED_ShowString(2, 10, "%");
				break;
			case 5:
			case 6:
				OLED_ShowString(2, 13, "cm");
				break;
			}
			
			// 闪烁显示当前编辑的阈值
			static uint8_t blink = 0;
			blink = !blink;
			
			if (blink)
			{
				switch (EditMode)
				{
				case 1:
					OLED_ShowNum(2, 10, temp_TL, 2);
					break;
				case 2:
					OLED_ShowNum(2, 10, temp_TM, 2);
					break;
				case 3:
					OLED_ShowNum(2, 5, AD0_T, 4);
					break;
				case 4:
					OLED_ShowNum(2, 7, AD1_T, 2);
					break;
				case 5:
					OLED_ShowNum(2, 10, distance_TL, 3);
					break;
				case 6:
					OLED_ShowNum(2, 10, distance_TM, 3);
					break;
				}
			}
			else
			{
				switch (EditMode)
				{
				case 1:
					OLED_ShowString(2, 10, "  ");
					break;
				case 2:
					OLED_ShowString(2, 10, "  ");
					break;
				case 3:
					OLED_ShowString(2, 5, "    ");
					break;
				case 4:
					OLED_ShowString(2, 7, "  ");
					break;
				case 5:
					OLED_ShowString(2, 10, "   ");
					break;
				case 6:
					OLED_ShowString(2, 10, "   ");
					break;
				}
			}
			
			// 显示操作提示
			OLED_ShowString(3, 1, "K11: +");
			OLED_ShowString(3, 8, "K13: -");
			OLED_ShowString(4, 1, "Long K14: Next");
		}
		// 正常检测模式
		else
		{
			OLED_ShowString(1, 1, "TEMP:");
			OLED_ShowString(1, 8, "C");
			OLED_ShowString(2, 1, "TDS:");
			OLED_ShowString(2, 9, "ppm");
			OLED_ShowString(3, 1, "Light:");
			OLED_ShowString(3, 9, "%");
			OLED_ShowString(4, 1, "DIST:");
			OLED_ShowString(4, 9, "cm");
			if (temp == -99) {
				OLED_ShowString(1, 6, "Err");
			} 
			else {
				OLED_ShowNum(1, 6, (uint32_t)temp, 2);
			}
			OLED_ShowNum(2, 5, AD0, 4);		//显示通道0的转换结果AD0
			OLED_ShowNum(3, 7, AD1, 2);		//显示通道1的转换结果AD1
			OLED_ShowNum(4, 6, distance, 3);		//显示距离
		}
		}
}

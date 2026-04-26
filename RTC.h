#ifndef __RTC_H
#define __RTC_H

extern uint16_t MyRTC_Time[];
extern uint8_t Alarm_Hour;
extern uint8_t Alarm_Minute;
extern uint8_t Alarm_Second;

void MyRTC_Init(void);
void MyRTC_SetTime(void);
void MyRTC_ReadTime(void);
void MyRTC_SetAlarm(uint8_t hour, uint8_t minute, uint8_t second);
void MyRTC_GetAlarmTime(void);

#endif

/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCMCU.com --------------------------------------------*/
/* --- Web: www.STCMCUDATA.com  ---------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* 如果要在程序中使用此代码,请在程序中注明使用了STC的资料及程序            */
/*---------------------------------------------------------------------*/

#include "Beep.h"
#include "timer.h"

bit beepStartFlag = 0;	  //控制蜂鸣器是否鸣笛
bit beepAlarmRunFlag = 0; //控制定时器1K信号是否工作
typedef struct
{
	unsigned char State;
	unsigned char ModeSet;
	unsigned short TimeSet;
	unsigned int TimeCnt;
} BeepAlarmStruct_T;

BeepAlarmStruct_T BeepAlarmStruct;

//获取蜂鸣器报警标志
bit Get_beepAlarmRunFlag(void)
{
	return beepAlarmRunFlag;
}

//开始蜂鸣器报警设定值
// time:持续时间
// speed：连续鸣笛速度 0:慢速，1：快速
// mode:鸣笛模式 0:1次 ，1：连续
void StartBeepAlarm(unsigned short time, unsigned char mode) // time单位1ms
{
	BeepAlarmStruct.State = 0;
	BeepAlarmStruct.TimeSet = time * 2;
	BeepAlarmStruct.ModeSet = mode;
	beepAlarmRunFlag = 1;
	beepStartFlag = 1;
}

//清除报警
void ClrBeepAlarm(void)
{
	beepStartFlag = 0;
	beepAlarmRunFlag = 0;
}

//运行蜂鸣器报警任务
void BeepAlarmTask(void)
{
	if (beepStartFlag == 0)
		return;
	switch (BeepAlarmStruct.State)
	{
	case 0:
		beepAlarmRunFlag = 1;
		BeepAlarmStruct.TimeCnt = Get_SysHalfMsTick();
		BeepAlarmStruct.State = 1;
		break;
	case 1:
		if (get_time_escape_sec(Get_SysHalfMsTick(), BeepAlarmStruct.TimeCnt) >= BeepAlarmStruct.TimeSet)
		{
			beepAlarmRunFlag = 0;
			if (BeepAlarmStruct.ModeSet)
			{
				ClrBeepAlarm();
			}
			else
			{
				BeepAlarmStruct.State = 2;
			}
		}
		break;
	case 2:
		if (get_time_escape_sec(Get_SysHalfMsTick(), BeepAlarmStruct.TimeCnt) >= BeepAlarmStruct.TimeSet * 2)
		{
			BeepAlarmStruct.State = 0;
		}
		break;
	default:
		break;
	}
}
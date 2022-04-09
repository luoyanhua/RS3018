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

#ifndef __TIMER_H
#define __TIMER_H

#include "config.h"

#define Timer0 0
#define Timer1 1
#define Timer2 2
#define Timer3 3
#define Timer4 4

#define TIM_16BitAutoReload 0
#define TIM_16Bit 1
#define TIM_8BitAutoReload 2
#define TIM_16BitAutoReloadNoMask 3

#define TIM_CLOCK_1T 0
#define TIM_CLOCK_12T 1
#define TIM_CLOCK_Ext 2

typedef struct
{
	unsigned char TIM_Mode;	  //工作模式,  	TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	unsigned char TIM_Priority;  //优先级设置	Priority_0,Priority_1,Priority_2,Priority_3
	unsigned char TIM_Interrupt; //中断允许		ENABLE,DISABLE
	unsigned char TIM_ClkSource; //时钟源		TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	unsigned char TIM_ClkOut;	  //可编程时钟输出,	ENABLE,DISABLE
	u16 TIM_Value;	  //装载初值
	unsigned char TIM_Run;		  //是否运行		ENABLE,DISABLE
} TIM_InitTypeDef;

unsigned char Timer_Inilize(unsigned char TIM, TIM_InitTypeDef *TIMx);

void Start_SendPlus(void);

bit Get_plusOutFlag(void);

unsigned int Get_RecvPlusTimerCnt(void); //单位12.5us

unsigned int Get_us_250Cnt(void);

unsigned int Get_SysHalfMsTick(void);

unsigned int get_time_escape_sec(unsigned int ticks_now, unsigned int ticks_old);

#endif

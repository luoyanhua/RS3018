/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCMCU.com --------------------------------------------*/
/* --- Web: www.STCMCUDATA.com  ---------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* ���Ҫ�ڳ�����ʹ�ô˴���,���ڳ�����ע��ʹ����STC�����ϼ�����            */
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
	unsigned char TIM_Mode;	  //����ģʽ,  	TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	unsigned char TIM_Priority;  //���ȼ�����	Priority_0,Priority_1,Priority_2,Priority_3
	unsigned char TIM_Interrupt; //�ж�����		ENABLE,DISABLE
	unsigned char TIM_ClkSource; //ʱ��Դ		TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	unsigned char TIM_ClkOut;	  //�ɱ��ʱ�����,	ENABLE,DISABLE
	u16 TIM_Value;	  //װ�س�ֵ
	unsigned char TIM_Run;		  //�Ƿ�����		ENABLE,DISABLE
} TIM_InitTypeDef;

unsigned char Timer_Inilize(unsigned char TIM, TIM_InitTypeDef *TIMx);

void Start_SendPlus(void);

bit Get_plusOutFlag(void);

unsigned int Get_RecvPlusTimerCnt(void); //��λ12.5us

unsigned int Get_us_250Cnt(void);

unsigned int Get_SysHalfMsTick(void);

unsigned int get_time_escape_sec(unsigned int ticks_now, unsigned int ticks_old);

#endif

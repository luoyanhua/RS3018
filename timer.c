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

/*************	功能说明	**************

本文件为STC8系列的定时器初始化和中断程序,用户可以在这个文件中修改自己需要的中断程序.

******************************************/

#include "timer.h"
#include "Beep.h"

#define PLUS_MAX 24 //定义输出脉冲数

bit plusOutFlag = 0;		  //输出波形标志
bit firstIntFlag = 0;
bit firstBeepFlag = 0;
unsigned char plusOutCnt = 0; // 80K中断计数

unsigned int recvPlusTimerCnt = 0;

unsigned char plusAdjustState = 0;

unsigned int halfMsCnt = 0;

unsigned int us_250Cnt = 0;

void PlusAdjust(void)
{
	switch (plusAdjustState)
	{
	case 0:
		P36 = 1;
		plusAdjustState = 1;
		break;
	case 1:
		if (plusOutCnt > PLUS_MAX )
		{
			plusOutFlag = 0;
			Timer0_InterruptDisable(); //禁止中断
		}
		else
		{
			P36 = ~P36;
		}
		break;
	default:
		break;
	}
}

void Start_SendPlus(void)
{
	plusOutFlag = 1;
	plusOutCnt = 0;
	plusAdjustState = 0;
	Timer0_InterruptEnable(); //允许中断
}

bit Get_plusOutFlag(void)
{
	return plusOutFlag;
}

unsigned int Get_RecvPlusTimerCnt(void) //单位12.5us
{
	return recvPlusTimerCnt;
}

unsigned int Get_us_250Cnt(void)
{
	return us_250Cnt;
}

unsigned int Get_SysHalfMsTick(void) //单位是0.5ms
{
	return halfMsCnt;
}

unsigned int get_time_escape_sec(unsigned int ticks_now, unsigned int ticks_old)
{
	return ((ticks_now - ticks_old) & 0xffffffff);
}

/********************* Timer0中断函数************************/
void timer0_int(void) interrupt TIMER0_VECTOR
{
	recvPlusTimerCnt++;
	
	//每次调用输出MAX个脉冲
	if (plusOutFlag == 1)
	{
		plusOutCnt++;
		PlusAdjust();
	}
}



/********************* Timer1中断函数************************/
void timer1_int(void) interrupt TIMER1_VECTOR
{
	us_250Cnt++;
	if(firstIntFlag == 0)
	{
		firstIntFlag = 1;
		halfMsCnt++;
		if (Get_beepAlarmRunFlag() == 1)
		{
			P35 = ~P35;
		}
	}
	else
	{
		firstIntFlag = 0;
	}
}

//========================================================================
// 函数: unsigned char	Timer_Inilize(unsigned char TIM, TIM_InitTypeDef *TIMx)
// 描述: 定时器初始化程序.
// 参数: TIMx: 结构参数,请参考timer.h里的定义.
// 返回: 成功返回0, 空操作返回1,错误返回2.
// 版本: V1.0, 2012-10-22
//========================================================================
unsigned char Timer_Inilize(unsigned char TIM, TIM_InitTypeDef *TIMx)
{
	if (TIM == Timer0)
	{
		Timer0_Stop(); //停止计数
		if (TIMx->TIM_Interrupt == ENABLE)
			Timer0_InterruptEnable(); //允许中断
		else
			Timer0_InterruptDisable(); //禁止中断
		if (TIMx->TIM_Priority > Priority_3)
			return 2;						 //错误
		Timer0_Priority(TIMx->TIM_Priority); //指定中断优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3

		if (TIMx->TIM_Mode >= TIM_16BitAutoReloadNoMask)
			return 2;							//错误
		TMOD = (TMOD & ~0x30) | TIMx->TIM_Mode; //工作模式,0: 16位自动重装, 1: 16位定时/计数, 2: 8位自动重装
		if (TIMx->TIM_ClkSource == TIM_CLOCK_12T)
			Timer0_12T(); // 12T
		if (TIMx->TIM_ClkSource == TIM_CLOCK_1T)
			Timer0_1T(); // 1T
		if (TIMx->TIM_ClkSource == TIM_CLOCK_Ext)
			Timer0_AsCounter(); //对外计数或分频
		else
			Timer0_AsTimer(); //定时
		if (TIMx->TIM_ClkOut == ENABLE)
			Timer0_CLKO_Enable(); //输出时钟
		else
			Timer0_CLKO_Disable(); //不输出时钟

		T0_Load(TIMx->TIM_Value);
		if (TIMx->TIM_Run == ENABLE)
			Timer0_Run(); //开始运行
		return 0;		  //成功
	}
	if (TIM == Timer1)
	{
		Timer1_Stop(); //停止计数
		if (TIMx->TIM_Interrupt == ENABLE)
			Timer1_InterruptEnable(); //允许中断
		else
			Timer1_InterruptDisable(); //禁止中断
		if (TIMx->TIM_Priority > Priority_3)
			return 2;						 //错误
		Timer1_Priority(TIMx->TIM_Priority); //指定中断优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
		if (TIMx->TIM_Mode >= TIM_16BitAutoReloadNoMask)
			return 2;							//错误
		TMOD = (TMOD & ~0x30) | TIMx->TIM_Mode; //工作模式,0: 16位自动重装, 1: 16位定时/计数, 2: 8位自动重装
		if (TIMx->TIM_ClkSource == TIM_CLOCK_12T)
			Timer1_12T(); // 12T
		if (TIMx->TIM_ClkSource == TIM_CLOCK_1T)
			Timer1_1T(); // 1T
		if (TIMx->TIM_ClkSource == TIM_CLOCK_Ext)
			Timer1_AsCounter(); //对外计数或分频
		else
			Timer1_AsTimer(); //定时
		if (TIMx->TIM_ClkOut == ENABLE)
			Timer1_CLKO_Enable(); //输出时钟
		else
			Timer1_CLKO_Disable(); //不输出时钟

		T1_Load(TIMx->TIM_Value);
		if (TIMx->TIM_Run == ENABLE)
			Timer1_Run(); //开始运行
		return 0;		  //成功
	}
	return 2; //错误
}

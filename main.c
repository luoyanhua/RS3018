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

#include "config.h"
#include "GPIO.h"
#include "UART.h"
#include "delay.h"
#include "WDT.h"
#include "timer.h"
#include "Sensor.h"
#include "Beep.h"
#include "Task.h"
#include "Exti.h"

/*************   功能说明   ***************

本例程基于STC8G1K08-QFN20进行编写测试

PCA0  使用定时器0获得40KHz输出
捕获脉宽时间=捕捉的时钟数/PCA时钟源, 比如 捕捉的时钟数 = 256, PCA时钟源 = 20MHz(1T), 脉宽 = 256/20MHz = 12.8us. 
下载时, 选择时钟 20MHz (用户可在"config.h"修改频率).

******************************************/

/*************	本地常量声明	**************/

/*************	本地变量声明	**************/

/*************	本地函数声明	**************/

/*************  外部函数和变量声明 *****************/

/******************** IO口配置 ********************/
void GPIO_config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; //结构定义

	GPIO_InitStructure.Pin = GPIO_Pin_All;		//指定要初始化的IO, GPIO_Pin_0 ~ GPIO_Pin_7, 或操作
	GPIO_InitStructure.Mode = GPIO_PullUp;		//指定IO的输入或输出方式,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P1, &GPIO_InitStructure); //初始化

	GPIO_InitStructure.Pin = GPIO_Pin_All;		//指定要初始化的IO, GPIO_Pin_0 ~ GPIO_Pin_7, 或操作
	GPIO_InitStructure.Mode = GPIO_OUT_PP;		//指定IO的输入或输出方式,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P3, &GPIO_InitStructure); //初始化
}

/***************  串口初始化函数 *****************/
void UART_config(void)
{
	COMx_InitDefine COMx_InitStructure; //结构定义

	COMx_InitStructure.UART_Mode = UART_8bit_BRTx;	 //模式,       UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	COMx_InitStructure.UART_BRT_Use = BRT_Timer1;	 //选择波特率发生器,   BRT_Timer1, BRT_Timer2 (注意: 串口2固定使用BRT_Timer2)
	COMx_InitStructure.UART_BaudRate = 115200UL;	 //波特率, 一般 110 ~ 115200
	COMx_InitStructure.UART_RxEnable = ENABLE;		 //接收允许,   ENABLE或DISABLE
	COMx_InitStructure.BaudRateDouble = DISABLE;	 //波特率加倍, ENABLE或DISABLE
	COMx_InitStructure.UART_Interrupt = ENABLE;		 //中断允许,   ENABLE或DISABLE
	COMx_InitStructure.UART_Priority = Priority_0;	 //指定中断优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
	COMx_InitStructure.UART_P_SW = UART1_SW_P30_P31; //切换端口,   UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17,UART1_SW_P43_P44
	UART_Configuration(UART1, &COMx_InitStructure);	 //初始化串口1 UART1,UART2,UART3,UART4

	COMx_InitStructure.UART_Mode = UART_8bit_BRTx;	 //模式,       UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
													 //	COMx_InitStructure.UART_BRT_Use   = BRT_Timer2;			//选择波特率发生器,   BRT_Timer1, BRT_Timer2 (注意: 串口2固定使用BRT_Timer2)
	COMx_InitStructure.UART_BaudRate = 4800UL;		 //波特率, 一般 110 ~ 115200
	COMx_InitStructure.UART_RxEnable = DISABLE;		 //接收允许,   ENABLE或DISABLE
	COMx_InitStructure.BaudRateDouble = DISABLE;	 //波特率加倍, ENABLE或DISABLE
	COMx_InitStructure.UART_Interrupt = DISABLE;	 //中断允许,   ENABLE或DISABLE
	COMx_InitStructure.UART_Priority = Priority_0;	 //指定中断优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
	COMx_InitStructure.UART_P_SW = UART2_SW_P10_P11; //切换端口, UART2_SW_P10_P11,UART2_SW_P46_P47
	UART_Configuration(UART2, &COMx_InitStructure);	 //初始化串口1 UART1,UART2,UART3,UART4

	PrintString1("STC8 UART1 OK!\r\n"); //UART1发送一个字符串
	PrintString2("STC8 UART2 OK!\r\n"); //UART1发送一个字符串
}

/************************ 定时器配置 ****************************/
void Timer_config(void)
{
	TIM_InitTypeDef TIM_InitStructure;							   //结构定义
	TIM_InitStructure.TIM_Mode = TIM_16BitAutoReload;			   //指定工作模式,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_Priority = Priority_0;				   //指定中断优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
	TIM_InitStructure.TIM_Interrupt = ENABLE;					   //中断是否允许,   ENABLE或DISABLE
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_1T;				   //指定时钟源,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut = DISABLE;						   //是否输出高速脉冲, ENABLE或DISABLE
	TIM_InitStructure.TIM_Value = 65536UL - (MAIN_Fosc / 80000UL); //初值,    80KHz   12.5us中断一次
	TIM_InitStructure.TIM_Run = ENABLE;							   //是否初始化后启动定时器, ENABLE或DISABLE
	Timer_Inilize(Timer0, &TIM_InitStructure);					   //初始化Timer0	  Timer0,Timer1,Timer2,Timer3,Timer4

	TIM_InitStructure.TIM_Mode = TIM_16BitAutoReload;			  //指定工作模式,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_Priority = Priority_0;				  //指定中断优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
	TIM_InitStructure.TIM_Interrupt = ENABLE;					  //中断是否允许,   ENABLE或DISABLE
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_1T;				  //指定时钟源, TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut = DISABLE;						  //是否输出高速脉冲, ENABLE或DISABLE
	TIM_InitStructure.TIM_Value = 65536UL - (MAIN_Fosc / 2000UL); //初值,    2KHz   0.5mS中断一次
	TIM_InitStructure.TIM_Run = ENABLE;							  //是否初始化后启动定时器, ENABLE或DISABLE
	Timer_Inilize(Timer1, &TIM_InitStructure);					  //初始化Timer1	  Timer0,Timer1,Timer2,Timer3,Timer4
}

/******************** INT配置 ********************/
void Exti_config(void)
{
	EXTI_InitTypeDef Exti_InitStructure; //结构定义

	Exti_InitStructure.EXTI_Interrupt = ENABLE;	   //中断使能,   ENABLE或DISABLE
	Exti_InitStructure.EXTI_Mode = EXT_MODE_Fall;  //中断模式,   EXT_MODE_RiseFall,EXT_MODE_Fall
	Exti_InitStructure.EXTI_Priority = Priority_0; //指定中断优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
	Ext_Inilize(EXT_INT1, &Exti_InitStructure);	   //初始化
}

/******************** WDT配置 ********************/
void WDT_config(void)
{
	WDT_InitTypeDef WDT_InitStructure; //结构定义

	WDT_InitStructure.WDT_Enable = ENABLE;			 //中断使能   ENABLE或DISABLE
	WDT_InitStructure.WDT_IDLE_Mode = WDT_IDLE_STOP; //IDLE模式是否停止计数		WDT_IDLE_STOP,WDT_IDLE_RUN
	WDT_InitStructure.WDT_PS = WDT_SCALE_16;		 //看门狗定时器时钟分频系数		WDT_SCALE_2,WDT_SCALE_4,WDT_SCALE_8,WDT_SCALE_16,WDT_SCALE_32,WDT_SCALE_64,WDT_SCALE_128,WDT_SCALE_256
	WDT_Inilize(&WDT_InitStructure);				 //初始化
}

/******************** task A **************************/
void main(void)
{
	GPIO_config();
	UART_config();
	Timer_config();
	WDT_config();
	Exti_config();
	EA = 1;

	while (1)
	{
		AppTask();	//总流程处理任务
		BeepAlarmTask();	//蜂鸣器报警任务
		SendOncePlusTask();	//发送脉冲群任务
		WDT_Clear(); // 喂狗
	}
}

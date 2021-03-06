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

	GPIO_InitStructure.Pin = GPIO_Pin_0;		//指定要初始化的IO, GPIO_Pin_0 ~ GPIO_Pin_7, 或操作
	GPIO_InitStructure.Mode = GPIO_HighZ;		//指定IO的输入或输出方式,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P1, &GPIO_InitStructure); //初始化

	GPIO_InitStructure.Pin = GPIO_Pin_All;		//指定要初始化的IO, GPIO_Pin_0 ~ GPIO_Pin_7, 或操作
	GPIO_InitStructure.Mode = GPIO_PullUp;		//指定IO的输入或输出方式,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P3, &GPIO_InitStructure); //初始化

	GPIO_InitStructure.Pin = GPIO_Pin_4;		//指定要初始化的IO, GPIO_Pin_0 ~ GPIO_Pin_7, 或操作
	GPIO_InitStructure.Mode = GPIO_HighZ;		//指定IO的输入或输出方式,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P3, &GPIO_InitStructure); //初始化

	GPIO_InitStructure.Pin = GPIO_Pin_5 | GPIO_Pin_2; //指定要初始化的IO, GPIO_Pin_0 ~ GPIO_Pin_7, 或操作
	GPIO_InitStructure.Mode = GPIO_OUT_PP;			  //指定IO的输入或输出方式,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P3, &GPIO_InitStructure);		  //初始化

	UART_CHG_IO = UART_RX_EN;
}

/************************ 定时器配置 ****************************/
void Timer_config(void)
{
	TIM_InitTypeDef TIM_InitStructure;							   //结构定义
	TIM_InitStructure.TIM_Mode = TIM_16BitAutoReload;			   //指定工作模式,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_Priority = Priority_1;				   //指定中断优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
	TIM_InitStructure.TIM_Interrupt = DISABLE;					   //中断是否允许,   ENABLE或DISABLE
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_1T;				   //指定时钟源,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut = DISABLE;						   //是否输出高速脉冲, ENABLE或DISABLE
	TIM_InitStructure.TIM_Value = 65536UL - (MAIN_Fosc / 80000UL); //初值,    80KHz   12.5us中断一次
	TIM_InitStructure.TIM_Run = ENABLE;							   //是否初始化后启动定时器, ENABLE或DISABLE
	Timer_Inilize(Timer0, &TIM_InitStructure);					   //初始化Timer0	  Timer0,Timer1,Timer2,Timer3,Timer4

	TIM_InitStructure.TIM_Mode = TIM_16BitAutoReload;			//指定工作模式,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_Priority = Priority_0;				//指定中断优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
	TIM_InitStructure.TIM_Interrupt = ENABLE;					//中断是否允许,   ENABLE或DISABLE
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_1T;				//指定时钟源, TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut = DISABLE;						//是否输出高速脉冲, ENABLE或DISABLE
	TIM_InitStructure.TIM_Value = 65536UL - (MAIN_Fosc / 4000); //初值,    250us中断一次
	TIM_InitStructure.TIM_Run = ENABLE;							//是否初始化后启动定时器, ENABLE或DISABLE
	Timer_Inilize(Timer1, &TIM_InitStructure);					//初始化Timer1	  Timer0,Timer1,Timer2,Timer3,Timer4
}

/******************** WDT配置 ********************/
void WDT_config(void)
{
	WDT_InitTypeDef WDT_InitStructure; //结构定义

	WDT_InitStructure.WDT_Enable = ENABLE;			 //中断使能   ENABLE或DISABLE
	WDT_InitStructure.WDT_IDLE_Mode = WDT_IDLE_STOP; // IDLE模式是否停止计数		WDT_IDLE_STOP,WDT_IDLE_RUN
	WDT_InitStructure.WDT_PS = WDT_SCALE_16;		 //看门狗定时器时钟分频系数		WDT_SCALE_2,WDT_SCALE_4,WDT_SCALE_8,WDT_SCALE_16,WDT_SCALE_32,WDT_SCALE_64,WDT_SCALE_128,WDT_SCALE_256
	WDT_Inilize(&WDT_InitStructure);				 //初始化
}

/******************** task A **************************/
void main(void)
{
	GPIO_config();
	Timer_config();
	WDT_config();
	EA = 1;

	while (1)
	{
		sensorIdAdjustTask();	 //探头位置识别任务
		AppTask();				 //总流程处理任务
		uartBuffSendTask();		 //串口数据发送任务
		uartBuffRxTask();		 //串口数据接收任务
		BeepAlarmTask();		 //蜂鸣器报警任务
		SendOncePlusTask();		 //发送脉冲群任务
		sensorDistanceGetTask(); //距离测量任务
		WDT_Clear();			 // 喂狗
	}
}

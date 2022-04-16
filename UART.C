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

#include "Task.h"
#include "timer.h"
#include "Beep.h"
#include "Task.h"
#include "Sensor.h"
#include "delay.h"
#include "UART.h"
#include <string.h>

COMx_Define COM2;
u8 xdata TX2_Buffer[COM_TX2_Lenth]; //发送缓冲
u8 xdata RX2_Buffer[COM_RX2_Lenth]; //接收缓冲

u8 UART_Configuration(u8 UARTx, COMx_InitDefine *COMx)
{
	u8 i;
	u32 j;

	if (UARTx == UART2)
	{
		COM2.id = 2;
		COM2.TX_read = 0;
		COM2.TX_write = 0;
		COM2.B_TX_busy = 0;
		COM2.RX_Cnt = 0;
		COM2.RX_TimeOut = 0;
		COM2.B_RX_OK = 0;
		for (i = 0; i < COM_TX2_Lenth; i++)
			TX2_Buffer[i] = 0;
		for (i = 0; i < COM_RX2_Lenth; i++)
			RX2_Buffer[i] = 0;

		if ((COMx->UART_Mode == UART_9bit_BRTx) || (COMx->UART_Mode == UART_8bit_BRTx)) //可变波特率
		{
			if (COMx->UART_Priority > Priority_3)
				return 2;						 //错误
			UART2_Priority(COMx->UART_Priority); //指定中断优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
			if (COMx->UART_Mode == UART_9bit_BRTx)
				S2CON |= (1 << 7); // 9bit
			else
				S2CON &= ~(1 << 7);					   // 8bit
			j = (MAIN_Fosc / 4) / COMx->UART_BaudRate; //按1T计算
			if (j >= 65536UL)
				return 2; //错误
			j = 65536UL - j;
			AUXR &= ~(1 << 4); // Timer stop
			AUXR &= ~(1 << 3); // Timer2 set As Timer
			AUXR |= (1 << 2);  // Timer2 set as 1T mode
			TH2 = (u8)(j >> 8);
			TL2 = (u8)j;
			IE2 &= ~(1 << 2); //禁止中断
			AUXR |= (1 << 4); // Timer run enable
		}
		else
			return 2; //模式错误
		if (COMx->UART_Interrupt == ENABLE)
			IE2 |= 1; //允许中断
		else
			IE2 &= ~1; //禁止中断
		if (COMx->UART_RxEnable == ENABLE)
			S2CON |= (1 << 4); //允许接收
		else
			S2CON &= ~(1 << 4);							 //禁止接收
		P_SW2 = (P_SW2 & ~1) | (COMx->UART_P_SW & 0x01); //切换IO
		return 0;
	}
	return 2; //错误
}

/*********************************************************/

/***************  串口初始化函数 *****************/
void UART_config(void)
{
	COMx_InitDefine COMx_InitStructure;				 //结构定义
	COMx_InitStructure.UART_Mode = UART_8bit_BRTx;	 //模式,   UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
													 //	COMx_InitStructure.UART_BRT_Use   = BRT_Timer2;			//选择波特率发生器, BRT_Timer2 (注意: 串口2固定使用BRT_Timer2, 所以不用选择)
	COMx_InitStructure.UART_BaudRate = 9600ul;		 //波特率,     110 ~ 115200
	COMx_InitStructure.UART_RxEnable = ENABLE;		 //接收允许,   ENABLE或DISABLE
	COMx_InitStructure.UART_Interrupt = ENABLE;		 //中断允许,   ENABLE或DISABLE
	COMx_InitStructure.UART_Priority = Priority_0;	 //指定中断优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
	COMx_InitStructure.UART_P_SW = UART2_SW_P10_P11; //切换端口,   UART2_SW_P10_P11,UART2_SW_P46_P47
	UART_Configuration(UART2, &COMx_InitStructure);	 //初始化串口2 UART1,UART2,UART3,UART4
}

void UART2_int(void) interrupt UART2_VECTOR
{
	if (RI2)
	{
		CLR_RI2();
		if (COM2.B_RX_OK == 0)
		{
			if (COM2.RX_Cnt >= COM_RX2_Lenth)
				COM2.RX_Cnt = 0;
			RX2_Buffer[COM2.RX_Cnt++] = S2BUF;
			COM2.RX_TimeOut = TimeOutSet2;
		}
	}

	if (TI2)
	{
		CLR_TI2();
		if (COM2.TX_read != COM2.TX_write)
		{
			S2BUF = TX2_Buffer[COM2.TX_read];
			if (++COM2.TX_read >= COM_TX2_Lenth)
				COM2.TX_read = 0;
		}
		else
			COM2.B_TX_busy = 0;
	}
}

/********************* UART2 函数 ************************/
void TX2_write2buff(u8 dat) //写入发送缓冲，指针+1
{
	TX2_Buffer[COM2.TX_write] = dat; //装发送缓冲
	if (++COM2.TX_write >= COM_TX2_Lenth)
		COM2.TX_write = 0;

	if (COM2.B_TX_busy == 0) //空闲
	{
		COM2.B_TX_busy = 1; //标志忙
		SET_TI2();			//触发发送中断
	}
}

void VirtualCOM_StringSend(unsigned char *str)
{
	//    while (*str != 0)
	//    {
	//        TX2_write2buff(*str);
	//        str++;
	//    }
}

void clrRX2_Buffer(void)
{
	COM2.id = 2;
	COM2.TX_read = 0;
	COM2.TX_write = 0;
	COM2.B_TX_busy = 0;
	COM2.RX_Cnt = 0;
	COM2.RX_TimeOut = 0;
	COM2.B_RX_OK = 0;
}

unsigned char getRxSensorId(void)
{
	return RX2_Buffer[2];
}

void uartSendBuf(unsigned char *buf, unsigned char len)
{
	unsigned char i = 0;
	for (i = 0; i < len; i++)
	{
		TX2_write2buff(buf[i]);
	}
}

unsigned char txBuf[14];
unsigned char currentSensorIDValue = 0;

unsigned char get_currentSensorIDValue(void)
{
	return currentSensorIDValue;
}

void uartSendPackage(unsigned char mode)
{
	unsigned char temp_sendLen = 0;
	if (mode == RES) //预留
	{
		txBuf[0] = 0xAA;
	}
	else if(mode == PWM_CHECK)	//PWM检测
	{
		txBuf[0] = 0xBB;
		txBuf[1] = 4;
		txBuf[2] = 0xBF;		
		temp_sendLen = 3;
		currentSensorIDValue = txBuf[1];
	}
	else if (mode == SELF_CHECK) //自检
	{
		txBuf[0] = 0xBB;
		txBuf[2] = 0xBF;
		if(getRxSensorId() == 4)
		{
			txBuf[1] = 8;	
		}
		else if(getRxSensorId() == 8)
		{
			txBuf[1] = 2;	
		}	
		else if(getRxSensorId() == 2)
		{
			txBuf[1] = 1;	
		}				
		temp_sendLen = 3;
		currentSensorIDValue = txBuf[1];
	}
	else if (mode == NOM_WORK) //工作
	{
		memset(txBuf, 0, 14);
		txBuf[0] = 0xCC;
		txBuf[12] = Get_meterDistance() / 10;
		txBuf[13] = 0xcf;
		temp_sendLen = 14;
	}
	uartSendBuf(txBuf, temp_sendLen);
}

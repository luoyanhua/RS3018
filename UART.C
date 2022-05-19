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

unsigned char xdata txBuf[14];
unsigned char xdata rxbuf[COM_RX2_Lenth];
unsigned char rxlen = 0;

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
		COM2.B_TX_busy = 0;
	}
}

/********************* UART2 函数 ************************/
void TX2_write2buff(u8 dat) //写入发送缓冲，指针+1
{
	
	while(COM2.B_TX_busy){};
	COM2.B_TX_busy = 1; //标志忙	
	S2BUF = dat; //装发送缓冲
	CLR_TI2();
}

void VirtualCOM_StringSend(unsigned char *str)
{

}

/***************  串口初始化函数 *****************/
void UART_config(void)
{
	COMx_InitDefine COMx_InitStructure;				 //结构定义
	COMx_InitStructure.UART_Mode = UART_8bit_BRTx;	 //模式,   UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
													 //	COMx_InitStructure.UART_BRT_Use   = BRT_Timer2;			//选择波特率发生器, BRT_Timer2 (注意: 串口2固定使用BRT_Timer2, 所以不用选择)
	COMx_InitStructure.UART_BaudRate = 115200ul;		 //波特率,     110 ~ 115200
	COMx_InitStructure.UART_RxEnable = ENABLE;		 //接收允许,   ENABLE或DISABLE
	COMx_InitStructure.UART_Interrupt = ENABLE;		 //中断允许,   ENABLE或DISABLE
	COMx_InitStructure.UART_Priority = Priority_0;	 //指定中断优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
	COMx_InitStructure.UART_P_SW = UART2_SW_P10_P11; //切换端口,   UART2_SW_P10_P11,UART2_SW_P46_P47
	UART_Configuration(UART2, &COMx_InitStructure);	 //初始化串口2 UART1,UART2,UART3,UART4

	UART_CHG_IO = UART_RX_EN;
}

//清除接收缓存
void clrRX2_Buffer(void)
{
	COM2.RX_Cnt = 0;
	COM2.B_RX_OK = 0;
	memset(RX2_Buffer, 0, COM_TX2_Lenth);
}

//获取接收到的数据 0:未收到有效数据
unsigned char getRxBuf(void)
{
	rxlen = COM2.RX_Cnt;
	if (rxlen == 4 || rxlen == 5) //目前只有两种协议
	{
		memcpy(rxbuf, RX2_Buffer, rxlen);
		return rxlen;
	}
	else
		return 0;
}

bit uartUsedFlag = 0;

bit get_uartUsedFlag(void)
{
	return uartUsedFlag;
}

void uartBuffSenfTask(void)
{
	if(Get_plusOutFlag() == 0)	//避免串口和高速定时器同时工作，影响串口数据
	{
		unsigned char i = 0;
		uartUsedFlag = 1;
		clrRX2_Buffer();
		UART_CHG_IO = UART_TX_EN;
		_nop_();
		_nop_();
		_nop_();
		_nop_();	
		for (i = 0; i < COM2.TX_write; i++)
		{
			TX2_write2buff(TX2_Buffer[i]);
		}
		while(COM2.B_TX_busy){};
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		COM2.TX_write = 0;			
		UART_CHG_IO = UART_RX_EN;
		uartUsedFlag = 0;
	}
}

void uartSendBuf(unsigned char *buf, unsigned char len)
{
	COM2.TX_write = 0;
	if(len == 0) return;
	memcpy(TX2_Buffer,buf,len);
	COM2.TX_write = len;
}

unsigned char saveTotalSensorDistance[SENSOR_NUM_MAX] = {0}; //顺序分别代表：左，左中，右中，右

unsigned char get_distanceLevel(unsigned char value)
{
	unsigned char temp_distanceLevel;
	if(value > 200)
	{
		temp_distanceLevel = 5;
	}
	else if(value > 155)
	{
		temp_distanceLevel = 4;
	}
	else if(value > 110)
	{
		temp_distanceLevel = 3;
	}
	else if(value > 60)
	{
		temp_distanceLevel = 2;
	}
	else
	{
		temp_distanceLevel = 1;
	}
	return temp_distanceLevel;
}

void sensorTotalPackage(void)
{
	txBuf[0] = 0xAA;
	txBuf[1] = 4;
	txBuf[2] = get_distanceLevel(saveTotalSensorDistance[0]);
	txBuf[3] = saveTotalSensorDistance[0];
	txBuf[4] = get_distanceLevel(saveTotalSensorDistance[1]);
	txBuf[5] = saveTotalSensorDistance[1];
	txBuf[6] = get_distanceLevel(saveTotalSensorDistance[2]);
	txBuf[7] = saveTotalSensorDistance[2];
	txBuf[8] = get_distanceLevel(Get_meterDistance()/10);
	txBuf[9] = Get_meterDistance()/10;	
	txBuf[10] = 0xAF;	
	uartSendBuf(txBuf,11);	
}

void sensorReplyPackage(unsigned char ch, unsigned char cmd)
{
	unsigned char temp_txLen = 0;
	if(cmd == CMD_ID)
	{
		txBuf[0] = 0xBB;
		txBuf[1] = cmd;
		txBuf[2] = ch;
		txBuf[3] = 0xBF;
		temp_txLen = 4;
	}
	else
	{
		txBuf[0] = 0xBB;
		txBuf[1] = cmd;
		txBuf[2] = ch;
		txBuf[3] = Get_meterDistance()/10;
		txBuf[4] = 0xBF;	
		temp_txLen = 5;	
	}
	uartSendBuf(txBuf,temp_txLen);
}


// cmd 0:右传感器循环获取其他传感器位置信息 1:右传感器循环获取其他传感器距离信息
// ch 根据宏定义来
void getSensorImfo(unsigned char ch, unsigned char cmd)
{
	txBuf[0] = 0xBB;
	txBuf[1] = cmd;
	txBuf[2] = ch;
	txBuf[3] = 0xBF;
	uartSendBuf(txBuf, 4);
}

//接收数据解析函数
// buf:接收数据指针
// len:接收数据长度
//格式：BB + cmd + ch + distance(BYTE) + BF
unsigned char analysisSensorImfo(void)
{
	unsigned char result = 0;
	if(getRxBuf() ==  0) return 0;
	
	if (get_currentSensorID() == RIGHT_SENSOR)
	{
		if (rxbuf[1] == CMD_ID)
		{
			if (rxbuf[2] == LEFT_SENSOR)
			{
				result = 1;//传感器存在
			}
			else if (rxbuf[2] == LEFT_MID_SENSOR)
			{
				result = 1;//传感器存在
			}
			else if (rxbuf[2] == RIGHT_MID_SENSOR)
			{
				result = 1;//传感器存在
			}
		}
		else
		{
			if (rxbuf[2] == LEFT_SENSOR)
			{
				saveTotalSensorDistance[0] = rxbuf[3] ;
				result = 1;//解析距离
			}
			else if (rxbuf[2] == LEFT_MID_SENSOR)
			{
				saveTotalSensorDistance[1] = rxbuf[3] ;
				result = 1;//解析距离
			}
			else if (rxbuf[2] == RIGHT_MID_SENSOR)
			{
				saveTotalSensorDistance[2] = rxbuf[3] ;
				result = 1;//解析距离
			}
		}
	}
	else
	{
		if (rxbuf[1] == CMD_ID)
		{
			if (rxbuf[2] == get_currentSensorID()) //判断收到的信息是否需要回复，回复必须传感器号对应
			{
				sensorReplyPackage(rxbuf[2] ,rxbuf[1]);
				result = 1;//传感器存在，需要回复信息
			}
		}
		else
		{
			if (rxbuf[2] == get_currentSensorID()) //判断收到的信息是否需要回复，回复必须传感器号对应
			{
				sensorReplyPackage(rxbuf[2] ,rxbuf[1]);
				result = 1;//需要回复距离信息
			}
		}
	}
	return result;
}

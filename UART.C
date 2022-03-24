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
#include "Uart.h"
#include "delay.h"

unsigned char txBuf[14];

void IO_LOW(unsigned char comTemp)
{
    if (comTemp == COM0)
        P10 = 0;
    else
        P11 = 0;
}

void IO_HIGH(unsigned char comTemp)
{
    if (comTemp == COM0)
        P10 = 1;
    else
        P11 = 1;
}

void VirtualCOM_ByteSend(unsigned char com, unsigned char val)
{

    unsigned char i;

    IO_LOW(com); //起始位，拉低电平

    Delay208us();

    for (i = 0; i < 8; i++) // 8位数据位

    {

        if (val & 0x01)

            IO_HIGH(com);

        else

            IO_LOW(com);

        Delay208us();

        val >>= 1;
    }

    IO_HIGH(com); //停止位，拉高电平

    Delay208us();
}

void VirtualCOM_StringSend(unsigned char com, unsigned char *str)
{
    while (*str != 0)
    {
        VirtualCOM_ByteSend(com, *str);
        str++;
    }
}

void uartSendBuf(unsigned char *buf, unsigned char len)
{
    unsigned char i = 0;
    for(i = 0; i < len; i++)
    {
        VirtualCOM_ByteSend(COM1, buf[i]);
    }
}

void uartSendPackage(unsigned char mode)
{
    if (mode == RES) //预留
    {
        txBuf[0] = 0xAA;
    }
    else if (mode == SELF_CHECK) //自检
    {
        txBuf[0] = 0xBB;
        txBuf[1] = Get_sensorOkFlag();
        txBuf[2] = 0xBF;
        uartSendBuf(txBuf,3);
    }
    else if (mode == NOM_WORK) //工作
    {
        txBuf[0] = 0xCC;
        txBuf[12] = Get_meterDistance()/10;
        txBuf[13] = 0xcf;
        uartSendBuf(txBuf,14);
    }
}

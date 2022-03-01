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

#define IO_LOW() P11 = 0
#define IO_HIGH() P11 = 1

void VirtualCOM_ByteSend(unsigned char val)
{

    unsigned char i;

    IO_LOW(); //起始位，拉低电平

    Delay208us();

    for (i = 0; i < 8; i++) //8位数据位

    {

        if (val & 0x01)

            IO_HIGH();

        else

            IO_LOW();

        Delay208us();

        val >>= 1;
    }

    IO_HIGH(); //停止位，拉高电平

    Delay208us();
}

void VirtualCOM_StringSend(unsigned char *str)
{
    while (*str != 0)
    {
        VirtualCOM_ByteSend(*str);
        str++;
    }
}


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
#include "UART.h"
#include "stdio.h"

unsigned char appTaskState = 0;
unsigned int appTaskTimeCnt = 0;

void AppTask(void)
{
    switch (appTaskState)
    {
    case 0: //发送PWM 0.5S 用来确认位置信号
        StartBeepAlarm(500);
        appTaskTimeCnt = Get_SysHalfMsTick();
        appTaskState = 1;
        break;
    case 1: //Sensor ID会收到自己发的信号
        if (P10 == 0)
        {
			PrintString1("recv ID!\r\n"); //UART1发送一个字符串
            appTaskState = 2;
        }
        else if (get_time_escape_sec(Get_SysHalfMsTick(), appTaskTimeCnt) >= 20) //10ms超时后继续往下进行
        {
            appTaskState = 2;
        }
        break;
    case 2: //开机探头自检
        Start_sendOncePlusTask();
        appTaskState = 3;
        break;
    case 3: //获取探头自检结果
        if (Get_SendOncePlusTaskFlag() == 0)
        {
            if (Get_sensorOkFlag())
            {
				PrintString1("sensor is ok!\r\n"); //UART1发送一个字符串
                appTaskState = 4; //检测到探头
            }
            else
            {
				PrintString1("No sensor!\r\n"); //UART1发送一个字符串
                appTaskState = 0; //无探头从头开始
            }
        }
        break;
    case 4: //开始测距结果
        Start_sendOncePlusTask();
        appTaskTimeCnt = Get_SysHalfMsTick();
        appTaskState = 5;
        break;
    case 5: //判断是否有障碍物
        if (Get_SendOncePlusTaskFlag() == 0)
        {
            if (Get_obstaclesExistenceFlag()) //有障碍物
            {
                if (get_time_escape_sec(Get_SysHalfMsTick(), appTaskTimeCnt) >= 100) //50ms测试一次
                {
					PrintString1("there is obstacle!\r\n"); //UART1发送一个字符串
					printf("d=%d mm",Get_meterDistance());
                    appTaskState = 4;
                }
            }
            else //没有障碍物
            {
                if (get_time_escape_sec(Get_SysHalfMsTick(), appTaskTimeCnt) >= 400) //200ms测试一次
                {
                    //获取测试距离
					PrintString1("No obstacles!\r\n"); //UART1发送一个字符串
                    appTaskState = 4;
                }
            }
        }
        break;
    default:
        break;
    }
}
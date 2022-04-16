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
#include "string.h"

#define SENSOR_NUM_MAX 4

typedef struct
{
    bit runFlag;                           //运行标志
    bit adjustOKFlag;                      //判断OK标志
    bit ioStatusSaveValue;                 // IO口状态保存
    bit saveTotalSensorID[SENSOR_NUM_MAX]; //当前所有设备ID位置,如果为右侧传感器，则自定义为主传感器
    unsigned char state;                   //运行状态机
    unsigned int TimeCnt;                  //定时器
} sensorIdAdjustStruct_T;

sensorIdAdjustStruct_T sensorIdAdjustStruct;

void sensorIdAdjustTask_init(void)
{
    memset(&sensorIdAdjustStruct, 0, sizeof(sensorIdAdjustStruct));
}

unsigned char appTaskState = 0;
unsigned int appTaskTimeCnt = 0;

void sensorIdAdjustTask(void)
{
    if (sensorIdAdjustStruct.runFlag == 1)
        return;
    switch (sensorIdAdjustStruct.state)
    {
    case 0: //发送PWM 150ms 用来确认位置信号
        StartBeepAlarm(150);
        sensorIdAdjustStruct.TimeCnt = Get_SysHalfMsTick(); //开启计时器
        sensorIdAdjustStruct.state = 1;
        break;
    case 1: // 判断右中探头位置，只有右侧探头会收到自己发的信号
        //开启P10接收PWM
        if (get_time_escape_sec(Get_SysHalfMsTick(), sensorIdAdjustStruct.TimeCnt) >= 200) // 200ms超时后继续往下进行
        {
            if (sensorIdAdjustStruct.adjustOKFlag == 1) //收到PWM
            {
                sensorIdAdjustStruct.adjustOKFlag = 0;
                uartSendPackage(PWM_CHECK); //定义自己为右中探头，发送右中探头的ID识别信息
            }
            sensorIdAdjustStruct.state = 2;
        }
        else
        {
            if (P10 == 1)
            {
                sensorIdAdjustStruct.ioStatusSaveValue = 1;
            }
            if (sensorIdAdjustStruct.ioStatusSaveValue == 1 && P10 == 0)
            {
                sensorIdAdjustStruct.adjustOKFlag = 1;
            }
        }
        break;
    case 2:                                                                                // 判断右侧探头位置
        if (get_time_escape_sec(Get_SysHalfMsTick(), sensorIdAdjustStruct.TimeCnt) >= 400) // 200ms超时后继续往下进行
        {
            if (getRxSensorId() == 4) //收到右中探头的ID识别信息
            {
                sensorIdAdjustStruct.ioStatusSaveValue = 1;
                uartSendPackage(SELF_CHECK);    //定义自己为右侧探头，发送右侧探头身份
                sensorIdAdjustStruct.state = 5; //跳转到等待1S计时完成
            }
            else
            {
                sensorIdAdjustStruct.state = 3;
            }
        }
        break;
    case 3:                                                                                // 判断左中探头位置
        if (get_time_escape_sec(Get_SysHalfMsTick(), sensorIdAdjustStruct.TimeCnt) >= 600) // 200ms超时后继续往下进行
        {
            if (sensorIdAdjustStruct.adjustOKFlag == 1) //判断连续低电平，左中
            {

                uartSendPackage(SELF_CHECK);    //定义自己为左中探头，发送左中探头身份
                sensorIdAdjustStruct.state = 5; //跳转到等待1S计时完成
            }
            else
            {
                sensorIdAdjustStruct.state = 4;
            }
        }
        else
        {
            if (P10 == 1)
            {
                sensorIdAdjustStruct.adjustOKFlag = 0;
            }
        }
        break;
    case 4:                                                                                // 判断左侧探头位置
        if (get_time_escape_sec(Get_SysHalfMsTick(), sensorIdAdjustStruct.TimeCnt) >= 800) // 200ms超时后继续往下进行
        {
            if (sensorIdAdjustStruct.adjustOKFlag == 1) //判断连续高电平，左边
            {
                uartSendPackage(SELF_CHECK);    //定义自己为左侧探头，发送左侧探头身份
                sensorIdAdjustStruct.state = 5; //跳转到等待1S计时完成
            }
            else
            {
                sensorIdAdjustStruct.state = 4;
            }
        }
        else
        {
            if (P10 == 0)
            {
                sensorIdAdjustStruct.adjustOKFlag = 0;
            }
        }
        break;
    case 5: // 判断左侧探头位置
        sensorIdAdjustStruct.state = 0;
        sensorIdAdjustStruct.runFlag = 1; //探头位置识别完成
        break;
    }
}

void AppTask(void)
{
    if (sensorIdAdjustStruct.runFlag == 0)
        return;
    switch (appTaskState)
    {
    case 0: //发送PWM 150ms 用来确认位置信号
        StartBeepAlarm(150);
        appTaskTimeCnt = Get_SysHalfMsTick();
        appTaskState = 1;
        break;
    case 1: // Sensor ID会收到自己发的信号
        if (P34 == 0)
        {
            VirtualCOM_StringSend("recv ID!\r\n"); // UART1发送一个字符串
            appTaskState = 2;
        }
        else if (get_time_escape_sec(Get_SysHalfMsTick(), appTaskTimeCnt) >= 20) // 10ms超时后继续往下进行
        {
            VirtualCOM_StringSend("recv ID timeout!\r\n"); // UART1发送一个字符串
            appTaskState = 2;
        }
        break;
    case 2: //开机探头自检
        Start_sendOncePlusTask();
        appTaskState = 3;
        VirtualCOM_StringSend("sensor self check!\r\n"); // UART1发送一个字符串
        break;
    case 3: //获取探头自检结果
        if (Get_SendOncePlusTaskFlag() == 0)
        {
            if (Get_sensorOkFlag())
            {
                VirtualCOM_StringSend("sensor is ok!\r\n"); // UART1发送一个字符串
                appTaskState = 4;                           //检测到探头
            }
            else
            {
                VirtualCOM_StringSend("No sensor!\r\n"); // UART1发送一个字符串
                appTaskState = 0;                        //无探头从头开始
            }
            uartSendPackage(SELF_CHECK);
        }
        break;
    case 4: //开始测距结果
        appTaskTimeCnt = Get_SysHalfMsTick();
        Start_sendOncePlusTask();
        appTaskState = 5;
        break;
    case 5: //判断是否有障碍物
        if (Get_SendOncePlusTaskFlag() == 0)
        {
            if (Get_obstaclesExistenceFlag()) //有障碍物
            {
                if (get_time_escape_sec(Get_SysHalfMsTick(), appTaskTimeCnt) >= 100) // 50ms测试一次
                {
                    VirtualCOM_StringSend("obstacle!\r\n"); // UART1发送一个字符串
                    appTaskState = 4;
                    uartSendPackage(NOM_WORK);
                }
            }
            else //没有障碍物
            {
                if (get_time_escape_sec(Get_SysHalfMsTick(), appTaskTimeCnt) >= 400) // 200ms测试一次
                {
                    //获取测试距离
                    VirtualCOM_StringSend("No obstacles!\r\n"); // UART1发送一个字符串
                    appTaskState = 4;
                    uartSendPackage(NOM_WORK);
                }
            }
        }
        break;
    default:
        break;
    }
}
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
#include "stdio.h"
#include "UART.h"
#include "string.h"

#define RightSensorCheck_0 0
#define RightSensorCheck_1 1
#define RightSensorCheck_2 2
#define RightSensorCheck_3 3
#define RightSensorCheck_4 4
#define RightSensorCheck_5 5
#define RightSensorCheck_6 6
#define RightSensorCheck_7 7
#define RightSensorCheck_8 8
#define RightSensorCheck_9 9

#define WAIT_REV_CMD 10
#define WAIT_STATE 12

#define GET_SENSOR_ID_CNT_MAX 3

bit sensorIdCheckRunFlag = 0;                          //运行标志
bit sensorIdCheckOKFlag = 0;                           //判断OK标志
bit ioStatusSaveValue = 0;                             // IO口状态保存
unsigned char saveTotalSensorID[SENSOR_NUM_MAX] = {0}; //当前所有设备ID位置,如果为右侧传感器，则自定义为主传感器，顺序分别代表：左，左中，右中，右
unsigned char sensorIdCheckState = 0;                  //运行状态机
unsigned int sensorIdCheckTimeCnt = 0;                 //定时器
unsigned char sensorIdGetCnt = 0;                      // ID 获取计数
unsigned char currentSensorID = 0;                     //当前传感器值

unsigned char appTaskState = 0;
unsigned int appTaskTimeCnt = 0;

unsigned char get_currentSensorID(void)
{
    return currentSensorID;
}

void sensorIdAdjustTask(void)
{
    if (sensorIdCheckRunFlag == 1)
        return;
    switch (sensorIdCheckState)
    {
    case RightSensorCheck_0:
        PWM_OUTPUT = LOW;
        sensorIdCheckTimeCnt = Get_SysHalfMsTick(); //刷新计时器
        sensorIdCheckState = RightSensorCheck_1;
        break;
    case RightSensorCheck_1:
        if (get_time_escape_sec(Get_SysHalfMsTick(), sensorIdCheckTimeCnt) >= 20) // 等待20ms
        {
            if (SENSOR_ID_CHECK == LOW) //此时左，右中，右都满足要求,只有左中不满足要求
            {
                ioStatusSaveValue = SENSOR_ID_CHECK; //记录SENSOR_ID_CHECK电平
                sensorIdCheckState = RightSensorCheck_2;
            }
            else
            {
                currentSensorID = LEFT_MID_SENSOR;
                sensorIdCheckState = WAIT_REV_CMD; //后续判读左中是否存在
            }
        }
        break;
    case RightSensorCheck_2:
        if (get_time_escape_sec(Get_SysHalfMsTick(), sensorIdCheckTimeCnt) >= 50) // 等待30ms后再反转电平，避免电平冲突
        {
            PWM_OUTPUT = HIGH;
            sensorIdCheckState = RightSensorCheck_3;
        }
        break;
    case RightSensorCheck_3:                                                       // 判断右侧探头位置                                                      // 判断左中探头位置
        if (get_time_escape_sec(Get_SysHalfMsTick(), sensorIdCheckTimeCnt) >= 100) // 50ms超时后继续往下进行
        {
            if (SENSOR_ID_CHECK == HIGH) //此时右中，右满足要求，只有左不满足要求
            {
                if (SENSOR_ID_CHECK == ioStatusSaveValue) //两次SENSOR_ID_CHECK电平相等就是右
                {
                    currentSensorID = RIGHT_SENSOR;
                    sensorIdCheckState = RightSensorCheck_4; //右一定存在，不用判断
                }
                else //不相等就是右中
                {
                    currentSensorID = RIGHT_MID_SENSOR;
                    sensorIdCheckState = WAIT_REV_CMD; //后续判读右中是否存在
                }
            }
            else
            {
                currentSensorID = LEFT_SENSOR;
                sensorIdCheckState = WAIT_REV_CMD; //后续判读左是否存在
            }
        }
        break;
    case RightSensorCheck_4: //进入此状态机，必然为右侧探头，发送获取右中探头信息指令
        getSensorImfo(RIGHT_MID_SENSOR, CMD_ID);
        sensorIdCheckState = RightSensorCheck_5;
        sensorIdCheckTimeCnt = Get_SysHalfMsTick(); //刷新计时器
        break;
    case RightSensorCheck_5:                                                      //判断是否收到右中探头ID信息
        if (get_time_escape_sec(Get_SysHalfMsTick(), sensorIdCheckTimeCnt) >= 50) // 50ms超时后继续往下进行
        {
            if (analysisSensorImfo())
            {
                //收到右中探头ID信息
                saveTotalSensorID[2] = 1;
                sensorIdGetCnt = 0;
                sensorIdCheckState = RightSensorCheck_6;
            }
            else
            {
                sensorIdGetCnt++;
                sensorIdCheckState = RightSensorCheck_4;
                if (sensorIdGetCnt >= GET_SENSOR_ID_CNT_MAX)
                {
                    sensorIdGetCnt = 0;
                    sensorIdCheckState = RightSensorCheck_6;
                }
            }
        }
        break;
    case RightSensorCheck_6:                //发送获取左侧探头信息指令
        getSensorImfo(LEFT_SENSOR, CMD_ID); //发送获取左侧探头信息指令
        sensorIdCheckState = RightSensorCheck_7;
        sensorIdCheckTimeCnt = Get_SysHalfMsTick(); //刷新计时器
        break;
    case RightSensorCheck_7:                                                      //判断是否收到左侧探头ID信息
        if (get_time_escape_sec(Get_SysHalfMsTick(), sensorIdCheckTimeCnt) >= 50) // 50ms超时后继续往下进行
        {
            if (analysisSensorImfo())
            {
                //收到左侧探头ID信息
                saveTotalSensorID[0] = 1;
                sensorIdGetCnt = 0;
                sensorIdCheckState = RightSensorCheck_8;
            }
            else
            {
                sensorIdGetCnt++;
                sensorIdCheckState = RightSensorCheck_6;
                if (sensorIdGetCnt >= GET_SENSOR_ID_CNT_MAX)
                {
                    sensorIdGetCnt = 0;
                    sensorIdCheckState = RightSensorCheck_8;
                }
            }
        }
        break;
    case RightSensorCheck_8:                    //发送获取左中探头信息指令
        getSensorImfo(LEFT_MID_SENSOR, CMD_ID); //发送获取左中探头信息指令
        sensorIdCheckState = RightSensorCheck_9;
        sensorIdCheckTimeCnt = Get_SysHalfMsTick(); //刷新计时器
        break;
    case RightSensorCheck_9:                                                      //判断是否收到左中探头ID信息
        if (get_time_escape_sec(Get_SysHalfMsTick(), sensorIdCheckTimeCnt) >= 50) // 50ms超时后继续往下进行
        {
            if (analysisSensorImfo())
            {
                //收到左中探头ID信息
                saveTotalSensorID[1] = 1;
                sensorIdGetCnt = 0;
                sensorIdCheckState = WAIT_STATE;
            }
            else
            {
                sensorIdGetCnt++;
                sensorIdCheckState = RightSensorCheck_8;
                if (sensorIdGetCnt >= GET_SENSOR_ID_CNT_MAX)
                {
                    sensorIdGetCnt = 0;
                    sensorIdCheckState = WAIT_STATE;
                }
            }
        }
        break;
    case WAIT_REV_CMD:                                                             //进入这里，必然不是右传感器
        if (get_time_escape_sec(Get_SysHalfMsTick(), sensorIdCheckTimeCnt) >= 600) // 600ms超时后继续往下进行
        {
            sensorIdCheckState = WAIT_STATE;
        }
        else
        {
            if (analysisSensorImfo()) //是否收到获取位置信息指令
            {
                //解析回复数据
                sensorIdCheckState = WAIT_STATE;
            }
        }
        break;
    case WAIT_STATE: // 判断左侧探头位置
        sensorIdCheckState = 0;
        sensorIdCheckRunFlag = 1; //探头位置识别完成
        break;
    }
}

void AppTask(void)
{
    if (sensorIdCheckRunFlag == 0)
        return;
    switch (appTaskState)
    {
    case 0: //发送PWM 150ms 用来确认位置信号
        StartBeepAlarm(150,0);
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
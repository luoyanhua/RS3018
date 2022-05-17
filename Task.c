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


#define SensorCheck_0 0
#define SensorCheck_1 1
#define SensorCheck_2 2
#define SensorCheck_3 3
#define SensorCheck_4 4
#define SensorCheck_5 5
#define SensorCheck_6 6
#define SensorCheck_7 7
#define SensorCheck_8 8
#define SensorCheck_9 9

#define WAIT_REV_CMD 10
#define WAIT_STATE 12
#define GET_SENSOR_ID_CNT_MAX 3

bit sensorIdCheckRunFlag = 0;                          //运行标志
bit sensorIdCheckOKFlag = 0;                           //判断OK标志
bit ioStatusSaveValueOld = 0;                             // IO口状态保存
bit ioStatusSaveValueNew = 0;                             // IO口状态保存

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

//判断标准
//P35输出高电平，如果P34电压为低电平，就是左,右传感器；如果P34为高电平就是左中，右中
//P35输出低电平，如果P34电压为低电平，就是左，右中传感器，如果P34为高电平，就是右，左中传感器。
//两次电平相等恒低就是左中，恒高就是左；两次电平不等，但与P35电平相等就是右中，与P35电平不等就是右。


void sensorIdAdjustTask(void)
{
    if (sensorIdCheckRunFlag == 1)
        return;
    switch (sensorIdCheckState)
    {

    case SensorCheck_0:
        PWM_OUTPUT = HIGH;
        sensorIdCheckTimeCnt = Get_SysHalfMsTick(); //刷新计时器
        sensorIdCheckState = SensorCheck_1;
        break;
    case SensorCheck_1:
        if (get_time_escape_sec(Get_SysHalfMsTick(), sensorIdCheckTimeCnt) >= 20) // 等待20ms
        {
            ioStatusSaveValueOld = SENSOR_ID_CHECK; //记录SENSOR_ID_CHECK电平             
            sensorIdCheckState = SensorCheck_2;
        }
        break;
    case SensorCheck_2:
        if (get_time_escape_sec(Get_SysHalfMsTick(), sensorIdCheckTimeCnt) >= 50) // 等待30ms后再反转电平，避免电平冲突
        {
            PWM_OUTPUT = LOW;
            sensorIdCheckState = SensorCheck_3;
        }
        break;
    case SensorCheck_3:                                                      
        if (get_time_escape_sec(Get_SysHalfMsTick(), sensorIdCheckTimeCnt) >= 100) // 50ms超时后继续往下进行
        {
            ioStatusSaveValueNew = SENSOR_ID_CHECK; //记录SENSOR_ID_CHECK电平
            if (SENSOR_ID_CHECK == LOW) 
            {
                if (ioStatusSaveValueNew == ioStatusSaveValueOld) //两次SENSOR_ID_CHECK电平相等恒低
                {
                    currentSensorID = LEFT_MID_SENSOR;
                }
                else //两次电平不等，但与P35电平相等就是右中
                {
                    currentSensorID = RIGHT_MID_SENSOR;
                }
            }
            else
            {
                if (ioStatusSaveValueNew == ioStatusSaveValueOld) //两次SENSOR_ID_CHECK电平相等恒高
                {
                    currentSensorID = LEFT_SENSOR;
                }
                else //两次电平不等，与P35电平不等就是右
                {
                    currentSensorID = RIGHT_SENSOR;                   
                }
            }
            sensorIdCheckState = SensorCheck_4; 
            UART_config();        
        }
        break;
    case SensorCheck_4: 
        if(currentSensorID == RIGHT_SENSOR)//如果是右传感器需要跟其他传感器通信
        {
            getSensorImfo(RIGHT_MID_SENSOR, CMD_ID);
            sensorIdCheckState = SensorCheck_5;
            sensorIdCheckTimeCnt = Get_SysHalfMsTick(); //刷新计时器
        }
        else//不是右传感器，等待接收数据
        {
            sensorIdCheckState = WAIT_REV_CMD;
        }
        break;
    case SensorCheck_5:                                                      //判断是否收到右中探头ID信息
        if (get_time_escape_sec(Get_SysHalfMsTick(), sensorIdCheckTimeCnt) >= 50) // 50ms超时后继续往下进行
        {
            if (analysisSensorImfo())
            {
                //收到右中探头ID信息
                saveTotalSensorID[2] = 1;
                sensorIdGetCnt = 0;
                sensorIdCheckState = SensorCheck_6;
            }
            else
            {
                sensorIdGetCnt++;
                sensorIdCheckState = SensorCheck_4;
                if (sensorIdGetCnt >= GET_SENSOR_ID_CNT_MAX)
                {
                    sensorIdGetCnt = 0;
                    sensorIdCheckState = SensorCheck_6;
                }
            }
        }
        break;
    case SensorCheck_6:                //发送获取左侧探头信息指令
        getSensorImfo(LEFT_SENSOR, CMD_ID); //发送获取左侧探头信息指令
        sensorIdCheckState = SensorCheck_7;
        sensorIdCheckTimeCnt = Get_SysHalfMsTick(); //刷新计时器
        break;
    case SensorCheck_7:                                                      //判断是否收到左侧探头ID信息
        if (get_time_escape_sec(Get_SysHalfMsTick(), sensorIdCheckTimeCnt) >= 50) // 50ms超时后继续往下进行
        {
            if (analysisSensorImfo())
            {
                //收到左侧探头ID信息
                saveTotalSensorID[0] = 1;
                sensorIdGetCnt = 0;
                sensorIdCheckState = SensorCheck_8;
            }
            else
            {
                sensorIdGetCnt++;
                sensorIdCheckState = SensorCheck_6;
                if (sensorIdGetCnt >= GET_SENSOR_ID_CNT_MAX)
                {
                    sensorIdGetCnt = 0;
                    sensorIdCheckState = SensorCheck_8;
                }
            }
        }
        break;
    case SensorCheck_8:                    //发送获取左中探头信息指令
        getSensorImfo(LEFT_MID_SENSOR, CMD_ID); //发送获取左中探头信息指令
        sensorIdCheckState = SensorCheck_9;
        sensorIdCheckTimeCnt = Get_SysHalfMsTick(); //刷新计时器
        break;
    case SensorCheck_9:                                                      //判断是否收到左中探头ID信息
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
                sensorIdCheckState = SensorCheck_8;
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
        StartBeepAlarm(150, 0);
        appTaskTimeCnt = Get_SysHalfMsTick();
        appTaskState = 1;
        break;
    case 1: // Sensor ID会收到自己发的信号
        if (SENSOR_ID_CHECK == 0)
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
//            uartSendPackage(SELF_CHECK);
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
//                    uartSendPackage(NOM_WORK);
                }
            }
            else //没有障碍物
            {
                if (get_time_escape_sec(Get_SysHalfMsTick(), appTaskTimeCnt) >= 400) // 200ms测试一次
                {
                    //获取测试距离
                    VirtualCOM_StringSend("No obstacles!\r\n"); // UART1发送一个字符串
                    appTaskState = 4;
//                    uartSendPackage(NOM_WORK);
                }
            }
        }
        break;
    default:
        break;
    }
}
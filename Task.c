/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCMCU.com --------------------------------------------*/
/* --- Web: www.STCMCUDATA.com  ---------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* ���Ҫ�ڳ�����ʹ�ô˴���,���ڳ�����ע��ʹ����STC�����ϼ�����            */
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

bit sensorIdCheckRunFlag = 0;                          //���б�־
bit sensorIdCheckOKFlag = 0;                           //�ж�OK��־
bit ioStatusSaveValue = 0;                             // IO��״̬����
unsigned char saveTotalSensorID[SENSOR_NUM_MAX] = {0}; //��ǰ�����豸IDλ��,���Ϊ�Ҳഫ���������Զ���Ϊ����������˳��ֱ���������У����У���
unsigned char sensorIdCheckState = 0;                  //����״̬��
unsigned int sensorIdCheckTimeCnt = 0;                 //��ʱ��
unsigned char sensorIdGetCnt = 0;                      // ID ��ȡ����
unsigned char currentSensorID = 0;                     //��ǰ������ֵ

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
        sensorIdCheckTimeCnt = Get_SysHalfMsTick(); //ˢ�¼�ʱ��
        sensorIdCheckState = RightSensorCheck_1;
        break;
    case RightSensorCheck_1:
        if (get_time_escape_sec(Get_SysHalfMsTick(), sensorIdCheckTimeCnt) >= 20) // �ȴ�20ms
        {
            if (SENSOR_ID_CHECK == LOW) //��ʱ�����У��Ҷ�����Ҫ��,ֻ�����в�����Ҫ��
            {
                ioStatusSaveValue = SENSOR_ID_CHECK; //��¼SENSOR_ID_CHECK��ƽ
                sensorIdCheckState = RightSensorCheck_2;
            }
            else
            {
                currentSensorID = LEFT_MID_SENSOR;
                sensorIdCheckState = WAIT_REV_CMD; //�����ж������Ƿ����
            }
        }
        break;
    case RightSensorCheck_2:
        if (get_time_escape_sec(Get_SysHalfMsTick(), sensorIdCheckTimeCnt) >= 50) // �ȴ�30ms���ٷ�ת��ƽ�������ƽ��ͻ
        {
            PWM_OUTPUT = HIGH;
            sensorIdCheckState = RightSensorCheck_3;
        }
        break;
    case RightSensorCheck_3:                                                       // �ж��Ҳ�̽ͷλ��                                                      // �ж�����̽ͷλ��
        if (get_time_escape_sec(Get_SysHalfMsTick(), sensorIdCheckTimeCnt) >= 100) // 50ms��ʱ��������½���
        {
            if (SENSOR_ID_CHECK == HIGH) //��ʱ���У�������Ҫ��ֻ��������Ҫ��
            {
                if (SENSOR_ID_CHECK == ioStatusSaveValue) //����SENSOR_ID_CHECK��ƽ��Ⱦ�����
                {
                    currentSensorID = RIGHT_SENSOR;
                    sensorIdCheckState = RightSensorCheck_4; //��һ�����ڣ������ж�
                }
                else //����Ⱦ�������
                {
                    currentSensorID = RIGHT_MID_SENSOR;
                    sensorIdCheckState = WAIT_REV_CMD; //�����ж������Ƿ����
                }
            }
            else
            {
                currentSensorID = LEFT_SENSOR;
                sensorIdCheckState = WAIT_REV_CMD; //�����ж����Ƿ����
            }
        }
        break;
    case RightSensorCheck_4: //�����״̬������ȻΪ�Ҳ�̽ͷ�����ͻ�ȡ����̽ͷ��Ϣָ��
        getSensorImfo(RIGHT_MID_SENSOR, CMD_ID);
        sensorIdCheckState = RightSensorCheck_5;
        sensorIdCheckTimeCnt = Get_SysHalfMsTick(); //ˢ�¼�ʱ��
        break;
    case RightSensorCheck_5:                                                      //�ж��Ƿ��յ�����̽ͷID��Ϣ
        if (get_time_escape_sec(Get_SysHalfMsTick(), sensorIdCheckTimeCnt) >= 50) // 50ms��ʱ��������½���
        {
            if (analysisSensorImfo())
            {
                //�յ�����̽ͷID��Ϣ
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
    case RightSensorCheck_6:                //���ͻ�ȡ���̽ͷ��Ϣָ��
        getSensorImfo(LEFT_SENSOR, CMD_ID); //���ͻ�ȡ���̽ͷ��Ϣָ��
        sensorIdCheckState = RightSensorCheck_7;
        sensorIdCheckTimeCnt = Get_SysHalfMsTick(); //ˢ�¼�ʱ��
        break;
    case RightSensorCheck_7:                                                      //�ж��Ƿ��յ����̽ͷID��Ϣ
        if (get_time_escape_sec(Get_SysHalfMsTick(), sensorIdCheckTimeCnt) >= 50) // 50ms��ʱ��������½���
        {
            if (analysisSensorImfo())
            {
                //�յ����̽ͷID��Ϣ
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
    case RightSensorCheck_8:                    //���ͻ�ȡ����̽ͷ��Ϣָ��
        getSensorImfo(LEFT_MID_SENSOR, CMD_ID); //���ͻ�ȡ����̽ͷ��Ϣָ��
        sensorIdCheckState = RightSensorCheck_9;
        sensorIdCheckTimeCnt = Get_SysHalfMsTick(); //ˢ�¼�ʱ��
        break;
    case RightSensorCheck_9:                                                      //�ж��Ƿ��յ�����̽ͷID��Ϣ
        if (get_time_escape_sec(Get_SysHalfMsTick(), sensorIdCheckTimeCnt) >= 50) // 50ms��ʱ��������½���
        {
            if (analysisSensorImfo())
            {
                //�յ�����̽ͷID��Ϣ
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
    case WAIT_REV_CMD:                                                             //���������Ȼ�����Ҵ�����
        if (get_time_escape_sec(Get_SysHalfMsTick(), sensorIdCheckTimeCnt) >= 600) // 600ms��ʱ��������½���
        {
            sensorIdCheckState = WAIT_STATE;
        }
        else
        {
            if (analysisSensorImfo()) //�Ƿ��յ���ȡλ����Ϣָ��
            {
                //�����ظ�����
                sensorIdCheckState = WAIT_STATE;
            }
        }
        break;
    case WAIT_STATE: // �ж����̽ͷλ��
        sensorIdCheckState = 0;
        sensorIdCheckRunFlag = 1; //̽ͷλ��ʶ�����
        break;
    }
}

void AppTask(void)
{
    if (sensorIdCheckRunFlag == 0)
        return;
    switch (appTaskState)
    {
    case 0: //����PWM 150ms ����ȷ��λ���ź�
        StartBeepAlarm(150,0);
        appTaskTimeCnt = Get_SysHalfMsTick();
        appTaskState = 1;
        break;
    case 1: // Sensor ID���յ��Լ������ź�
        if (P34 == 0)
        {
            VirtualCOM_StringSend("recv ID!\r\n"); // UART1����һ���ַ���
            appTaskState = 2;
        }
        else if (get_time_escape_sec(Get_SysHalfMsTick(), appTaskTimeCnt) >= 20) // 10ms��ʱ��������½���
        {
            VirtualCOM_StringSend("recv ID timeout!\r\n"); // UART1����һ���ַ���
            appTaskState = 2;
        }
        break;
    case 2: //����̽ͷ�Լ�
        Start_sendOncePlusTask();
        appTaskState = 3;
        VirtualCOM_StringSend("sensor self check!\r\n"); // UART1����һ���ַ���
        break;
    case 3: //��ȡ̽ͷ�Լ���
        if (Get_SendOncePlusTaskFlag() == 0)
        {
            if (Get_sensorOkFlag())
            {
                VirtualCOM_StringSend("sensor is ok!\r\n"); // UART1����һ���ַ���
                appTaskState = 4;                           //��⵽̽ͷ
            }
            else
            {
                VirtualCOM_StringSend("No sensor!\r\n"); // UART1����һ���ַ���
                appTaskState = 0;                        //��̽ͷ��ͷ��ʼ
            }
            uartSendPackage(SELF_CHECK);
        }
        break;
    case 4: //��ʼ�����
        appTaskTimeCnt = Get_SysHalfMsTick();
        Start_sendOncePlusTask();
        appTaskState = 5;
        break;
    case 5: //�ж��Ƿ����ϰ���
        if (Get_SendOncePlusTaskFlag() == 0)
        {
            if (Get_obstaclesExistenceFlag()) //���ϰ���
            {
                if (get_time_escape_sec(Get_SysHalfMsTick(), appTaskTimeCnt) >= 100) // 50ms����һ��
                {
                    VirtualCOM_StringSend("obstacle!\r\n"); // UART1����һ���ַ���
                    appTaskState = 4;
                    uartSendPackage(NOM_WORK);
                }
            }
            else //û���ϰ���
            {
                if (get_time_escape_sec(Get_SysHalfMsTick(), appTaskTimeCnt) >= 400) // 200ms����һ��
                {
                    //��ȡ���Ծ���
                    VirtualCOM_StringSend("No obstacles!\r\n"); // UART1����һ���ַ���
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
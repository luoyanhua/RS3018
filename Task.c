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
#include "UART.h"
#include "stdio.h"
#include "string.h"

#define SENSOR_NUM_MAX 4

typedef struct
{
    bit runFlag;                           //���б�־
    bit adjustOKFlag;                      //�ж�OK��־
    bit ioStatusSaveValue;                 // IO��״̬����
    bit saveTotalSensorID[SENSOR_NUM_MAX]; //��ǰ�����豸IDλ��,���Ϊ�Ҳഫ���������Զ���Ϊ��������
    unsigned char state;                   //����״̬��
    unsigned int TimeCnt;                  //��ʱ��
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
    case 0: //����PWM 150ms ����ȷ��λ���ź�
        StartBeepAlarm(150);
        sensorIdAdjustStruct.TimeCnt = Get_SysHalfMsTick(); //������ʱ��
        sensorIdAdjustStruct.state = 1;
        break;
    case 1: // �ж�����̽ͷλ�ã�ֻ���Ҳ�̽ͷ���յ��Լ������ź�
        //����P10����PWM
        if (get_time_escape_sec(Get_SysHalfMsTick(), sensorIdAdjustStruct.TimeCnt) >= 200) // 200ms��ʱ��������½���
        {
            if (sensorIdAdjustStruct.adjustOKFlag == 1) //�յ�PWM
            {
                sensorIdAdjustStruct.adjustOKFlag = 0;
                uartSendPackage(PWM_CHECK); //�����Լ�Ϊ����̽ͷ����������̽ͷ��IDʶ����Ϣ
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
    case 2:                                                                                // �ж��Ҳ�̽ͷλ��
        if (get_time_escape_sec(Get_SysHalfMsTick(), sensorIdAdjustStruct.TimeCnt) >= 400) // 200ms��ʱ��������½���
        {
            if (getRxSensorId() == 4) //�յ�����̽ͷ��IDʶ����Ϣ
            {
                sensorIdAdjustStruct.ioStatusSaveValue = 1;
                uartSendPackage(SELF_CHECK);    //�����Լ�Ϊ�Ҳ�̽ͷ�������Ҳ�̽ͷ���
                sensorIdAdjustStruct.state = 5; //��ת���ȴ�1S��ʱ���
            }
            else
            {
                sensorIdAdjustStruct.state = 3;
            }
        }
        break;
    case 3:                                                                                // �ж�����̽ͷλ��
        if (get_time_escape_sec(Get_SysHalfMsTick(), sensorIdAdjustStruct.TimeCnt) >= 600) // 200ms��ʱ��������½���
        {
            if (sensorIdAdjustStruct.adjustOKFlag == 1) //�ж������͵�ƽ������
            {

                uartSendPackage(SELF_CHECK);    //�����Լ�Ϊ����̽ͷ����������̽ͷ���
                sensorIdAdjustStruct.state = 5; //��ת���ȴ�1S��ʱ���
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
    case 4:                                                                                // �ж����̽ͷλ��
        if (get_time_escape_sec(Get_SysHalfMsTick(), sensorIdAdjustStruct.TimeCnt) >= 800) // 200ms��ʱ��������½���
        {
            if (sensorIdAdjustStruct.adjustOKFlag == 1) //�ж������ߵ�ƽ�����
            {
                uartSendPackage(SELF_CHECK);    //�����Լ�Ϊ���̽ͷ���������̽ͷ���
                sensorIdAdjustStruct.state = 5; //��ת���ȴ�1S��ʱ���
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
    case 5: // �ж����̽ͷλ��
        sensorIdAdjustStruct.state = 0;
        sensorIdAdjustStruct.runFlag = 1; //̽ͷλ��ʶ�����
        break;
    }
}

void AppTask(void)
{
    if (sensorIdAdjustStruct.runFlag == 0)
        return;
    switch (appTaskState)
    {
    case 0: //����PWM 150ms ����ȷ��λ���ź�
        StartBeepAlarm(150);
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
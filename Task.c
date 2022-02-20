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

unsigned char appTaskState = 0;
unsigned int appTaskTimeCnt = 0;

void AppTask(void)
{
    switch (appTaskState)
    {
    case 0: //����PWM 0.5S ����ȷ��λ���ź�
        StartBeepAlarm(500);
        appTaskTimeCnt = Get_SysHalfMsTick();
        appTaskState = 1;
        break;
    case 1: //Sensor ID���յ��Լ������ź�
        if (P10 == 0)
        {
			PrintString1("recv ID!\r\n"); //UART1����һ���ַ���
            appTaskState = 2;
        }
        else if (get_time_escape_sec(Get_SysHalfMsTick(), appTaskTimeCnt) >= 20) //10ms��ʱ��������½���
        {
            appTaskState = 2;
        }
        break;
    case 2: //����̽ͷ�Լ�
        Start_sendOncePlusTask();
        appTaskState = 3;
        break;
    case 3: //��ȡ̽ͷ�Լ���
        if (Get_SendOncePlusTaskFlag() == 0)
        {
            if (Get_sensorOkFlag())
            {
				PrintString1("sensor is ok!\r\n"); //UART1����һ���ַ���
                appTaskState = 4; //��⵽̽ͷ
            }
            else
            {
				PrintString1("No sensor!\r\n"); //UART1����һ���ַ���
                appTaskState = 0; //��̽ͷ��ͷ��ʼ
            }
        }
        break;
    case 4: //��ʼ�����
        Start_sendOncePlusTask();
        appTaskTimeCnt = Get_SysHalfMsTick();
        appTaskState = 5;
        break;
    case 5: //�ж��Ƿ����ϰ���
        if (Get_SendOncePlusTaskFlag() == 0)
        {
            if (Get_obstaclesExistenceFlag()) //���ϰ���
            {
                if (get_time_escape_sec(Get_SysHalfMsTick(), appTaskTimeCnt) >= 100) //50ms����һ��
                {
					PrintString1("there is obstacle!\r\n"); //UART1����һ���ַ���
					printf("d=%d mm",Get_meterDistance());
                    appTaskState = 4;
                }
            }
            else //û���ϰ���
            {
                if (get_time_escape_sec(Get_SysHalfMsTick(), appTaskTimeCnt) >= 400) //200ms����һ��
                {
                    //��ȡ���Ծ���
					PrintString1("No obstacles!\r\n"); //UART1����һ���ַ���
                    appTaskState = 4;
                }
            }
        }
        break;
    default:
        break;
    }
}
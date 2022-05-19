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

#include "Sensor.h"
#include "timer.h"
#include "Beep.h"
#include "Exti.h"
#include "uart.h"

#define START_SEND_STATE 0
#define WAIT_SEND_END_STATE 1
#define START_SELF_CHECK_STATE 2
#define START_METER_STATE 3
#define SENSOR_ERR_STATE 4

bit SendOncePlusTaskFlag = 0, sensorOkFlag = 0, obstaclesExistenceFlag = 0;
bit sendMode = 0;
unsigned char SendContinuePlusState = 0;
unsigned int SendContinuePlusTimeCnt = 0;
unsigned int meterDistance = 0; //��λ��mm

//��ȡ���Ծ���ֵ
unsigned int Get_meterDistance(void)
{
	return meterDistance;
}

//��ȡ������״̬
bit Get_sensorOkFlag(void)
{
	return sensorOkFlag;
}

//��ȡ����״̬
bit Get_SendOncePlusTaskFlag(void)
{
	return SendOncePlusTaskFlag;
}

//��ȡ�����ϰ���״̬
bit Get_obstaclesExistenceFlag(void)
{
	return obstaclesExistenceFlag;
}

//׼����ʼ���ԣ���ʼ����ز���
void Start_sendOncePlusTask(void)
{
	SendOncePlusTaskFlag = 1;
	SendContinuePlusState = START_SEND_STATE;
}

// sendMode 0:�Լ�ģʽ 1:��������ģʽ
//���η�������Ⱥ����
void SendOncePlusTask(void)
{
	if (SendOncePlusTaskFlag == 0)
		return;
	switch (SendContinuePlusState)
	{
	case START_SEND_STATE:
		if(get_uartUsedFlag() == 0)		//���⴮�ں͸��ٶ�ʱ��ͬʱ������Ӱ�촮������
		{
			SendContinuePlusTimeCnt = Get_us_250Cnt(); //��λ50us
			Start_SendPlus();							   //���ͳ�����
			meterDistance = 0;							   //ÿ�η���ǰ��������
			SendContinuePlusState = WAIT_SEND_END_STATE;
		}
		break;
	case WAIT_SEND_END_STATE:
		if (Get_plusOutFlag() == 0) //�ȴ��������
		{
			if (sendMode == 0)
			{
				Start_recvPlus(); //�������շ����ź�
				SendContinuePlusState = START_SELF_CHECK_STATE;
			}
			else
			{
				if (get_time_escape_sec(Get_us_250Cnt(), SendContinuePlusTimeCnt) >= 6) // 1.5ms��ʼ�������
				{
					Start_recvPlus(); //�������շ����ź�
					SendContinuePlusState = START_METER_STATE;
				}
			}
		}
		break;
	case START_SELF_CHECK_STATE:
		if (Get_recvPlusFlag() == 0) //���������ɣ��϶��յ�����
		{
			sendMode = 1;
			sensorOkFlag = 1;
			StartBeepAlarm(500,0);	  //�����յ������ź�	0.5S
			SendOncePlusTaskFlag = 0; //�Լ���ɺ�ֹͣ��������
		}
		else if (get_time_escape_sec(Get_us_250Cnt(), SendContinuePlusTimeCnt) >= 160) //���δ�յ��������Σ����ж��Ƿ�ʱ
		{
			sensorOkFlag = 0;
			StartBeepAlarm(2000,0);	  //û���յ������źţ�����	2S
			SendOncePlusTaskFlag = 0; //�Լ���ɺ�ֹͣ��������
			Stop_recvPlus();
		}
		break;
	case START_METER_STATE:
		if (Get_recvPlusFlag() == 0) //�յ����Σ���ʼ�������
		{
			meterDistance = get_time_escape_sec(Get_us_250Cnt(), SendContinuePlusTimeCnt) * 2.5 * 17; // 340/2=170·�����أ�1����ֵ����250us
			if (meterDistance < 250)																	//��̾���0.25��
			{
				meterDistance = 0;
			}
			else if (meterDistance > 2550)
			{
				meterDistance = 2550;
			}
			obstaclesExistenceFlag = 1;
			SendOncePlusTaskFlag = 0;
		}
		//��Զ3�ף������17.6ms���ݶ�40ms
		else if (get_time_escape_sec(Get_us_250Cnt(), SendContinuePlusTimeCnt) >= 160) //���δ�յ��������Σ����ж��Ƿ�ʱ
		{
			//�������ϰ�������
			obstaclesExistenceFlag = 0;
			SendOncePlusTaskFlag = 0; //һ�β��Խ�����δ�յ����ز���
			meterDistance = 160;	  //û̽�⵽���ݣ������ϱ�0x10
			Stop_recvPlus();
		}
		break;
	default:
		break;
	}
}
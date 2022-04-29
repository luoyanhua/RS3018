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

#include "Beep.h"
#include "timer.h"

bit beepStartFlag = 0;	  //���Ʒ������Ƿ�����
bit beepAlarmRunFlag = 0; //���ƶ�ʱ��1K�ź��Ƿ���
typedef struct
{
	unsigned char State;
	unsigned char ModeSet;
	unsigned short TimeSet;
	unsigned int TimeCnt;
} BeepAlarmStruct_T;

BeepAlarmStruct_T BeepAlarmStruct;

//��ȡ������������־
bit Get_beepAlarmRunFlag(void)
{
	return beepAlarmRunFlag;
}

//��ʼ�����������趨ֵ
// time:����ʱ��
// speed�����������ٶ� 0:���٣�1������
// mode:����ģʽ 0:1�� ��1������
void StartBeepAlarm(unsigned short time, unsigned char mode) // time��λ1ms
{
	BeepAlarmStruct.State = 0;
	BeepAlarmStruct.TimeSet = time * 2;
	BeepAlarmStruct.ModeSet = mode;
	beepAlarmRunFlag = 1;
	beepStartFlag = 1;
}

//�������
void ClrBeepAlarm(void)
{
	beepStartFlag = 0;
	beepAlarmRunFlag = 0;
}

//���з�������������
void BeepAlarmTask(void)
{
	if (beepStartFlag == 0)
		return;
	switch (BeepAlarmStruct.State)
	{
	case 0:
		beepAlarmRunFlag = 1;
		BeepAlarmStruct.TimeCnt = Get_SysHalfMsTick();
		BeepAlarmStruct.State = 1;
		break;
	case 1:
		if (get_time_escape_sec(Get_SysHalfMsTick(), BeepAlarmStruct.TimeCnt) >= BeepAlarmStruct.TimeSet)
		{
			beepAlarmRunFlag = 0;
			if (BeepAlarmStruct.ModeSet)
			{
				ClrBeepAlarm();
			}
			else
			{
				BeepAlarmStruct.State = 2;
			}
		}
		break;
	case 2:
		if (get_time_escape_sec(Get_SysHalfMsTick(), BeepAlarmStruct.TimeCnt) >= BeepAlarmStruct.TimeSet * 2)
		{
			BeepAlarmStruct.State = 0;
		}
		break;
	default:
		break;
	}
}
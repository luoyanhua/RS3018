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

bit beepAlarmFlag = 0;
u8 BeepAlarmState = 0;
unsigned short BeepAlarmTimeSet = 0;
unsigned int BeepAlarmTimeCnt = 0;

//��ȡ������������־
bit Get_beepAlarmFlag(void)
{
	return beepAlarmFlag;
}

//��ʼ�����������趨ֵ
void StartBeepAlarm(unsigned short time) //time��λ1ms
{
	BeepAlarmState = 0;
	BeepAlarmTimeSet = time * 2;
	beepAlarmFlag = 1;
}

//���з�������������
void BeepAlarmTask(void)
{
	if (beepAlarmFlag == 0)
		return;
	switch (BeepAlarmState)
	{
	case 0:
		BeepAlarmTimeCnt = Get_SysHalfMsTick();
		BeepAlarmState = 1;
		break;
	case 1:
		if (get_time_escape_sec(Get_SysHalfMsTick(), BeepAlarmTimeCnt) >= BeepAlarmTimeSet)
		{
			beepAlarmFlag = 0;
		}
		break;
	default:
		break;
	}
}
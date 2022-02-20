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

/*************	����˵��	**************

���ļ�ΪSTC8ϵ�еĶ�ʱ����ʼ�����жϳ���,�û�����������ļ����޸��Լ���Ҫ���жϳ���.

******************************************/

#include "timer.h"
#include "Beep.h"

#define PLUS_MAX 12 //�������������

bit plusOutFlag = 0;		  //������α�־
unsigned char plusOutCnt = 0; //80K�жϼ���

unsigned int recvPlusTimerCnt = 0;

/********************* Timer0�жϺ���************************/
void timer0_int(void) interrupt TIMER0_VECTOR
{

	recvPlusTimerCnt++;

	//ÿ�ε������MAX������
	if (plusOutFlag == 0)
		return;
	plusOutCnt++;
	if (plusOutCnt >= (PLUS_MAX * 2))
	{
		plusOutCnt = 0;
		plusOutFlag = 0;
	}
	else
	{
		P11 = ~P11;
	}
}

void Start_SendPlus(void)
{
	plusOutFlag = 1;
}

bit Get_plusOutFlag(void)
{
	return plusOutFlag;
}

unsigned int Get_RecvPlusTimerCnt(void) //��λ12.5us
{
	return recvPlusTimerCnt;
}

unsigned int halfMsCnt = 0;
/********************* Timer1�жϺ���************************/
void timer1_int(void) interrupt TIMER1_VECTOR
{
	halfMsCnt++;
	//ÿ�ε����������1KHz����
	if (Get_beepAlarmFlag() == 0)
		return;
	P35 = ~P35;
}

unsigned int Get_SysHalfMsTick(void) //��λ��0.5ms
{
	return halfMsCnt;
}

unsigned int get_time_escape_sec(unsigned int ticks_now, unsigned int ticks_old)
{
	return ((ticks_now - ticks_old) & 0xffffffff);
}

//========================================================================
// ����: unsigned char	Timer_Inilize(unsigned char TIM, TIM_InitTypeDef *TIMx)
// ����: ��ʱ����ʼ������.
// ����: TIMx: �ṹ����,��ο�timer.h��Ķ���.
// ����: �ɹ�����0, �ղ�������1,���󷵻�2.
// �汾: V1.0, 2012-10-22
//========================================================================
unsigned char Timer_Inilize(unsigned char TIM, TIM_InitTypeDef *TIMx)
{
	if (TIM == Timer0)
	{
		Timer0_Stop(); //ֹͣ����
		if (TIMx->TIM_Interrupt == ENABLE)
			Timer0_InterruptEnable(); //�����ж�
		else
			Timer0_InterruptDisable(); //��ֹ�ж�
		if (TIMx->TIM_Priority > Priority_3)
			return 2;						 //����
		Timer0_Priority(TIMx->TIM_Priority); //ָ���ж����ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3

		if (TIMx->TIM_Mode >= TIM_16BitAutoReloadNoMask)
			return 2;							//����
		TMOD = (TMOD & ~0x30) | TIMx->TIM_Mode; //����ģʽ,0: 16λ�Զ���װ, 1: 16λ��ʱ/����, 2: 8λ�Զ���װ
		if (TIMx->TIM_ClkSource == TIM_CLOCK_12T)
			Timer0_12T(); //12T
		if (TIMx->TIM_ClkSource == TIM_CLOCK_1T)
			Timer0_1T(); //1T
		if (TIMx->TIM_ClkSource == TIM_CLOCK_Ext)
			Timer0_AsCounter(); //����������Ƶ
		else
			Timer0_AsTimer(); //��ʱ
		if (TIMx->TIM_ClkOut == ENABLE)
			Timer0_CLKO_Enable(); //���ʱ��
		else
			Timer0_CLKO_Disable(); //�����ʱ��

		T0_Load(TIMx->TIM_Value);
		if (TIMx->TIM_Run == ENABLE)
			Timer0_Run(); //��ʼ����
		return 0;		  //�ɹ�
	}

	if (TIM == Timer1)
	{
		Timer1_Stop(); //ֹͣ����
		if (TIMx->TIM_Interrupt == ENABLE)
			Timer1_InterruptEnable(); //�����ж�
		else
			Timer1_InterruptDisable(); //��ֹ�ж�
		if (TIMx->TIM_Priority > Priority_3)
			return 2;						 //����
		Timer1_Priority(TIMx->TIM_Priority); //ָ���ж����ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
		if (TIMx->TIM_Mode >= TIM_16BitAutoReloadNoMask)
			return 2;							//����
		TMOD = (TMOD & ~0x30) | TIMx->TIM_Mode; //����ģʽ,0: 16λ�Զ���װ, 1: 16λ��ʱ/����, 2: 8λ�Զ���װ
		if (TIMx->TIM_ClkSource == TIM_CLOCK_12T)
			Timer1_12T(); //12T
		if (TIMx->TIM_ClkSource == TIM_CLOCK_1T)
			Timer1_1T(); //1T
		if (TIMx->TIM_ClkSource == TIM_CLOCK_Ext)
			Timer1_AsCounter(); //����������Ƶ
		else
			Timer1_AsTimer(); //��ʱ
		if (TIMx->TIM_ClkOut == ENABLE)
			Timer1_CLKO_Enable(); //���ʱ��
		else
			Timer1_CLKO_Disable(); //�����ʱ��

		T1_Load(TIMx->TIM_Value);
		if (TIMx->TIM_Run == ENABLE)
			Timer1_Run(); //��ʼ����
		return 0;		  //�ɹ�
	}
	return 2; //����
}
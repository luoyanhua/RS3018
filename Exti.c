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

���ļ�ΪSTC8ϵ�е����жϳ�ʼ�����жϳ���,�û�����������ļ����޸��Լ���Ҫ���жϳ���.

******************************************/

#include "Exti.h"
#include "timer.h"

/*************  �ⲿ�����ͱ������� *****************/
bit recvPlusFlag = 0;

u8 recvPlusCnt = 0;			//�������
u32 recvPlusTimeCnt = 0;	//��������ʱ��

//�������ճ�����
void Start_recvPlus(void)
{
	recvPlusFlag = 1;
	recvPlusCnt = 0;
	recvPlusTimeCnt = 0;
}

//�����ж��Ƿ�������
bit Get_recvPlusFlag(void)
{
	return recvPlusFlag;
}

/********************* INT1�жϺ��� *************************/
void Ext_INT1(void) interrupt INT1_VECTOR //���ж�ʱ�Ѿ������־
{
	if (recvPlusFlag == 0)	//���տ�ʼ��־ 0��������ɻ�δ��ʼ��1������δ���
		return;

	if (get_time_escape_sec(Get_RecvPlusTimerCnt(),recvPlusTimeCnt) <= 4)			//���������ſ�ʼ����
	{
		recvPlusCnt++;
		if(recvPlusCnt >= 3)
		{			
			recvPlusFlag = 0;
		}
	}
	else
	{
		recvPlusCnt = 0;		
	}
	
	recvPlusTimeCnt = Get_RecvPlusTimerCnt();
}

//========================================================================
// ����: u8	Ext_Inilize(u8 EXT, EXTI_InitTypeDef *INTx)
// ����: �ⲿ�жϳ�ʼ������.
// ����: INTx: �ṹ����,��ο�Exti.h��Ķ���.
// ����: �ɹ�����0, �ղ�������1,���󷵻�2.
// �汾: V1.0, 2012-10-22
//========================================================================
u8 Ext_Inilize(u8 EXT, EXTI_InitTypeDef *INTx)
{
	if (EXT == EXT_INT1) //���ж�1
	{
		IE1 = 0; //���ж�1��־λ
		if (INTx->EXTI_Interrupt == ENABLE)
			EX1 = 1; //�����ж�
		else
			EX1 = 0; //��ֹ�ж�
		if (INTx->EXTI_Priority > Priority_3)
			return 2;						//����
		INT1_Priority(INTx->EXTI_Priority); //ָ���ж����ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
		if (INTx->EXTI_Mode == EXT_MODE_Fall)
			IT1 = 1; //�½����ж�
		else
			IT1 = 0; //����,�½����ж�
		return 0;	 //�ɹ�
	}
	return 2; //ʧ��
}

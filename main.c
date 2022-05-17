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

#include "config.h"
#include "GPIO.h"
#include "UART.h"
#include "delay.h"
#include "WDT.h"
#include "timer.h"
#include "Sensor.h"
#include "Beep.h"
#include "Task.h"
#include "Exti.h"

/*************   ����˵��   ***************

�����̻���STC8G1K08-QFN20���б�д����

PCA0  ʹ�ö�ʱ��0���40KHz���
��������ʱ��=��׽��ʱ����/PCAʱ��Դ, ���� ��׽��ʱ���� = 256, PCAʱ��Դ = 20MHz(1T), ���� = 256/20MHz = 12.8us.
����ʱ, ѡ��ʱ�� 20MHz (�û�����"config.h"�޸�Ƶ��).

******************************************/

/*************	���س�������	**************/

/*************	���ر�������	**************/

/*************	���غ�������	**************/

/*************  �ⲿ�����ͱ������� *****************/

/******************** IO������ ********************/
void GPIO_config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; //�ṹ����

	GPIO_InitStructure.Pin = GPIO_Pin_All;		//ָ��Ҫ��ʼ����IO, GPIO_Pin_0 ~ GPIO_Pin_7, �����
	GPIO_InitStructure.Mode = GPIO_PullUp;		//ָ��IO������������ʽ,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P1, &GPIO_InitStructure); //��ʼ��

	GPIO_InitStructure.Pin = GPIO_Pin_0;		//ָ��Ҫ��ʼ����IO, GPIO_Pin_0 ~ GPIO_Pin_7, �����
	GPIO_InitStructure.Mode = GPIO_HighZ;		//ָ��IO������������ʽ,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P1, &GPIO_InitStructure); //��ʼ��	

	GPIO_InitStructure.Pin = GPIO_Pin_All;		//ָ��Ҫ��ʼ����IO, GPIO_Pin_0 ~ GPIO_Pin_7, �����
	GPIO_InitStructure.Mode = GPIO_PullUp;		//ָ��IO������������ʽ,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P3, &GPIO_InitStructure); //��ʼ��

	GPIO_InitStructure.Pin = GPIO_Pin_4;		//ָ��Ҫ��ʼ����IO, GPIO_Pin_0 ~ GPIO_Pin_7, �����
	GPIO_InitStructure.Mode = GPIO_HighZ;		//ָ��IO������������ʽ,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P3, &GPIO_InitStructure); //��ʼ��
	
	GPIO_InitStructure.Pin = GPIO_Pin_5|GPIO_Pin_2;		//ָ��Ҫ��ʼ����IO, GPIO_Pin_0 ~ GPIO_Pin_7, �����
	GPIO_InitStructure.Mode = GPIO_OUT_PP;		//ָ��IO������������ʽ,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P3, &GPIO_InitStructure); //��ʼ��

	UART_CHG_IO = UART_RX_EN;	
}

/************************ ��ʱ������ ****************************/
void Timer_config(void)
{
	TIM_InitTypeDef TIM_InitStructure;							   //�ṹ����
	TIM_InitStructure.TIM_Mode = TIM_16BitAutoReload;			   //ָ������ģʽ,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_Priority = Priority_1;				   //ָ���ж����ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
	TIM_InitStructure.TIM_Interrupt = DISABLE;					   //�ж��Ƿ�����,   ENABLE��DISABLE
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_1T;				   //ָ��ʱ��Դ,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut = DISABLE;						   //�Ƿ������������, ENABLE��DISABLE
	TIM_InitStructure.TIM_Value = 65536UL - (MAIN_Fosc / 80000UL); //��ֵ,    80KHz   12.5us�ж�һ��
	TIM_InitStructure.TIM_Run = ENABLE;							   //�Ƿ��ʼ����������ʱ��, ENABLE��DISABLE
	Timer_Inilize(Timer0, &TIM_InitStructure);					   //��ʼ��Timer0	  Timer0,Timer1,Timer2,Timer3,Timer4

	TIM_InitStructure.TIM_Mode = TIM_16BitAutoReload;			//ָ������ģʽ,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_Priority = Priority_0;				//ָ���ж����ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
	TIM_InitStructure.TIM_Interrupt = ENABLE;					//�ж��Ƿ�����,   ENABLE��DISABLE
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_1T;				//ָ��ʱ��Դ, TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut = DISABLE;						//�Ƿ������������, ENABLE��DISABLE
	TIM_InitStructure.TIM_Value = 65536UL - (MAIN_Fosc / 4000); //��ֵ,    250us�ж�һ��
	TIM_InitStructure.TIM_Run = ENABLE;							//�Ƿ��ʼ����������ʱ��, ENABLE��DISABLE
	Timer_Inilize(Timer1, &TIM_InitStructure);					//��ʼ��Timer1	  Timer0,Timer1,Timer2,Timer3,Timer4
}

/******************** WDT���� ********************/
void WDT_config(void)
{
	WDT_InitTypeDef WDT_InitStructure; //�ṹ����

	WDT_InitStructure.WDT_Enable = ENABLE;			 //�ж�ʹ��   ENABLE��DISABLE
	WDT_InitStructure.WDT_IDLE_Mode = WDT_IDLE_STOP; // IDLEģʽ�Ƿ�ֹͣ����		WDT_IDLE_STOP,WDT_IDLE_RUN
	WDT_InitStructure.WDT_PS = WDT_SCALE_16;		 //���Ź���ʱ��ʱ�ӷ�Ƶϵ��		WDT_SCALE_2,WDT_SCALE_4,WDT_SCALE_8,WDT_SCALE_16,WDT_SCALE_32,WDT_SCALE_64,WDT_SCALE_128,WDT_SCALE_256
	WDT_Inilize(&WDT_InitStructure);				 //��ʼ��
}

/******************** task A **************************/
void main(void)
{
	GPIO_config();
	Timer_config();
	WDT_config();
	EA = 1;

	while (1)
	{
		sensorIdAdjustTask();	//̽ͷλ��ʶ������
		AppTask();			//�����̴�������
		BeepAlarmTask();	//��������������
		SendOncePlusTask(); //��������Ⱥ����
		WDT_Clear();		// ι��
	}
}

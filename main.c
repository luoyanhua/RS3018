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

	GPIO_InitStructure.Pin = GPIO_Pin_All;		//ָ��Ҫ��ʼ����IO, GPIO_Pin_0 ~ GPIO_Pin_7, �����
	GPIO_InitStructure.Mode = GPIO_OUT_PP;		//ָ��IO������������ʽ,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P3, &GPIO_InitStructure); //��ʼ��
}

/***************  ���ڳ�ʼ������ *****************/
void UART_config(void)
{
	COMx_InitDefine COMx_InitStructure; //�ṹ����

	COMx_InitStructure.UART_Mode = UART_8bit_BRTx;	 //ģʽ,       UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	COMx_InitStructure.UART_BRT_Use = BRT_Timer1;	 //ѡ�����ʷ�����,   BRT_Timer1, BRT_Timer2 (ע��: ����2�̶�ʹ��BRT_Timer2)
	COMx_InitStructure.UART_BaudRate = 115200UL;	 //������, һ�� 110 ~ 115200
	COMx_InitStructure.UART_RxEnable = ENABLE;		 //��������,   ENABLE��DISABLE
	COMx_InitStructure.BaudRateDouble = DISABLE;	 //�����ʼӱ�, ENABLE��DISABLE
	COMx_InitStructure.UART_Interrupt = ENABLE;		 //�ж�����,   ENABLE��DISABLE
	COMx_InitStructure.UART_Priority = Priority_0;	 //ָ���ж����ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
	COMx_InitStructure.UART_P_SW = UART1_SW_P30_P31; //�л��˿�,   UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17,UART1_SW_P43_P44
	UART_Configuration(UART1, &COMx_InitStructure);	 //��ʼ������1 UART1,UART2,UART3,UART4

	COMx_InitStructure.UART_Mode = UART_8bit_BRTx;	 //ģʽ,       UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
													 //	COMx_InitStructure.UART_BRT_Use   = BRT_Timer2;			//ѡ�����ʷ�����,   BRT_Timer1, BRT_Timer2 (ע��: ����2�̶�ʹ��BRT_Timer2)
	COMx_InitStructure.UART_BaudRate = 4800UL;		 //������, һ�� 110 ~ 115200
	COMx_InitStructure.UART_RxEnable = DISABLE;		 //��������,   ENABLE��DISABLE
	COMx_InitStructure.BaudRateDouble = DISABLE;	 //�����ʼӱ�, ENABLE��DISABLE
	COMx_InitStructure.UART_Interrupt = DISABLE;	 //�ж�����,   ENABLE��DISABLE
	COMx_InitStructure.UART_Priority = Priority_0;	 //ָ���ж����ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
	COMx_InitStructure.UART_P_SW = UART2_SW_P10_P11; //�л��˿�, UART2_SW_P10_P11,UART2_SW_P46_P47
	UART_Configuration(UART2, &COMx_InitStructure);	 //��ʼ������1 UART1,UART2,UART3,UART4

	PrintString1("STC8 UART1 OK!\r\n"); //UART1����һ���ַ���
	PrintString2("STC8 UART2 OK!\r\n"); //UART1����һ���ַ���
}

/************************ ��ʱ������ ****************************/
void Timer_config(void)
{
	TIM_InitTypeDef TIM_InitStructure;							   //�ṹ����
	TIM_InitStructure.TIM_Mode = TIM_16BitAutoReload;			   //ָ������ģʽ,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_Priority = Priority_0;				   //ָ���ж����ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
	TIM_InitStructure.TIM_Interrupt = ENABLE;					   //�ж��Ƿ�����,   ENABLE��DISABLE
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_1T;				   //ָ��ʱ��Դ,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut = DISABLE;						   //�Ƿ������������, ENABLE��DISABLE
	TIM_InitStructure.TIM_Value = 65536UL - (MAIN_Fosc / 80000UL); //��ֵ,    80KHz   12.5us�ж�һ��
	TIM_InitStructure.TIM_Run = ENABLE;							   //�Ƿ��ʼ����������ʱ��, ENABLE��DISABLE
	Timer_Inilize(Timer0, &TIM_InitStructure);					   //��ʼ��Timer0	  Timer0,Timer1,Timer2,Timer3,Timer4

	TIM_InitStructure.TIM_Mode = TIM_16BitAutoReload;			  //ָ������ģʽ,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_Priority = Priority_0;				  //ָ���ж����ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
	TIM_InitStructure.TIM_Interrupt = ENABLE;					  //�ж��Ƿ�����,   ENABLE��DISABLE
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_1T;				  //ָ��ʱ��Դ, TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut = DISABLE;						  //�Ƿ������������, ENABLE��DISABLE
	TIM_InitStructure.TIM_Value = 65536UL - (MAIN_Fosc / 2000UL); //��ֵ,    2KHz   0.5mS�ж�һ��
	TIM_InitStructure.TIM_Run = ENABLE;							  //�Ƿ��ʼ����������ʱ��, ENABLE��DISABLE
	Timer_Inilize(Timer1, &TIM_InitStructure);					  //��ʼ��Timer1	  Timer0,Timer1,Timer2,Timer3,Timer4
}

/******************** INT���� ********************/
void Exti_config(void)
{
	EXTI_InitTypeDef Exti_InitStructure; //�ṹ����

	Exti_InitStructure.EXTI_Interrupt = ENABLE;	   //�ж�ʹ��,   ENABLE��DISABLE
	Exti_InitStructure.EXTI_Mode = EXT_MODE_Fall;  //�ж�ģʽ,   EXT_MODE_RiseFall,EXT_MODE_Fall
	Exti_InitStructure.EXTI_Priority = Priority_0; //ָ���ж����ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
	Ext_Inilize(EXT_INT1, &Exti_InitStructure);	   //��ʼ��
}

/******************** WDT���� ********************/
void WDT_config(void)
{
	WDT_InitTypeDef WDT_InitStructure; //�ṹ����

	WDT_InitStructure.WDT_Enable = ENABLE;			 //�ж�ʹ��   ENABLE��DISABLE
	WDT_InitStructure.WDT_IDLE_Mode = WDT_IDLE_STOP; //IDLEģʽ�Ƿ�ֹͣ����		WDT_IDLE_STOP,WDT_IDLE_RUN
	WDT_InitStructure.WDT_PS = WDT_SCALE_16;		 //���Ź���ʱ��ʱ�ӷ�Ƶϵ��		WDT_SCALE_2,WDT_SCALE_4,WDT_SCALE_8,WDT_SCALE_16,WDT_SCALE_32,WDT_SCALE_64,WDT_SCALE_128,WDT_SCALE_256
	WDT_Inilize(&WDT_InitStructure);				 //��ʼ��
}

/******************** task A **************************/
void main(void)
{
	GPIO_config();
	UART_config();
	Timer_config();
	WDT_config();
	Exti_config();
	EA = 1;

	while (1)
	{
		AppTask();	//�����̴�������
		BeepAlarmTask();	//��������������
		SendOncePlusTask();	//��������Ⱥ����
		WDT_Clear(); // ι��
	}
}

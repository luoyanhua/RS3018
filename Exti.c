/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCMCU.com --------------------------------------------*/
/* --- Web: www.STCMCUDATA.com  ---------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* 如果要在程序中使用此代码,请在程序中注明使用了STC的资料及程序            */
/*---------------------------------------------------------------------*/

/*************	功能说明	**************

本文件为STC8系列的外中断初始化和中断程序,用户可以在这个文件中修改自己需要的中断程序.

******************************************/

#include "Exti.h"
#include "timer.h"

/*************  外部函数和变量声明 *****************/
bit recvPlusFlag = 0;

u8 recvPlusCnt = 0;			//脉冲个数
u32 recvPlusTimeCnt = 0;	//连续脉冲时间

//开启接收超声波
void Start_recvPlus(void)
{
	recvPlusFlag = 1;
	recvPlusCnt = 0;
	recvPlusTimeCnt = 0;
}

//用于判断是否接收完成
bit Get_recvPlusFlag(void)
{
	return recvPlusFlag;
}

/********************* INT1中断函数 *************************/
void Ext_INT1(void) interrupt INT1_VECTOR //进中断时已经清除标志
{
	if (recvPlusFlag == 0)	//接收开始标志 0：接收完成或未开始，1：接收未完成
		return;

	if (get_time_escape_sec(Get_RecvPlusTimerCnt(),recvPlusTimeCnt) <= 4)			//波形连续才开始计数
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
// 函数: u8	Ext_Inilize(u8 EXT, EXTI_InitTypeDef *INTx)
// 描述: 外部中断初始化程序.
// 参数: INTx: 结构参数,请参考Exti.h里的定义.
// 返回: 成功返回0, 空操作返回1,错误返回2.
// 版本: V1.0, 2012-10-22
//========================================================================
u8 Ext_Inilize(u8 EXT, EXTI_InitTypeDef *INTx)
{
	if (EXT == EXT_INT1) //外中断1
	{
		IE1 = 0; //外中断1标志位
		if (INTx->EXTI_Interrupt == ENABLE)
			EX1 = 1; //允许中断
		else
			EX1 = 0; //禁止中断
		if (INTx->EXTI_Priority > Priority_3)
			return 2;						//错误
		INT1_Priority(INTx->EXTI_Priority); //指定中断优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
		if (INTx->EXTI_Mode == EXT_MODE_Fall)
			IT1 = 1; //下降沿中断
		else
			IT1 = 0; //上升,下降沿中断
		return 0;	 //成功
	}
	return 2; //失败
}

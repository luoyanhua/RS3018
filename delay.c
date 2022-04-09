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

/***************	功能说明	****************

本文件为STC8系列的延时程序,用户几乎可以不修改这个程序.

******************************************/

#include "delay.h"
#include "timer.h"

//========================================================================
// 函数: void delay_ms(u8 ms)
// 描述: 延时函数。
// 参数: ms,要延时的ms数, 这里只支持1~255ms. 自动适应主时钟.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-3-9
// 备注:
//========================================================================

unsigned int delayCnt = 0;

void Delay208us(void) //@24.000MHz
{
//	unsigned char i, j;

//	i = 7;
//	j = 121;
//	do
//	{
//		while (--j);
//	} while (--i);	
	
	unsigned char i, j;

	i = 4;
	j = 59;
	do
	{
		while (--j);
	} while (--i);
}

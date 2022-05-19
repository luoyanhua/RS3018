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
unsigned int meterDistance = 0; //单位是mm

//获取测试距离值
unsigned int Get_meterDistance(void)
{
	return meterDistance;
}

//获取传感器状态
bit Get_sensorOkFlag(void)
{
	return sensorOkFlag;
}

//获取任务状态
bit Get_SendOncePlusTaskFlag(void)
{
	return SendOncePlusTaskFlag;
}

//获取存在障碍物状态
bit Get_obstaclesExistenceFlag(void)
{
	return obstaclesExistenceFlag;
}

//准备开始测试，初始化相关参数
void Start_sendOncePlusTask(void)
{
	SendOncePlusTaskFlag = 1;
	SendContinuePlusState = START_SEND_STATE;
}

// sendMode 0:自检模式 1:正常测试模式
//单次发送脉冲群任务
void SendOncePlusTask(void)
{
	if (SendOncePlusTaskFlag == 0)
		return;
	switch (SendContinuePlusState)
	{
	case START_SEND_STATE:
		if(get_uartUsedFlag() == 0)		//避免串口和高速定时器同时工作，影响串口数据
		{
			SendContinuePlusTimeCnt = Get_us_250Cnt(); //单位50us
			Start_SendPlus();							   //发送超声波
			meterDistance = 0;							   //每次发送前距离清零
			SendContinuePlusState = WAIT_SEND_END_STATE;
		}
		break;
	case WAIT_SEND_END_STATE:
		if (Get_plusOutFlag() == 0) //等待发送完成
		{
			if (sendMode == 0)
			{
				Start_recvPlus(); //开启接收返回信号
				SendContinuePlusState = START_SELF_CHECK_STATE;
			}
			else
			{
				if (get_time_escape_sec(Get_us_250Cnt(), SendContinuePlusTimeCnt) >= 6) // 1.5ms后开始计算距离
				{
					Start_recvPlus(); //开启接收返回信号
					SendContinuePlusState = START_METER_STATE;
				}
			}
		}
		break;
	case START_SELF_CHECK_STATE:
		if (Get_recvPlusFlag() == 0) //如果接收完成，肯定收到波形
		{
			sendMode = 1;
			sensorOkFlag = 1;
			StartBeepAlarm(500,0);	  //正常收到返回信号	0.5S
			SendOncePlusTaskFlag = 0; //自检完成后停止任务运行
		}
		else if (get_time_escape_sec(Get_us_250Cnt(), SendContinuePlusTimeCnt) >= 160) //如果未收到连续波形，则判断是否超时
		{
			sensorOkFlag = 0;
			StartBeepAlarm(2000,0);	  //没有收到返回信号，故障	2S
			SendOncePlusTaskFlag = 0; //自检完成后停止任务运行
			Stop_recvPlus();
		}
		break;
	case START_METER_STATE:
		if (Get_recvPlusFlag() == 0) //收到波形，开始计算距离
		{
			meterDistance = get_time_escape_sec(Get_us_250Cnt(), SendContinuePlusTimeCnt) * 2.5 * 17; // 340/2=170路程来回，1个差值代表250us
			if (meterDistance < 250)																	//最短距离0.25米
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
		//最远3米，大概是17.6ms，暂定40ms
		else if (get_time_escape_sec(Get_us_250Cnt(), SendContinuePlusTimeCnt) >= 160) //如果未收到连续波形，则判断是否超时
		{
			//发送无障碍物数据
			obstaclesExistenceFlag = 0;
			SendOncePlusTaskFlag = 0; //一次测试结束，未收到返回波形
			meterDistance = 160;	  //没探测到数据，距离上报0x10
			Stop_recvPlus();
		}
		break;
	default:
		break;
	}
}
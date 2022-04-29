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

#ifndef	__SENSOR_H
#define	__SENSOR_H

#define SENSOR_NUM_MAX 4

#define LEFT_SENSOR 0
#define LEFT_MID_SENSOR 1
#define RIGHT_MID_SENSOR    2
#define RIGHT_SENSOR    3

//获取测试距离值
unsigned int Get_meterDistance(void);

//获取传感器状态
bit Get_sensorOkFlag(void);

//获取任务状态
bit Get_SendOncePlusTaskFlag(void);

//获取存在障碍物状态
bit Get_obstaclesExistenceFlag(void);

void Start_sendOncePlusTask(void);

//单次发送脉冲群任务
void SendOncePlusTask(void);

#endif

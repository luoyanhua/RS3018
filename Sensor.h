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

#ifndef	__SENSOR_H
#define	__SENSOR_H

#define SENSOR_NUM_MAX 4

#define LEFT_SENSOR 0
#define LEFT_MID_SENSOR 1
#define RIGHT_MID_SENSOR    2
#define RIGHT_SENSOR    3

//��ȡ���Ծ���ֵ
unsigned int Get_meterDistance(void);

//��ȡ������״̬
bit Get_sensorOkFlag(void);

//��ȡ����״̬
bit Get_SendOncePlusTaskFlag(void);

//��ȡ�����ϰ���״̬
bit Get_obstaclesExistenceFlag(void);

void Start_sendOncePlusTask(void);

//���η�������Ⱥ����
void SendOncePlusTask(void);

#endif

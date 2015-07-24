/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           common.h
** Last modified Date:  2013-01-06
** Last Version:        No
** Descriptions:        ϵͳ�������弰ͨ�ú�������                     
**------------------------------------------------------------------------------------------------------
** Created by:          sunway
** Created date:        2013-01-06
** Version:             V0.1
** Descriptions:        The original version        
********************************************************************************************************/
#ifndef __COMMON_H 
#define __COMMON_H

#define MAXLANGUAGETYPE 5 //ϵͳ֧�������������࣬Ŀǰ�ݶ�5�֣����ģ�Ӣ�ģ������䣬����˹��������





















/******************************************��������*****************************************************/
typedef struct	
{
	uint8_t		CrcCheck[2];					//CRCУ��
} GOODSCHANNELPARAMETER;
/******************************************���ײ���*****************************************************/
typedef struct	
{
	uint8_t		CrcCheck[2];					//CRCУ��
} VENDINGPARAMETER;

/******************************************���׼�¼*****************************************************/
typedef struct	
{
	uint8_t		CrcCheck[2];					//CRCУ��
} VENDINGLOG;

/******************************************λͼ��ʽ*****************************************************/
typedef struct
{
	unsigned char Width;			//λͼ����
	unsigned char Hight;			//λͼ�߶�
	const unsigned char *Data;		//λͼ����
} BMP;
/******************************************��ʱ������*****************************************************/
typedef struct	
{
	uint8_t		PayoutTimer;					//���ҵȴ���ʱ��
	uint8_t		EscrowTimer;					//ѹ���ȴ���ʱ��
	uint8_t		DispFreeTimer;					//���׽�����н���ˢ�¶�ʱ��
	uint8_t		CheckDevStateTimer;				//����豸����״̬��ʱ��
	uint8_t		GetTubeTimer;					//����豸����״̬��ʱ��
	uint8_t		ChaxunTimer;					//���׽������������ѯ��ʱ��
	uint8_t		SaleTimer;					    //���׵���ʱ��ʱ��
	uint8_t		PCRecTimer;					    //PC���ڽ��ն�ʱ��
	uint8_t		ActionPCTimer;					//UBOX��ά��ģʽ��Action��ʱ��
	uint8_t		offLinePCTimer;				    //UBOX������ģʽ�¶�ʱ��
	uint8_t     printTimer;                     //��ӡ���׼�¼��ʱ��
	uint8_t		selTimer;						//ѡ���������ն�ʱ��

	//add by yoc zhihui 2013.12.16
	uint8_t    	zhihui_recv_timeout;
	uint8_t		zhihui_button_timeout;
	uint8_t     zhihui_trade_time;
	uint8_t		pcm_handle_timeout;	//�ۻ�������PC����ʱ add by yoc newAisle
	
	uint8_t		getTempTimer;				  //��ȡ�¶ȶ�ʱ��
} TIMER;
extern TIMER Timer;



unsigned char XorCheck(unsigned char *pstr,unsigned short len);
unsigned short CrcCheck(unsigned char *msg, unsigned short len);


#endif
/**************************************End Of File*******************************************************/
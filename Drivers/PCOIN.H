/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           pcoin.c
** Last modified Date:  2013-01-12
** Last Version:         
** Descriptions:        ����Ӳ�����ӿ�(���С�����)                     
**------------------------------------------------------------------------------------------------------
** Created by:          sunway
** Created date:        2013-01-12
** Version:             V0.1
** Descriptions:        The original version       
********************************************************************************************************/
#ifndef __PCOIN_H 
#define __PCOIN_H

//����Ӳ����
void InitParallelPluseCoinAcceptor(void);			//��ʼ������Ӳ����
void EnableParallelPluseCoinAcceptor(void);			//ʹ�ܲ���Ӳ����
void DisableParallelPluseCoinAcceptor(void);		//���ܲ���Ӳ����
unsigned char GetParallelCoinAcceptorStatus(void);	//��ȡ����Ӳ����״̬
unsigned char ReadParallelCoinAcceptor(void);		//��ȡ����Ӳ����ͨ��ֵ
void ScanPPCoinChannel(void);						//Time1��ʱɨ�貢��Ӳ������Ӧ�ò㲻�õ���

//����Ӳ����
void InitSerialPluseCoinAcdeptor(void);				//��ʼ������Ӳ����
void EnableSerialPluseCoinAcceptor(void);			//ʹ�ܴ���Ӳ����
void DisableSerialPluseCoinAcceptor(void);			//���ܴ���Ӳ����
unsigned char GetSerialCoinAcceptorStatus(void);	//��ȡ����Ӳ����״̬
unsigned char ReadSerialPluseCoinAcceptor(void);	//��ȡ����Ӳ����ͨ��ֵ
void ScanSPCoinChannel(void);						//Time1��ʱɨ�贮��Ӳ������Ӧ�ò㲻�õ���

#endif
/**************************************End Of File*******************************************************/

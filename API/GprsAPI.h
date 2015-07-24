#ifndef _GPRSAPI_H
#define _GPRSAPI_H



struct STGPRS
{
	//ģ��ͨ��״̬
	unsigned char Communication;
	//��ǰ�������óɹ����
	unsigned char QiFGCNT;
	//����ģ��GPRS�����Ƿ�ɹ�
	unsigned char QiCSGP;
	//����ģ�����ݴ���Ϊ��͸ģʽ
	unsigned char QiMODE;
	//���ͨ�ſ���PIN�Ƿ����
	unsigned char QiCPIN;
	//ģ��ĵ�ǰ�ź�ǿ��
	unsigned char QiCSQ;
	//ģ���Ƿ�ɹ�ע��GSM����
	unsigned char QiCREG;
	//ģ���Ƿ�ɹ�ע��GPRS����
	unsigned char QiCGREG;
	//ģ��ĵ�ǰ״̬
	unsigned char QiSTAT;
	//�����������ý����APN���û����������Ƿ�ɹ�
	unsigned char QiREGAPP;
	//������Ƿ�ɹ�
	unsigned char QiACT;
	//��ñ���IP�Ƿ�ɹ�
	unsigned char QiLOCIP;
	//���õ�·���·����ģʽ��0������· 1������·
	unsigned char QiMUX;
	//��ǰ��������������
	unsigned char QiOPEN;
	//����IP
	unsigned char PlaceIP[15];
	//���ӷ�����IP
	unsigned char ServerIP[15];
	//���ӷ�����IP��ַ����
	unsigned char ServerIPLen;
	//�������˿�
	unsigned char ServerPort[5];
	//�������˿ڵ�ַ����
	unsigned char ServerPortLen;
	//ģ�鵱ǰ״̬
	//1:IP INITIAL        2:IP START        3:IP CONFIG
	//4:IP IND            5:IP GPRSACK      6:IP STATUS
	//7:TCP CONNECTING    8:UDP CONNECTING  9:IP CLOSE
	//10:CONNECT OK       11:PDP DEACT
	unsigned char State;
};

extern struct STGPRS stGprsStruct;

unsigned char GPRS_Init(void);

unsigned char GPRS_PutInIP(unsigned char type,unsigned char *IpBuf,unsigned char *PortBuf);

extern unsigned char GPRS_QISEND(unsigned char *sbuf,unsigned char len);

extern unsigned char GPRS_QIOPEN(unsigned char *IPbuf,unsigned char *Port,unsigned char Iplen,unsigned char PortLen);

unsigned char GPRS_SendReport(unsigned char *Reportbuf,unsigned char len);

unsigned char GPRS_GetAckReport(unsigned char *buf,unsigned char *len);

unsigned char GPRS_QISTATE(void);

unsigned char GPRS_QIRD(unsigned char *AckBuf);

unsigned char GPRS_QINDI(void);

unsigned char GPRS_QIPOWERDOWN(void);

unsigned char GPRS_QICLOSE(void);

unsigned char GPRS_QIDEACT(void);

unsigned char GPRS_QIACT(void);
unsigned char mvc_gprs_init(void);



#endif

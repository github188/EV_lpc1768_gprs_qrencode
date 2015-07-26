/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           gprs.h
** Last modified Date:  2015-08-04
** Last Version:         
** Descriptions:        MG323 GPRS���������ļ�                  
**------------------------------------------------------------------------------------------------------
** Created by:          yanbo 
** Created date:        2013-01-04
** Version:              
** Descriptions:        The original version       
********************************************************************************************************/

#include "..\config.h"
#include "gprs.h"


#define GPRS_DEBUG
#ifdef GPRS_DEBUG
#define print_gprs(...)	Trace(__VA_ARGS__)
#else
#define print_gprs(...)
#endif



typedef void (*FUN_uartClear)(void);
typedef void (*FUN_UartPutStr)(unsigned char const *, unsigned int );
typedef unsigned char (*FUN_UartBuffIsNotEmpty)(void);
typedef unsigned char (*FUN_UartGetCh)(void);



static FUN_uartClear UART_clear = uart3_clr_buf;
static FUN_UartPutStr UART_putStr = Uart3PutStr;
static FUN_UartBuffIsNotEmpty UART_isNotEmpty = Uart3BuffIsNotEmpty;
static FUN_UartGetCh UART_getchar = Uart3GetCh;

#define GPRS_BUF_SIZE	1024
static char gprs_buf[GPRS_BUF_SIZE] = {0};
static uint16 gprs_len = 0;

GPRS_ST st_gprs;

#define GPRS_IP_SIZE		8
GPRS_IP	st_ip[8];

uint8 GPRS_send(char *str,...)
{
	va_list arg_ptr;
	uint16 len;
	memset(gprs_buf,0,sizeof(gprs_buf));
	va_start(arg_ptr, str);
	len = vsprintf(gprs_buf,(const char *)str,arg_ptr);
	va_end(arg_ptr);
	UART_clear();
	UART_putStr((const uint8 *)gprs_buf,len);
	print_gprs("GPRS_Send[%d]:%s\r\n",len,gprs_buf);
	return 1;
}

//���� ��ȡ���ݳ���
uint16 GPRS_readAll(uint8 *buf,uint32 timeout,uint16 subTimeout)
{
	uint16 index = 0;
	Timer.gprs_uart_timeout = (timeout > 10) ? timeout / 10 : 1;
	print_gprs("GPRS_Recv:");
	while(Timer.gprs_uart_timeout){
		if(UART_isNotEmpty()){
			 buf[index++] =  UART_getchar();
			 print_gprs("%c",buf[index - 1]);
			 Timer.gprs_recv_over = (subTimeout > 50) ? subTimeout / 10 : 5;	 
		}
		else{
			msleep(10);
		}

		if(index > 0 && Timer.gprs_recv_over == 0){ //�������
			print_gprs("\r\n");
			return index;
		}
	}
	print_gprs("\r\n");
	return index;

}


char *GPRS_strstr(const char *src,const char *str,...)
{
	char buf[64] = {0};
	uint16 len;	
	va_list arg_ptr;

	memset(buf,0,sizeof(buf));
	va_start(arg_ptr, str);
	len = vsprintf(buf,(const char *)str,arg_ptr);
	va_end(arg_ptr);

    return strstr(src,buf);

}

//ָ�������ַ��� 1��ȷ 0����ȷ
uint8 GPRS_recvByStr(uint32 timeout,uint32 subTimeout,char *str,...){

	char buf[64] = {0};
	uint16 len;	
	va_list arg_ptr;

	memset(buf,0,sizeof(buf));
	va_start(arg_ptr, str);
	len = vsprintf(buf,(const char *)str,arg_ptr);
	va_end(arg_ptr);


	len = GPRS_readAll((uint8 *)gprs_buf,timeout,subTimeout);
	if(len > 0){
		if(strstr(gprs_buf,buf) == NULL){
			return 0;
		}
		else{
			return 1;
		}	
	}
	else{
		return 0;
	}	
}



// 0 ��ʱ 1�ɹ� 2����
uint8 GPRS_sendAT(char *str,...)
{

	va_list arg_ptr;
	uint32 len;
	memset(gprs_buf,0,sizeof(gprs_buf));
	va_start(arg_ptr, str);
	len = vsprintf(gprs_buf,(const char *)str,arg_ptr);
	va_end(arg_ptr);
	
	strcat(gprs_buf,"\r");

	UART_clear();
	UART_putStr((const uint8 *)gprs_buf,len + 1);
	print_gprs("GPRS_Send[%d]:%s\r\n",len + 1,gprs_buf);
	memset(gprs_buf,0,sizeof(gprs_buf));


	len = GPRS_readAll((uint8 *)gprs_buf,2000,500);
	if(len == 0){
		print_gprs("GPRS:GPRS_TIMEOUT!!!!\r\n");
		return GPRS_TIMEOUT;
	}

	if(strstr(gprs_buf,"\r\nOK\r\n") != NULL){
		print_gprs("GPRS:OK!!!!\r\n");
		return GPRS_OK;
	}
	
    if(strstr(gprs_buf,"\r\nERROR\r\n") != NULL){
		print_gprs("GPRS:ERROR!!!!\r\n");
		return GPRS_ERROR;
	}

	if(strstr(gprs_buf,"+CME ERROR:") != NULL){
		print_gprs("GPRS:+CME ERROR:!!!!\r\n");
		return GPRS_CME_ERROR;
	}

	return GPRS_OTHER; 

}



/*********************************************************************************************************
** Function name:       GPRS_AT_CSQ
** Descriptions:        ��ѯģ�������ź� 
** input parameters:    ��
** output parameters:   ��
** Returned value:      0��ʾͨ�Ź��� 31��ʾ�źźܺ�  1-30
*********************************************************************************************************/
uint8 GPRS_AT_CSQ(void)
{
	uint8 res,rssi;
	char *p,*q,buf[10] = {0};
	res = GPRS_sendAT("AT+CSQ");
	if(res == GPRS_OK){
		p = strstr((char *)gprs_buf,"+CSQ:");
		if(p == NULL){
			return 0;
		}
		p = p + 5 + 1; // "+CSQ:" + " "
	    q = strstr((char *)p,",");
		if(q == NULL){
			return 0;
		}
		memset(buf,0,sizeof(buf));
		strncpy(buf,p,q - p);
		rssi = atoi(buf);
		print_gprs("GPRS:rssi = %s %d\r\n",buf,rssi);
		return rssi;
	}
	else{
		print_gprs("GPRS:'AT+CSQ' Fail\r\n");
		return 0;	
	}
		  	
}




/*********************************************************************************************************
** Function name:       GPRS_AT_CREG
** Descriptions:        ��ѯ��ǰ����ע��״̬
** input parameters:    ��
** output parameters:   ��
** Returned value:      0��ʾûע��  1��ʾע��
*********************************************************************************************************/
uint8 GPRS_AT_CREG(void)
{
	uint8 res;
	char *p;
	res = GPRS_sendAT("AT+CREG?");
   	if(res == GPRS_OK){
		 p = strstr((char *)gprs_buf,",");
		 if(p == NULL){
		 	return 0;
		 }
		 p = p + 1; //����','
		 if(*p == '1' || *p == '5'){
		 	print_gprs("GPRS:AT+CREG? = OK\r\n");
		 	return 1;
		 }
		 else{
		 	return 0;
		 }
	}
	else{
		return 0;
	}


}

/*********************************************************************************************************
** Function name:       GPRS_createProfile
** Descriptions:        ����Profile����
** input parameters:    ��
** output parameters:   ��
** Returned value:      0ʧ��  1�ɹ�
*********************************************************************************************************/
uint8 GPRS_createProfile(void)
{
	uint8 res;
	res = GPRS_sendAT("AT^SICS=0,conType,GPRS0");
   	if(res != GPRS_OK){
	    print_gprs("GPRS:'AT^SICS=0,conType,GPRS0' ERR!!!\r\n");
		return 0;
	}

	res = GPRS_sendAT("AT^SICS=0,apn,cmnet");
   	if(res != GPRS_OK){
	    print_gprs("GPRS:'AT^SICS=0,apn,cmnet' ERR!!!\r\n");
		return 0;
	}


	res = GPRS_sendAT("AT^SICS=0,user,yoc");
   	if(res != GPRS_OK){
	    print_gprs("GPRS:'AT^SICS=0,user,yoc' ERR!!!\r\n");
		return 0;
	}

	res = GPRS_sendAT("AT^SICS=0,passwd,123");
   	if(res != GPRS_OK){
	    print_gprs("GPRS:'AT^SICS=0,passwd,123' ERR!!!\r\n");
		return 0;
	}
	print_gprs("GPRS:GPRS_createProfile OK!\r\n");
	return 1;

}


/*********************************************************************************************************
** Function name:       GPRS_AT_SISS
** Descriptions:        ����Profile����  �˴�����TCP �׽���
** input parameters:    ��
** output parameters:   ��
** Returned value:      0ʧ��  1�ɹ�
*********************************************************************************************************/
uint8 GPRS_AT_SISS(GPRS_IP *ip)
{
	uint8 res;
	res = GPRS_sendAT("AT^SISS=%d,srvType,socket",ip->ch);
   	if(res != GPRS_OK){
	    print_gprs("GPRS:'AT^SISS,srvType,socket' ERR!!!\r\n");
		return 0;
	}

	res = GPRS_sendAT("AT^SISS=%d,conId,0",ip->ch);
   	if(res != GPRS_OK){
	    print_gprs("GPRS:'AT^SISS=1,conId,0' ERR!!!\r\n");
		return 0;
	}

	res = GPRS_sendAT("AT^SISS=%d,address,\"socktcp://%s:%d\"",ip->ch,ip->ip,ip->port);
   	if(res != GPRS_OK){
	    print_gprs("AT^SISS=%d,address,\"socktcp://%s:%d\" ERR\r\n",ip->ch,ip->ip,ip->port);
		return 0;
	}

	print_gprs("GPRS:GPRS_createProfileServer OK!\r\n");
	return 1;
}

/*********************************************************************************************************
** Function name:       GPRS_AT_SISO
** Descriptions:        ������
** input parameters:    ��
** output parameters:   ��
** Returned value:      0ʧ��  1�ɹ�
*********************************************************************************************************/
uint8 GPRS_AT_SISO(uint8 ch)
{
	char *p;
	GPRS_send("AT^SISO=%d\r",ch);
	gprs_len = GPRS_readAll((uint8 *)gprs_buf,5000,1000);
   	if(gprs_len == 0){
	    print_gprs("GPRS:'AT^SISO=1' ERR!!!\r\n");
		return 0;
	}

	p = strstr(gprs_buf,"OK");
	if(p == NULL){
		print_gprs("GPRS:'AT^SISO=1' ERR!!!\r\n");
		return 0;
	}
	else{
		print_gprs("GPRS:GPRS_AT_SISO OK!\r\n");
		return 1;
	}
}


uint8 GPRS_AT_SISC(uint8 ch)
{
	uint8 res;
	res = GPRS_sendAT("AT^SISC=%d",ch);
	if(res == GPRS_OK){return 1;}
	else {return 0;}
}



//"http://117.27.89.53:10001/yv/api/vmcCheckin"
uint8 GPRS_getIP(const char *url,GPRS_IP *ip)
{
	char buf[12] = {0};
	char *p,*q;
	if(url == NULL || ip == NULL){return 0;}
	p = strstr(url,"http://");
	if(p == NULL){return 0;}
	p = p + 7;
	q = strstr(p,":");
	if(q == NULL){return 0;}
	memset(ip->ip,0,sizeof(ip->ip));
	strncpy(ip->ip,p,q - p);
	p = q + 1;
	q = strstr(p,"/");
	if(q == NULL){return 0;}
	memset(buf,0,sizeof(buf));
	strncpy(buf,p,q - p);
	ip->port = atoi(buf);
	strcpy(ip->path,q);
	print_gprs("ip=%s,port=%d,path=%s\r\n",ip->ip,ip->port,ip->path);
	return 1;
}


GPRS_IP *GPRS_check(GPRS_IP *ip){
	uint8 i;
	GPRS_IP *ipTr;
	if(ip == NULL){return NULL;}
	for(i = 0;i < GPRS_IP_SIZE;i++){
		ipTr = &st_ip[i];
		if(ipTr->port == 0){
			ipTr->port = ip->port;
			ipTr->ch = i;
			strcpy(ipTr->ip,ip->ip);
			strcpy(ipTr->path,ip->path);
			return ipTr;
		}
		else{
		 	if(strcmp(ipTr->ip,ip->ip) == 0){
				return ipTr;
			}
		}	
	}
	return NULL;		
}


uint8 GPRS_httpPostPack(const char *data,GPRS_IP *ip,char *httpData)
{
	uint16 temp;
	char buf[10] = {0};
	if(data == NULL || httpData == NULL ){return 0;}
	strcat(httpData,"POST ");
	strcat(httpData,ip->path);
	strcat(httpData," HTTP/1.1\r\n");
	strcat(httpData,"Accept:*/*\r\n");
	strcat(httpData,"Accept-Language: zh-cn\r\n");
	strcat(httpData,"Host: ");
	strcat(httpData,ip->ip);
	strcat(httpData,"\r\n");
	strcat(httpData,"Content-Type: application/x-www-form-urlencoded\r\n");

	memset(buf,0,sizeof(buf));
	sprintf(buf,"Content-Length: %d\r\n",strlen(data));
	strcat(httpData,buf);
	strcat(httpData,"\r\n");
	strcat(httpData,data);
	return 1;
}


//0��ʱ 1�ɹ�  0xFF ����
uint8 GPRS_createTcp(GPRS_IP *ip)
{
	uint8 res;
	char buf[64] = {0};
	char *p;
	res = GPRS_sendAT("AT^SICS?");
	if(res != GPRS_OK){
		return (res == GPRS_TIMEOUT) ? 0 : 0xFF;
	}

	p = strstr(gprs_buf,"^SICS: 0,\"user\",");
	if(p == NULL){ //û����
		 res = GPRS_createProfile();
		 if(res != 1){return 0xFF;}
	}

	res = GPRS_sendAT("AT^SISS?");
	if(res != GPRS_OK){
		return (res == GPRS_TIMEOUT) ? 0 : 0xFF;
	}

	memset(buf,0,sizeof(buf));
	sprintf(buf,"^SISS: %d,\"address\"",ip->ch);
	p = strstr(gprs_buf,buf);
	if(p == NULL){ //û����
		 res = GPRS_AT_SISS(ip);
		 if(res != 1){return 0xFF;}
	}


	return 1;


}

// data   "http://117.27.89.53:10001/yv/api/vmcCheckin"
uint8 GPRS_httpPost(const char *data,const char *url)
{
	uint8 res;
	uint16 rlen,len;
	//��ȡip
	GPRS_IP ip,*ipTr;
	res = GPRS_getIP(url,&ip);
	if(res != 1){return 0;}

	
	ipTr = GPRS_check(&ip);
	if(ipTr == NULL){return 0;}
		
	memset(gprs_buf,0,sizeof(gprs_buf));
	res = GPRS_httpPostPack(data,ipTr,gprs_buf);
	if(res != 1){return 0;}
	print_gprs("GPRS_httpPost3\r\n");
	len = strlen(data);
	print_gprs("GPRS_httpPost4\r\n");
   	GPRS_AT_SISC(ipTr->ch);
	res = GPRS_createTcp(ipTr);
	if(res != 1){return 0;}

	res = GPRS_AT_SISO(ipTr->ch);
	if(res != 1){return 0;}


	GPRS_send("AT^SISW=%d,%d\r",ipTr->ch,len);
	res = GPRS_recvByStr(5000,1000,"^SISW: %d,%d,%d",ipTr->ch,len,len);
	if(res != 1){return 0;}

	
	
	UART_clear();
	UART_putStr((const uint8 *)gprs_buf,strlen(gprs_buf));
	print_gprs("GPRS_Send[%d]:%s\r\n",strlen(gprs_buf),gprs_buf);
	
	rlen = GPRS_readAll((uint8 *)gprs_buf,10000,5000);
	if(rlen == 0){return 0;}
	else{
		if(GPRS_strstr((const char *)gprs_buf,"^SISW: %d,1",ipTr->ch) == NULL){
			return 0;
		}
		if(GPRS_strstr((const char *)gprs_buf,"^SISR: %d, 1",ipTr->ch) == NULL){
			return 0;
		}
	}
	
	res = GPRS_send("AT^SISR=%d,1500\r",ipTr->ch);
	rlen = GPRS_readAll((uint8 *)gprs_buf,20000,5000);
			
	return 1;
}

/*********************************************************************************************************
** Function name:       GPRS_tcpWrite
** Descriptions:        TCP�׽��ַ�������
** input parameters:    ��
** output parameters:   ��
** Returned value:      0ʧ��  1�ɹ�
*********************************************************************************************************/
uint8 GPRS_tcpWrite(const char *data,uint16 len,uint8 ch)
{
	uint8 buf[64] = {0},ln,res;
	uint16 rlen = 0;
	GPRS_send("AT^SISW=%d,%d\r",ch,len);
	res = GPRS_recvByStr(5000,1000,"^SISW: %d,%d,%d",ch,len,len);
	if(res != 1){return 0;}



	res = GPRS_send((char *)data);
	rlen = GPRS_readAll((uint8 *)gprs_buf,10000,8000);
	if(rlen == 0){return 0;}
	else{
		memset(buf,0,sizeof(buf));
		ln = sprintf((char *)buf,"^SISW: %d,1",ch);
		if(strstr((const char *)gprs_buf,(const char *)buf) == NULL){
			return 0;
		}

		memset(buf,0,sizeof(buf));
		ln = sprintf((char *)buf,"^SISR: %d, 1",ch);
		if(strstr((const char *)gprs_buf,(const char *)buf) == NULL){
			return 0;
		}
	}



	res = GPRS_send("AT^SISR=%d,1500\r",ch);
	rlen = GPRS_readAll((uint8 *)gprs_buf,10000,5000);

	return 1;
}




void GPRS_task(void)
{
	uint8 res;
	char buf[256] = {0};
	GPRS_IP ip;
	ip.port = 10001;
	ip.ch = 1;
	strcpy(ip.ip,"117.27.89.53");
	res = GPRS_httpPost("param={'vmc_no':'ev0001','vmc_auth_code':'0001'}",
					"http://117.27.89.53:10001/yv/api/vmcCheckin");
	print_gprs("GPRS_http:res=%d\r\n",res);
	msleep(5000);
#if 0
	//GPRS_getIP("http://117.27.89.53:10001/yv/api/vmcCheckin",&ip);

	GPRS_createTcp(&ip);
	
	GPRS_AT_SISC(1);
	msleep(500);
	GPRS_AT_SISO(1);
	msleep(500);
	memset(buf,0,sizeof(buf));
	strcat(buf,"POST /yv/api/vmcCheckin HTTP/1.1\r\n");
	strcat(buf,"Accept:*/*\r\n");
	strcat(buf,"Accept-Language: zh-cn\r\n");
	strcat(buf,"Host: 117.27.89.53\r\n");
	strcat(buf,"Content-Type: application/x-www-form-urlencoded\r\n");
	strcat(buf,"Content-Length: 48\r\n");
	strcat(buf,"\r\n");
	strcat(buf,"param={'vmc_no':'ev0001','vmc_auth_code':'0001'}");
	//strcat(buf,"\r\n");
	//strncat(gprs_buf,"\0",sizeof("\0"));
	print_gprs("GPRS:TCPSendLen=%d\r\n",strlen(buf));
	GPRS_tcpWrite(buf,strlen(buf),1);	 
	msleep(5000);
#endif
}








/**************************************End Of File*******************************************************/

/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           lcd.c
** Last Version:        V2.0 
** Descriptions:        lcd��������,����оƬU1608E���ӿ�ģʽS8                   
**------------------------------------------------------------------------------------------------------
** Created by:          sunway
** Created date:        2013-03-01
** Version:             V2.0 
** Descriptions:        The original version       
********************************************************************************************************/
#include "..\config.h"
#include "DOT_ASCII16.C"
#include "DOT_CHINESE16.C" 
#include "DOT_LOGO.C" 
#include "DOT_NUMBER.C"


#define LCDWIDTH	240
#define LCDHIGHT	16

#define SCL_SET()   FIO3DIR |= (1ul << 25); FIO3SET |= (1ul << 25)	//SCL Clock,P3.25
#define SCL_CLR()   FIO3DIR |= (1ul << 25); FIO3CLR |= (1ul << 25)
#define SDA_SET()   FIO1DIR |= (1ul << 24); FIO1SET |= (1ul << 24) 	//SDL DATA,P1.24
#define SDA_CLR()   FIO1DIR |= (1ul << 24); FIO1CLR |= (1ul << 24) 
#define nCS_SET()   FIO1DIR |= (1ul << 25); FIO1SET |= (1ul << 25) 	//CS ChipSel,P1.25
#define nCS_CLR()   FIO1DIR |= (1ul << 25); FIO1CLR |= (1ul << 25)
#define CD_SET()    FIO1DIR |= (1ul << 26); FIO1SET |= (1ul << 26) 	//CD Data&CMD,P1.26
#define CD_CLR()    FIO1DIR |= (1ul << 26); FIO1CLR |= (1ul << 26)

void UC1608EPutByte(unsigned char dat);
void UC1608EPutCmd(unsigned char cmd);
void UC1608EPutData(unsigned char dat);
void LCDPutCh(unsigned char X,unsigned char Y,unsigned char Ch);

/*********************************************************************************************************
** Function name:       LCDInit
** Descriptions:        ��ʼ��LCD,ͬʱ��Ļ���㲢�򿪱���
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void LCDInit(void)					    
{
	UC1608EPutCmd(0xE2);
	OSTimeDly(4);			//Wait for 20ms
    UC1608EPutCmd(0x26);   	//Set Multiplex Rate and Temperature Compensation
	UC1608EPutCmd(0x2f);   	//Set Power Controlinternal vlcd(11x charge pump)
	UC1608EPutCmd(0xea);   	//set bias=1/12
	UC1608EPutCmd(0x81);	//Electronic volume mode set
	UC1608EPutCmd(0x72);	
	UC1608EPutCmd(0xc8);	//set MX=0, SEG output normal direction		
    UC1608EPutCmd(0x40);    //set start line
	UC1608EPutCmd(0xAF);   	//set display enable
	
	LCDClrScreen();			//Clear Screen 
	LCDBackLight(1);		//BlackLight On 
}
/*********************************************************************************************************
** Function name:       LCDBackLight
** Descriptions:        LCD�������
** input parameters:    ctrl:1�򿪣�0���ر�
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void LCDBackLight(uint8_t ctrl)
{
	if(ctrl)
	{
		FIO2DIR |= (1ul<<8);
		FIO2SET |= (1ul<<8);	
	}
	else
	{
		FIO2DIR |= (1ul<<8);
		FIO2CLR |= (1ul<<8);	
	}		
}

void LCD_flush(const unsigned char x,const unsigned char w)
{
	unsigned char i,y,k;
	for(y = 0;y < w;y++){				//д�У�ҳ������
		k = y / 8;
		LCDSetCursor(x,k);
		for(i = 0;i < w;i++){ 		//д����ʾ����
			UC1608EPutData(0xFF);		//д��ʾ����0x00
		}
	} 

	LCDSetCursor(x,w / 8);
	for(i = 0;i < w;i++){
		UC1608EPutData(0x0F);
	}

}

/*********************************************************************************************************
** Function name:       LCDClrScreen
** Descriptions:        ����
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void LCDClrScreen(void)
{
	unsigned char x,y;
	for(y=0;y<LCDHIGHT;y++)				//д�У�ҳ������
	{
		LCDSetCursor(0,y);
		for(x=0;x<LCDWIDTH;x++) 		//д����ʾ����
		{
			UC1608EPutData(0x00);		//д��ʾ����0x00
		}
	}
}
/*********************************************************************************************************
** Function name:       LCDClrArea
** Descriptions:        �����Ļ(x1,y1)��(x2,y2)����
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void LCDClrArea(unsigned char x1,unsigned char y1,unsigned char x2,unsigned char y2)
{
	unsigned char i,j;
	for(i=0;i<=(y2-y1);i++)
	{
		for(j=x1;j<=x2;j++)
		{
			LCDSetCursor(j,y1+i);
			UC1608EPutData(0x00);
		}	
	}	
}
/*********************************************************************************************************
** Function name:       LCDSetCursor
** Descriptions:        ���ù������
** input parameters:    x,y:����
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void LCDSetCursor(unsigned char X,unsigned char Y)
{  	
	UC1608EPutCmd(Y | 0xB0);			// page address 
	UC1608EPutCmd(X>>4 | 0x10);			// column address MSB
	UC1608EPutCmd(X&0x0F | 0x00);		// column address LSB
}
/*********************************************************************************************************
** Function name:       LCDPutAscii
** Descriptions:        ��(x,y)��ʼλ����ʾһ��(8*16)�ַ�
** input parameters:    x:x���ꣻy:y���ꣻstr:ASC�룻Inverse:1---���ף�0---������
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void LCDPutAscii(unsigned char x,unsigned char y,char *str,unsigned char Inverse)
{
	char *OffSet;
	unsigned char i;
	uint16_t AddOffSet;
	OffSet = strstr(IndexASCII16_TAB,str);
	AddOffSet = (OffSet-IndexASCII16_TAB)<<4;
	if(Inverse)
	{
		for(i=0;i<8;i++)
		{
			LCDPutCh(x+i,y,~ASCII16_TAB[AddOffSet++]);
		}
		for(i=0;i<8;i++)
		{
			LCDPutCh(x+i,y+1,~ASCII16_TAB[AddOffSet++]);
		}
	}
	else
	{
		for(i=0;i<8;i++)
		{
			LCDPutCh(x+i,y,ASCII16_TAB[AddOffSet++]);
		}
		for(i=0;i<8;i++)
		{
			LCDPutCh(x+i,y+1,ASCII16_TAB[AddOffSet++]);
		}
	}
}
/*********************************************************************************************************
** Function name:       LCDPutAscii
** Descriptions:        ��(x,y)��ʼλ����ʾһ��(8*16)�ַ�,��LCDPutAscii������ͬ������������ʾ�������������ַ�
** input parameters:    x:x���ꣻy:y���ꣻstr:ASC�룻Inverse:1---���ף�0---������
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void LCDPutAsciiFont(unsigned char x,unsigned char y,uint8_t str,unsigned char Inverse)
{	
	uint16_t i,j=0;
	uint16_t ZiNum;
	
	for(i=0;i<NUMBERNO;i++)
	{
		if(str == NumberIndex[i])
		{
			ZiNum = i;
			break;
		}
	}
	if(i==NUMBERNO)
	{
		return;
	}
	else
	{		
		if(Inverse)
		{
			for(i=0;i<8;i++)
			{
				LCDPutCh(x+i,y,~Number_8_16_Normalfont[ZiNum][j++]);
			}
			for(i=0;i<8;i++)
			{
				LCDPutCh(x+i,y+1,~Number_8_16_Normalfont[ZiNum][j++]);
			}
		}
		else
		{
			for(i=0;i<8;i++)
			{
				LCDPutCh(x+i,y,Number_8_16_Normalfont[ZiNum][j++]);
			}
			for(i=0;i<8;i++)
			{
				LCDPutCh(x+i,y+1,Number_8_16_Normalfont[ZiNum][j++]);
			}
		}
	}
}

/*********************************************************************************************************
** Function name:       LCDPutAscii
** Descriptions:        ��(x,y)��ʼλ����ʾһ��(8*16)�����ַ�
** input parameters:    x:x���ꣻy:y���ꣻstr:ASC�룻Inverse:1---���ף�0---������
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void LCDPutAsciiBlodFont(unsigned char x,unsigned char y,uint8_t str,unsigned char Inverse)
{	
	uint16_t i,j=0;
	uint16_t ZiNum;
	
	for(i=0;i<NUMBERNO;i++)
	{
		if(str == NumberIndex[i])
		{
			ZiNum = i;
			break;
		}
	}
	if(i==NUMBERNO)
	{
		return;
	}
	else
	{		
		if(Inverse)
		{
			for(i=0;i<8;i++)
			{
				LCDPutCh(x+i,y,~Number_8_16_Boldfont[ZiNum][j++]);
			}
			for(i=0;i<8;i++)
			{
				LCDPutCh(x+i,y+1,~Number_8_16_Boldfont[ZiNum][j++]);
			}
		}
		else
		{
			for(i=0;i<8;i++)
			{
				LCDPutCh(x+i,y,Number_8_16_Boldfont[ZiNum][j++]);
			}
			for(i=0;i<8;i++)
			{
				LCDPutCh(x+i,y+1,Number_8_16_Boldfont[ZiNum][j++]);
			}
		}
	}
}

/*********************************************************************************************************
** Function name:       LCDPutAscii
** Descriptions:        ��(x,y)��ʼλ����ʾһ��(16*32)�ַ�
** input parameters:    x:x���ꣻy:y���ꣻstr:ASC�룻Inverse:1---���ף�0---������
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void LCDPutAscii16_32(unsigned char x,unsigned char y,uint8_t str,unsigned char Inverse)
{
	uint16_t i,j=0;
	uint16_t ZiNum;
	
	for(i=0;i<NUMBERNO;i++)
	{
		if(str == NumberIndex[i])
		{
			ZiNum = i;
			break;
		}
	}
	if(i==NUMBERNO)
	{
		return;
	}
	else
	{		
		if(Inverse)
		{
			for(i=0;i<16;i++)
			{
				LCDPutCh(x+i,y,~Number_16_32_Normalfont[ZiNum][j++]);
			}
			for(i=0;i<16;i++)
			{
				LCDPutCh(x+i,y+1,~Number_16_32_Normalfont[ZiNum][j++]);
			}
			for(i=0;i<16;i++)
			{
				LCDPutCh(x+i,y+2,~Number_16_32_Normalfont[ZiNum][j++]);
			}
			for(i=0;i<16;i++)
			{
				LCDPutCh(x+i,y+3,~Number_16_32_Normalfont[ZiNum][j++]);
			}
		}
		else
		{
			for(i=0;i<16;i++)
			{
				LCDPutCh(x+i,y,Number_16_32_Normalfont[ZiNum][j++]);
			}
			for(i=0;i<16;i++)
			{
				LCDPutCh(x+i,y+1,Number_16_32_Normalfont[ZiNum][j++]);
			}
			for(i=0;i<16;i++)
			{
				LCDPutCh(x+i,y+2,Number_16_32_Normalfont[ZiNum][j++]);
			}
			for(i=0;i<16;i++)
			{
				LCDPutCh(x+i,y+3,Number_16_32_Normalfont[ZiNum][j++]);
			}
		}
	}
}

/*********************************************************************************************************
** Function name:       LCDPutAscii
** Descriptions:        ��(x,y)��ʼλ����ʾһ��(16*32)�ַ�
** input parameters:    x:x���ꣻy:y���ꣻstr:ASC�룻Inverse:1---���ף�0---������
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void LCDPutAscii24_48(unsigned char x,unsigned char y,uint8_t str,unsigned char Inverse)
{
	uint16_t i,j=0;
	uint16_t ZiNum;
	
	for(i=0;i<NUMBERNO;i++)
	{
		if(str == NumberIndex[i])
		{
			ZiNum = i;
			break;
		}
	}
	if(i==NUMBERNO)
	{
		return;
	}
	else
	{		
		if(Inverse)
		{
			for(i=0;i<24;i++)
			{
				LCDPutCh(x+i,y,~Number_24_48_Normalfont[ZiNum][j++]);
			}
			for(i=0;i<24;i++)
			{
				LCDPutCh(x+i,y+1,~Number_24_48_Normalfont[ZiNum][j++]);
			}
			for(i=0;i<24;i++)
			{
				LCDPutCh(x+i,y+2,~Number_24_48_Normalfont[ZiNum][j++]);
			}
			for(i=0;i<24;i++)
			{
				LCDPutCh(x+i,y+3,~Number_24_48_Normalfont[ZiNum][j++]);
			}
			for(i=0;i<24;i++)
			{
				LCDPutCh(x+i,y+4,~Number_24_48_Normalfont[ZiNum][j++]);
			}
			for(i=0;i<24;i++)
			{
				LCDPutCh(x+i,y+5,~Number_24_48_Normalfont[ZiNum][j++]);
			}
		}
		else
		{
			for(i=0;i<24;i++)
			{
				LCDPutCh(x+i,y,Number_24_48_Normalfont[ZiNum][j++]);
			}
			for(i=0;i<24;i++)
			{
				LCDPutCh(x+i,y+1,Number_24_48_Normalfont[ZiNum][j++]);
			}
			for(i=0;i<24;i++)
			{
				LCDPutCh(x+i,y+2,Number_24_48_Normalfont[ZiNum][j++]);
			}
			for(i=0;i<24;i++)
			{
				LCDPutCh(x+i,y+3,Number_24_48_Normalfont[ZiNum][j++]);
			}
			for(i=0;i<24;i++)
			{
				LCDPutCh(x+i,y+4,Number_24_48_Normalfont[ZiNum][j++]);
			}
			for(i=0;i<24;i++)
			{
				LCDPutCh(x+i,y+5,Number_24_48_Normalfont[ZiNum][j++]);
			}
		}
	}
}


/*********************************************************************************************************
** Function name:       LCDPutChinese
** Descriptions:        ��(x,y)��ʼλ����ʾһ��16*16�����ַ�
** input parameters:    x:x���ꣻy:y���ꣻstr:�����ַ�;Inverse:�Ƿ񷴰�
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void LCDPutChinese(uint8_t x,uint8_t y,char *str,uint8_t Inverse)
{
	char *OffSet;
	unsigned char i;
	uint16_t AddOffSet;
	OffSet=strstr(IndexChinese16Dot,str); 
	AddOffSet=(OffSet-IndexChinese16Dot)<<4;
	if(Inverse)
	{
		for(i=0;i<16;i++)
		{
			LCDPutCh(x+i,y,~CHINESE16_TAB[AddOffSet++]);
		}
		for(i=0;i<16;i++)
		{
			LCDPutCh(x+i,y+1,~CHINESE16_TAB[AddOffSet++]);
		}
	}
	else
	{
		for(i=0;i<16;i++)
		{
			LCDPutCh(x+i,y,CHINESE16_TAB[AddOffSet++]);
		}
		for(i=0;i<16;i++)
		{
			LCDPutCh(x+i,y+1,CHINESE16_TAB[AddOffSet++]);
		}
	}		
}

/*********************************************************************************************************
** Function name:       LCDPutChinese32
** Descriptions:        ��(x,y)��ʼλ����ʾһ��16*16�����ַ�
** input parameters:    x:x���ꣻy:y���ꣻstr:�����ַ�;Inverse:�Ƿ񷴰�
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void LCDPutChinese32(uint8_t x,uint8_t y,char *str,uint8_t Inverse)
{
	char *OffSet;
	unsigned char i;
	uint16_t AddOffSet;
	OffSet=strstr(IndexChinese32Dot,str); 
	AddOffSet=(OffSet-IndexChinese32Dot)<<6;
	//Trace("\r\n lcd=%ld,%d,%s",AddOffSet,OffSet,str);
	if(Inverse)
	{
		for(i=0;i<32;i++)
		{
			LCDPutCh(x+i,y,~CHINESE32_TAB[AddOffSet++]);
		}
		for(i=0;i<32;i++)
		{
			LCDPutCh(x+i,y+1,~CHINESE32_TAB[AddOffSet++]);
		}
		for(i=0;i<32;i++)
		{
			LCDPutCh(x+i,y+2,~CHINESE32_TAB[AddOffSet++]);
		}
		for(i=0;i<32;i++)
		{
			LCDPutCh(x+i,y+3,~CHINESE32_TAB[AddOffSet++]);
		}
	}
	else
	{
		for(i=0;i<32;i++)
		{
			LCDPutCh(x+i,y,CHINESE32_TAB[AddOffSet++]);
		}
		for(i=0;i<32;i++)
		{
			LCDPutCh(x+i,y+1,CHINESE32_TAB[AddOffSet++]);
		}
		for(i=0;i<32;i++)
		{
			LCDPutCh(x+i,y+2,CHINESE32_TAB[AddOffSet++]);
		}
		for(i=0;i<32;i++)
		{
			LCDPutCh(x+i,y+3,CHINESE32_TAB[AddOffSet++]);
		}
	}		
}

/*********************************************************************************************************
** Function name:       LCDPutStr
** Descriptions:        ��(x,y)��ʼλ����ʾһ����Ӣ��16*16������ַ���
** input parameters:    x:x���ꣻy:y���ꣻstr:�ַ�����Inverse:�Ƿ񷴰ף�Language:����
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void LCDPutStr(uint8_t x,uint8_t y,char *str,uint8_t Inverse,uint8_t Language)
{
	char rega[2];
	char regc[3];
	while(*str != 0)
	{
		if((uint8_t)*str < 0xA0 )
		{
			strncpy(rega,str,1);
			rega[1] = 0;
			LCDPutAsciiFont(x,y,rega[0],Inverse);
			str = str+1;
			x = x+8;
		}
		if((uint8_t)*str >= 0xA0)
		{
			strncpy(regc,str,2);
			regc[2] = 0;
			LCDPutChinese(x,y,regc,Inverse);
			str = str+2;
			x = x+16;
		}
	}
}

/*********************************************************************************************************
** Function name:       LCDClearLineDraw
** Descriptions:        ��x��ʼ��һ������
** input parameters:    
                        font:����1��ͨ8*16,2�Ӵ�8*16,3��16*32,4��24*48;;
                       
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void LCDClearLineDraw(uint8_t x,unsigned char row, uint8_t font)
{
	unsigned char p;				//����ֲ�����

	switch(font)
	{
		case 1:
		case 2:	
			//�����������8*16
			for(p=x;p<LCDWIDTH-x;p+=7)		//дDIS_COL����ʾ����
			{
				LCDPutAscii(p,row,0,0);	
			}
			break;
		case 3:
			LCDSetCursor(x,row);
			//�����������
			for(p=x;p<LCDWIDTH-x;p++)
			{		
				UC1608EPutData(0x00);	
			}	
			LCDSetCursor(x,row+1);
			//�����������
			for(p=x;p<LCDWIDTH-x;p++)
			{		
				UC1608EPutData(0x00);	
			}	
			LCDSetCursor(x,row+2);
			//�����������
			for(p=x;p<LCDWIDTH-x;p++)
			{		
				UC1608EPutData(0x00);	
			}	
			LCDSetCursor(x,row+3);
			//�����������
			for(p=x;p<LCDWIDTH-x;p++)
			{		
				UC1608EPutData(0x00);	
			}	
			break;
		case 4:
			LCDSetCursor(x,row);
			//�����������
			for(p=x;p<LCDWIDTH-x;p++)
			{		
				UC1608EPutData(0x00);	
			}	
			LCDSetCursor(x,row+1);
			//�����������
			for(p=x;p<LCDWIDTH-x;p++)
			{		
				UC1608EPutData(0x00);	
			}	
			LCDSetCursor(x,row+2);
			//�����������
			for(p=x;p<LCDWIDTH-x;p++)
			{		
				UC1608EPutData(0x00);	
			}	
			LCDSetCursor(x,row+3);
			//�����������
			for(p=x;p<LCDWIDTH-x;p++)
			{		
				UC1608EPutData(0x00);	
			}	
			LCDSetCursor(x,row+4);
			//�����������
			for(p=x;p<LCDWIDTH-x;p++)
			{		
				UC1608EPutData(0x00);	
			}	
			LCDSetCursor(x,row+5);
			//�����������
			for(p=x;p<LCDWIDTH-x;p++)
			{		
				UC1608EPutData(0x00);	
			}	
			break;
	}	
}


/*********************************************************************************************************
** Function name:       LCDPrintf
** Descriptions:        ��ʾһ����Ӣ�Ļ���ַ�����ز���
** input parameters:    x:x���ꣻy:y���ꣻInverse:1---���ף�0---������;format:��ʽ��;...:�������
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void LCDPrintf(uint8_t x,uint8_t y,uint8_t Inverse,uint8_t Language,char *format,...)
{
	va_list arg_ptr;
	char rega[2];			//ASCII
	char regc[3];			//CHINESE
	char str[30];
	unsigned char Index = 0;
	uint16_t len;
	va_start(arg_ptr,format);
	len = vsprintf((char *)str,(const char *)format,arg_ptr);
	va_end(arg_ptr);
	LCDClearLineDraw(x,y,1);	
	
	for(Index=0;Index<len;)
	{
		if(str[Index] < 0xA0 )
		{
			rega[0] = str[Index++];
			rega[1] = '\0';
			LCDPutAsciiFont(x,y,rega[0],Inverse);
			x = x+8;
		}
		if(str[Index] >= 0xA0)
		{
			regc[0] = str[Index++];
			regc[1] = str[Index++];
			regc[2] = '\0';
			LCDPutChinese(x,y,regc,Inverse);
			x = x+16;
		}
	}
}

/*********************************************************************************************************
** Function name:       LCDPrintf32
** Descriptions:        ��ʾһ����Ӣ�Ļ���ַ�����ز���
** input parameters:    x:x���ꣻy:y���ꣻInverse:1---���ף�0---������;format:��ʽ��;...:�������
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void LCDPrintf32(uint8_t x,uint8_t y,uint8_t Inverse,uint8_t Language,char *format,...)
{
	va_list arg_ptr;
	char rega[2];			//ASCII
	char regc[3];			//CHINESE
	char str[30];
	unsigned char Index = 0;
	uint16_t len;
	va_start(arg_ptr,format);
	len = vsprintf((char *)str,(const char *)format,arg_ptr);
	va_end(arg_ptr);
	LCDClearLineDraw(x,y,1);	
	
	for(Index=0;Index<len;)
	{
		if(str[Index] < 0xA0 )
		{
			rega[0] = str[Index++];
			rega[1] = '\0';
			LCDPutAscii16_32(x,y,rega[0],Inverse);
			x = x+16;
		}
		if(str[Index] >= 0xA0)
		{
			regc[0] = str[Index++];
			regc[1] = str[Index++];
			regc[2] = '\0';
			LCDPutChinese32(x,y,regc,Inverse);
			x = x+32;
		}
	}
}


/*********************************************************************************************************
** Function name:       LCDPrintf
** Descriptions:        ��ʾһ�и�������������ַ�����ز���
** input parameters:    x:x���ꣻy:y���ꣻfont:����1��ͨ8*16,2�Ӵ�8*16,3��16*32,4��24*48;format:��ʽ��;...:�������
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void LCDNumberFontPrintf(uint8_t x,uint8_t y,uint8_t font,char *format,...)
{
	va_list arg_ptr;
	char rega[2];			//ASCII
	//char regc[3];			//CHINESE
	char str[30];
	unsigned char Index = 0;
	uint16_t len;
	va_start(arg_ptr,format);
	len = vsprintf((char *)str,(const char *)format,arg_ptr);
	va_end(arg_ptr);
	LCDClearLineDraw(x,y,font);	
	
	for(Index=0;Index<len;)
	{
		if(str[Index] < 0xA0 )
		{
			rega[0] = str[Index++];
			rega[1] = '\0';
			switch(font)
			{
				case 1:
					LCDPutAsciiFont(x,y,rega[0],0);
					x = x+8;                               //��һ���ַ��е�ַ
					break;
				case 2:
					LCDPutAsciiBlodFont(x,y,rega[0],0);
					//x=x+8+8/4;
					x = x+8; 
					/*
					//��һ���ַ��е�ַ		
					if(rega[0]=='1')
					{
						x=x-3;
					}
					*/
					break;
				case 3:
					LCDPutAscii16_32(x,y,rega[0],0);
					//x=x+16+16/4;								//��һ���ַ��е�ַ
					x=x+16;
					/*
					if(rega[0]=='1')
					{
						x=x-7;
					}
					*/
					break;
				case 4:	
					/*
					if( (rega[0]=='.')||(rega[0]==':') )
					{
						x=x-24/4;
						LCDPutAscii24_48(x,y,rega[0],0);	
						x=x+24;								//��һ���ַ��е�ַ
					}
					else
					{
						LCDPutAscii24_48(x,y,rega[0],0);	
						x=x+24+24/4;								//��һ���ַ��е�ַ
						if(rega[0]=='1')
						{
							x=x-11;
						}
					}
					*/
					LCDPutAscii24_48(x,y,rega[0],0);	
					//x=x+24+24/4;								//��һ���ַ��е�ַ
					x=x+24;
					break;
			}	
		}		
	}
}

/*********************************************************************************************************
** Function name:       LCDSpecialPicPrintf
** Descriptions:        ��ʾ����ͼ��
** input parameters:    x:x���ꣻy:y���ꣻmode:0-�����Y 1---�Ҽ�ͷ2---���ͷ;3---���;4---��̾��;
                        font-1��ʾ16*16,-2��ʾ32*32
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void LCDSpecialPicPrintf(uint8_t x,uint8_t y,uint8_t mode,uint8_t font)
{	
	switch(font)
	{
		case 1:
			LCDClearLineDraw(x,y,1);	
			
			LCDPutBmp(x,y,16,16,SpecialPic_16_16_font[mode],0);
			break;
		case 2:
			LCDClearLineDraw(x,y,3);	
			
			LCDPutBmp(x,y,32,32,SpecialPic_32_32_font[mode],0);
			break;
	}	
	/*
	switch(mode)
	{
		case 1:
			LCDPutBmp(x,y,16,16,SpecialPic_16_16_font,0);
			break;
		case 2:
			LCDPutBmp(x,y,155,40,SpecialPic_16_16_font,0);			
			break;
		case 3:
			LCDPutBmp(x,y,155,40,SpecialPic_16_16_font,0);				
			break;
	}*/	
}


/*********************************************************************************************************
** Function name:       LCDPutBmp
** Descriptions:        ��(x,y)��ʼλ����ʾһ��λͼ
** input parameters:    dat:����
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void LCDPutBmp(uint8_t x,uint8_t y,const uint8_t W,const uint8_t H,const uint8_t *data,uint8_t Inverse)
{
	unsigned char i;
	unsigned char Row;
	if(Inverse)
	{
		for(Row=0;Row<(H/8);Row++)
		{
			for(i=0;i<W;i++)
			{
				LCDPutCh(x+i,y,~*data);
				data++;
			}
			y++;
		}	
	}
	else
	{
		for(Row=0;Row<(H/8);Row++)
		{
			for(i=0;i<W;i++)
			{
				LCDPutCh(x+i,y,*data);
				data++;
			}
			y++;
		}	
	}		
}
/*********************************************************************************************************
** Function name:       LCDDrawRectangle
** Descriptions:        ��x,y��ʾһ����
** input parameters:    dat:����
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void LCDDrawLine(unsigned char x,unsigned char y)
{
	unsigned char i;
	LCDSetCursor(x,y);
	for(i=0;i<(LCDWIDTH-x);i++)
	{		
		UC1608EPutData(0x01);	
	}	
}




void LCD_drawDot(unsigned char x,unsigned char y)
{
	unsigned char k,m;
	k = y/8;
	LCDSetCursor(x,k);
	y = y % 8;
	m = 1;
	y = m << y;
	m = m | y;
	UC1608EPutCmd(m);    
}

/*********************************************************************************************************
** Function name:       LCDDrawRectangle
** Descriptions:        ��(x1,y1),(x2,y2)��ʾһ������
** input parameters:    dat:����
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void LCDDrawRectangle(unsigned char x1,unsigned char y1,unsigned char x2,unsigned char y2)
{
	unsigned char i;
	for(i=0;i<=(x2-x1);i++)
	{
		LCDSetCursor(x1+i,y1);
		UC1608EPutData(0x01);
		LCDSetCursor(x1+i,y2);
		UC1608EPutData(0x80);	
	}
	for(i=0;i<=(y2-y1);i++)
	{
		LCDSetCursor(x1,y1+i);
		UC1608EPutData(0xFF);
		LCDSetCursor(x2,y2-i);
		UC1608EPutData(0xFF);	
	}	
}

/*********************************************************************************************************
** Function name:       LCDPutLOGOBmp
** Descriptions:        ��position��y������ʾlogo
** input parameters:    mode=1ʹ���ĸ�logo����1����easivend��2����pingtu��3����Tuerqi,4-Malaysia
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void LCDPutLOGOBmp(unsigned char x,unsigned char y,unsigned char mode)
{
	//unsigned int i,j,ch_x,ch_y,row,col,pituPosition;
	//unsigned int xi;					//ƫ�������ַ����ٵĿ��Զ���Ϊunsigned char 
	//uint8_t str[775]={0};
	//uint8_t *str[775]={0};	
	
	
	//if(mode==0)return;
	//LCDPutBmp(y,position,155,40,EasivendLOGO,0);
	switch(mode)
	{
		case 1:
			LCDPutBmp(x,y,192,96,EasivendLOGO192_96,0);
			break;
		case 2:
			LCDPutBmp(x,y,155,40,PingtuLOGO,0);			
			break;
		case 3:
			LCDPutBmp(x,y,155,40,TuerqiLOGO,0);				
			break;
		case 4:
			LCDPutBmp(x,y,155,40,MalaysiaLOGO,0);				
			break;	
	}		
	/*
	//GetLOGO(mode,str);
	//str = &EasivendLOGO[0];
	ch_x=5;
	ch_y=155;
	xi=0;
	//8X16�ַ����ְ�������,����
	for(i=0;i<ch_x;i++) 				//дch_x��
	{
		LCDSetCursor(col,row);
		//Trace("\r\n newline=%d",row);
		for(j=0;j<ch_y;j++) 			//дch_y��
		{
			switch(mode)
			{
				case 1:
					UC1608EPutData(EasivendLOGO[xi]);	
					break;
				case 2:
					UC1608EPutData(PingtuLOGO[xi]);	
					break;
				case 3:
					UC1608EPutData(TuerqiLOGO[xi]);	
					break;
			}			 			
			//Pitu_LCDPutData(str[xi]);			//д���һ�ֽ�	
			//Trace("%#02x,",str[xi]);
			xi++;
			col++;			
		}								//�ϰ���ַ��������	
		row++;						//ָ���°���ַ���
		col=y;	
	}
	*/
}

/*******************************************End Of API***************************************************/

/******************************************����ΪӲ������************************************************/
/*********************************************************************************************************
** Function name:       LCDPutCh
** Descriptions:        ��LCD�ͳ�һ���ֽڵ���ʾ����
** input parameters:    X:x���ꣻY:y���ꣻCh:Ҫд������
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void LCDPutCh(unsigned char X,unsigned char Y,unsigned char Ch)
{
	LCDSetCursor(X,Y);
	UC1608EPutData(Ch);		
}


/*********************************************************************************************************
** Function name:       UC1608EPutByte
** Descriptions:        ��ģ��SPI������LCM����һ���ֽڵ�����
** input parameters:    dat:Ҫ���͵����ݣ�����������Ҳ��������ʾ����
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void UC1608EPutByte(unsigned char dat)
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		SCL_CLR(); 
		if(( dat & 0x80 ) == 0)
		{
			SDA_CLR();
		}
		else
		{
			SDA_SET();
		} 
		SCL_SET(); 
		dat=dat<<1;  
	}
}
/*********************************************************************************************************
** Function name:       UC1608EPutCmd
** Descriptions:        ��LCMд��һ���ֽڵ�����
** input parameters:    cmd:����
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void UC1608EPutCmd(unsigned char cmd) 
{
	nCS_SET();
	CD_CLR();
	UC1608EPutByte(cmd);
	CD_SET();
	nCS_CLR();
}
/*********************************************************************************************************
** Function name:       UC1608EPutData
** Descriptions:        ��LCMд��һ���ֽڵ�����
** input parameters:    dat:����
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void UC1608EPutData(unsigned char dat) 
{
	nCS_SET();
	CD_SET();
	UC1608EPutByte(dat);
	CD_CLR();
	nCS_CLR();
}
/**************************************End Of File*******************************************************/
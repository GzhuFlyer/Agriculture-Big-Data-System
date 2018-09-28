/****************************************************************************
*��������main
*��  �룺void
*��  ����void
*��  �ܣ���ȡ���գ�����ˮ�֣������¶�ʪ����ֵ
*		 ����ͨ��ESP8266Wifiģ�齫���ݷ��͸�������
****************************************************************************/


#include "reg52.h"
#include "string.h"
#include "dht11.h"
#include "XPT2046.h"

sbit led1  = P2^0;
sbit led2 = P2^1;
sbit led3 = P2^2;
sbit led4 = P2^3;
sbit led5 = P2^4;
sbit led6 = P2^5;
sbit led7 = P2^6;
sbit led8 = P2^7;

typedef unsigned char u8;
u8 WifiRcv[10];	     //���ڽ��ջ�����
u8 WifiRcvAt[10];    //���洮�ڽ��յ�������
u8 WifiRcvCount = 0; //���ڽ��ռ���
u8 WifiAtFlag = 0;   //����Atָ�������ɱ�־

U8  str[5]={"RS23"};
U8  U8T_data_H,U8T_data_L,U8RH_data_H,U8RH_data_L,U8checkdata;
U8 temp[20];	//ʵ�ʷ��͵��ַ�����һ��'\0'��Ϊ���ͽ�����־���ܹ�20��
U8 temp1[10];	//temp1�ܹ���Ϊ����


void UsartInit(void)
{
	SCON=0x50;   //����Ϊ������ʽ1
	TMOD=0X20;	 //���ü�����������ʽ2
	PCON=0X80;	 //�����ʼӱ�
	TH1=0XF3;	 //��������ʼֵ����
	TL1=0XF3;	 //����������Ϊ4800
	ES=1;	     //�򿪽����ж�
	EA=1;	     //�������ж�
	TR1=1;	     //�򿪼�����
}

void delay1s(u8 sec)   //ʹ�÷������ʱ��Ϊ1.0023s
{
	u8 t1,t2,t3,t4;
	for(t3=sec;t3>0;t3--)
		for(t1=0;t1<192;t1++)
			for(t2=0;t2<149;t2++)
				for(t4=0;t4<10;t4++);
}

void UartRecv() interrupt 4
{
	u8 i = 0;
	if(RI==1)
	{	
		RI=0;						//�����ж��ź����㣬����������
		WifiRcv[WifiRcvCount]=SBUF; //����һ���ֽڵĽ���
		//***�����ſ��ƽ�ˮ��ֹͣ��ˮ��ʩ�ʣ�ֹͣʩ��***//
		if(WifiRcv[WifiRcvCount] == 'z') led4=0;  //���жϺ�����ʹ�ÿ�������
 		if(WifiRcv[WifiRcvCount] == 'Z') led4=1;  //ʹ�õ���û�б����յ�����ĸ
		if(WifiRcv[WifiRcvCount] == 'j') led5=0;
		if(WifiRcv[WifiRcvCount] == 'J') led5=1;
		WifiRcvCount++;				//�ַ����Ľ����ֽ�������
				//������ATָ������0x0d 0x0a��β������Ƿ��յ�������ATָ��
		if(WifiRcvCount >= 2 && WifiRcv[WifiRcvCount-1]==0x0a && WifiRcv[WifiRcvCount-2]==0x0d )
		{
			//һ���յ�0x0a 0x0d ,�ͽ����ݱ��������������û����������ж�
			for(i=0; i<WifiRcvCount; i++)
			{
				WifiRcvAt[i] = WifiRcv[i];  //���и�ֵ
				WifiRcv[i] = 0;	 //�����ջ������������㴦��
			}
			WifiRcvCount = 0;  //����ֵ�������㴦��
			WifiAtFlag = 1;    //�յ�������Atָ��ʱ��������һ����


		}else if(WifiRcvCount >= 10)  
		{
			//��ֹ���յ������ݳ������õĴ洢���򣬽���������������֮ǰ���յ�����
			WifiRcvCount = 0;
		}
	}


}
  
void UartSends(u8 *str)
{
	ES = 0;				  //����������жϣ���ֹ����
	while(*str != '\0')  //��鷢�͵��ַ�����β'\0'
	{
	
		SBUF=*str;		 //�Է��͵��ַ���������з���
		while(!TI);      //�ȴ���������źţ�TI=1������
		TI=0;		  	 //�������
		str++;			 //������һ���ַ����ķ���
	}
	ES = 1;				 //�ָ������ж�
}



void main(void)
{	
	unsigned char i = 0;
	while(1) {
		UsartInit();	 //�Դ��ڽ��г�ʼ������
		delay1s(5);	   	 //��ʱ2��

		UartSends("AT+CIPMUX=1\r\n"); 	//����ģ�������
		delay1s(4);
		if(strstr(WifiRcvAt,"OK"))  
		{					          
			led1 = 0;			//�źŵ�1��ָʾ�����Ϸ�����
			showHumTemp();  //�ȳ�ʼ��DHT11����������������Ҫ����2~3����ܳ�ʼ�����
			UartSends("AT+CIPSTART=4,\"TCP\",\"119.23.241.38\",1234\r\n");
			delay1s(4);					  //����Զ�� TCP ������	

		    while(1)
			{
				UartSends("AT+CIPSEND=4,43");
				UartSends("\r\n");

				//��ʼ��������DHT11��������ֵ
	  			showHumTemp();
				delay1s(1);
				UartSends(temp);
				UartSends("\r\n");
				//delay1s(2);

				//��ʼ�������͹�紫������ֵ
				showIllumination();
				UartSends(temp1);
				UartSends("\r\n");
				//delay1s(2);

				//��ʼ������������ˮ�ִ�������ֵ
				showSoilMoisture();
				UartSends(temp1);
				UartSends("\r\n");
				led2 = 0;		 //ָʾ��2����ʾ���ݷ������
				delay1s(5);	

				//���ݷ��͹���������ͷ������Ͽ����ӣ�������ѭ����ֹͣ���ݷ���
				//�ȴ���һ������
				if(strstr(WifiRcvAt,"ERROR"))	//���ݷ��ͳ������յ�Wifiģ�鷢����
				{								//ERROR�ַ���ָʾ����
					led3 = 0;					//ָʾ��3����ʾ���ݷ��͹����г��ִ���
					break;
				}		
			}
			
		}
		//�Ͽ��ͷ�����������
		UartSends("AT+CIPCLOSE=4\r\n");
		delay1s(2);
		led1 = 1;
		led2 = 1;
		led3 = 1;	
	}	 
}
	



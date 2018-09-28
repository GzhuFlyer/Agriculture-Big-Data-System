/****************************************************************************
*函数名：main
*输  入：void
*输  出：void
*功  能：获取光照，土壤水分，空气温度湿度数值
*		 并且通过ESP8266Wifi模块将数据发送给服务器
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
u8 WifiRcv[10];	     //串口接收缓存区
u8 WifiRcvAt[10];    //保存串口接收到的数据
u8 WifiRcvCount = 0; //串口接收计数
u8 WifiAtFlag = 0;   //完整At指令接收完成标志

U8  str[5]={"RS23"};
U8  U8T_data_H,U8T_data_L,U8RH_data_H,U8RH_data_L,U8checkdata;
U8 temp[20];	//实际发送的字符加上一个'\0'作为发送结束标志，总共20个
U8 temp1[10];	//temp1总共作为复用


void UsartInit(void)
{
	SCON=0x50;   //设置为工作方式1
	TMOD=0X20;	 //设置计数器工作方式2
	PCON=0X80;	 //波特率加倍
	TH1=0XF3;	 //计数器初始值设置
	TL1=0XF3;	 //比特率设置为4800
	ES=1;	     //打开接收中断
	EA=1;	     //代开总中断
	TR1=1;	     //打开计数器
}

void delay1s(u8 sec)   //使用仿真调试时间为1.0023s
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
		RI=0;						//接收中断信号清零，将继续接收
		WifiRcv[WifiRcvCount]=SBUF; //进行一个字节的接收
		//***代表着控制浇水，停止浇水，施肥，停止施肥***//
		if(WifiRcv[WifiRcvCount] == 'z') led4=0;  //在中断函数中使用控制命令
 		if(WifiRcv[WifiRcvCount] == 'Z') led4=1;  //使用的是没有被接收到的字母
		if(WifiRcv[WifiRcvCount] == 'j') led5=0;
		if(WifiRcv[WifiRcvCount] == 'J') led5=1;
		WifiRcvCount++;				//字符串的接收字节数增加
				//完整的AT指令是以0x0d 0x0a结尾，检测是否收到完整的AT指令
		if(WifiRcvCount >= 2 && WifiRcv[WifiRcvCount-1]==0x0a && WifiRcv[WifiRcvCount-2]==0x0d )
		{
			//一旦收到0x0a 0x0d ,就将数据保存起来。用于用户主函数的判断
			for(i=0; i<WifiRcvCount; i++)
			{
				WifiRcvAt[i] = WifiRcv[i];  //进行赋值
				WifiRcv[i] = 0;	 //将接收缓冲区进行清零处理
			}
			WifiRcvCount = 0;  //计数值进行置零处理
			WifiAtFlag = 1;    //收到完整的At指令时，进行置一操作


		}else if(WifiRcvCount >= 10)  
		{
			//防止接收到的数据超过设置的存储区域，进行置零操作，清除之前接收的数据
			WifiRcvCount = 0;
		}
	}


}
  
void UartSends(u8 *str)
{
	ES = 0;				  //不允许产生中断，防止干扰
	while(*str != '\0')  //检查发送的字符串结尾'\0'
	{
	
		SBUF=*str;		 //对发送的字符串逐个进行发送
		while(!TI);      //等待发送完成信号（TI=1）出现
		TI=0;		  	 //置零操作
		str++;			 //继续下一个字符串的发送
	}
	ES = 1;				 //恢复允许中断
}



void main(void)
{	
	unsigned char i = 0;
	while(1) {
		UsartInit();	 //对串口进行初始化操作
		delay1s(5);	   	 //延时2秒

		UartSends("AT+CIPMUX=1\r\n"); 	//启用模块多连接
		delay1s(4);
		if(strstr(WifiRcvAt,"OK"))  
		{					          
			led1 = 0;			//信号灯1，指示连接上服务器
			showHumTemp();  //先初始化DHT11传感器，传感器需要花费2~3秒才能初始化完成
			UartSends("AT+CIPSTART=4,\"TCP\",\"119.23.241.38\",1234\r\n");
			delay1s(4);					  //连上远程 TCP 服务器	

		    while(1)
			{
				UartSends("AT+CIPSEND=4,43");
				UartSends("\r\n");

				//初始化并发送DHT11传感器数值
	  			showHumTemp();
				delay1s(1);
				UartSends(temp);
				UartSends("\r\n");
				//delay1s(2);

				//初始化并发送光电传感器数值
				showIllumination();
				UartSends(temp1);
				UartSends("\r\n");
				//delay1s(2);

				//初始化并发送土壤水分传感器数值
				showSoilMoisture();
				UartSends(temp1);
				UartSends("\r\n");
				led2 = 0;		 //指示灯2，表示数据发送完成
				delay1s(5);	

				//数据发送过程中如果和服务器断开连接，则跳出循环，停止数据发送
				//等待下一次连接
				if(strstr(WifiRcvAt,"ERROR"))	//数据发送出错会接收到Wifi模块发来的
				{								//ERROR字符，指示出错
					led3 = 0;					//指示灯3，表示数据发送过程中出现错误
					break;
				}		
			}
			
		}
		//断开和服务器的连接
		UartSends("AT+CIPCLOSE=4\r\n");
		delay1s(2);
		led1 = 1;
		led2 = 1;
		led3 = 1;	
	}	 
}
	



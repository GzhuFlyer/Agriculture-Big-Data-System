#include <reg51.h>
#include <intrins.h> 
#include "DHT11.h"

#define uchar unsigned char
#define uint unsigned int
#define   Data_0_time    4

//----------------------------------------------//
//----------------IO�ڶ�����--------------------//
//----------------------------------------------//
sbit  P2_0  = P1^0 ;

//----------------------------------------------//
//----------------������--------------------//
//----------------------------------------------//
U8  U8FLAG,k;
U8  U8count,U8temp;
extern U8  U8T_data_H,U8T_data_L,U8RH_data_H,U8RH_data_L,U8checkdata;
U8  U8T_data_H_temp,U8T_data_L_temp,U8RH_data_H_temp,U8RH_data_L_temp,U8checkdata_temp;
U8  U8comdata;
U8  outdata[5];  //���巢�͵��ֽ���	   
U8  indata[5];
U8  count, count_r=0;
extern U8  str[5];
extern U8 temp[20];
U16 U16temp1,U16temp2;

       void Delay(U16 j)
    {      U8 i;
	    for(;j>0;j--)
	  { 	
		for(i=0;i<27;i++);

	  }
    }
       void  Delay_10us(void)
      {
        U8 i;
        i--;
        i--;
        i--;
        i--;
        i--;
        i--;
       }
	
	
void  COM(void)
{
     
	U8 i;
          
    for(i=0;i<8;i++)	   
	    {
		
	   	    U8FLAG=2;	
			while((!P2_0)&&U8FLAG++);
			Delay_10us();
			Delay_10us();
			Delay_10us();
			U8temp=0;
			if(P2_0)U8temp=1;
			U8FLAG=2;
			while((P2_0)&&U8FLAG++);
			//��ʱ������forѭ��		  
			if(U8FLAG==1)break;
			//�ж�����λ��0����1	 	   	   
			// ����ߵ�ƽ�߹�Ԥ��0�ߵ�ƽֵ������λΪ 1 	   	 
			U8comdata<<=1;
			U8comdata|=U8temp;        //0
	     }//rof
	   
	}

	//--------------------------------
	//-----ʪ�ȶ�ȡ�ӳ��� ------------
	//--------------------------------
	//----���±�����Ϊȫ�ֱ���--------
	//----�¶ȸ�8λ== U8T_data_H------
	//----�¶ȵ�8λ== U8T_data_L------
	//----ʪ�ȸ�8λ== U8RH_data_H-----
	//----ʪ�ȵ�8λ== U8RH_data_L-----
	//----У�� 8λ == U8checkdata-----
	//----��������ӳ�������----------
	//---- Delay();, Delay_10us();,COM(); 
	//--------------------------------

	void RH(void)
	{
	  //��������18ms 
       P2_0=0;
	   Delay(180);
	   P2_0=1;
	 //������������������ ������ʱ20us
	   Delay_10us();
	   Delay_10us();
	   Delay_10us();
	   Delay_10us();
	 //������Ϊ���� �жϴӻ���Ӧ�ź� 
	   P2_0=1;
	 //�жϴӻ��Ƿ��е͵�ƽ��Ӧ�ź� �粻��Ӧ����������Ӧ����������	  
	   if(!P2_0)		 //T !	  
	   {
			U8FLAG=2;
			//�жϴӻ��Ƿ񷢳� 80us �ĵ͵�ƽ��Ӧ�ź��Ƿ����	 
			while((!P2_0)&&U8FLAG++);
			U8FLAG=2;
			//�жϴӻ��Ƿ񷢳� 80us �ĸߵ�ƽ���緢����������ݽ���״̬
			while((P2_0)&&U8FLAG++);
			//���ݽ���״̬		 
			COM();
			U8RH_data_H_temp=U8comdata;
			COM();
			U8RH_data_L_temp=U8comdata;
			COM();
			U8T_data_H_temp=U8comdata;
			COM();
			U8T_data_L_temp=U8comdata;
			COM();
			U8checkdata_temp=U8comdata;
			P2_0=1;
			//����У�� 
	 
	   U8temp=(U8T_data_H_temp+U8T_data_L_temp+U8RH_data_H_temp+U8RH_data_L_temp);
	   if(U8temp==U8checkdata_temp)
	   {
	   	  U8RH_data_H=U8RH_data_H_temp;
	   	  U8RH_data_L=U8RH_data_L_temp;
		  U8T_data_H=U8T_data_H_temp;
	   	  U8T_data_L=U8T_data_L_temp;
	   	  U8checkdata=U8checkdata_temp;
	   }//fi
	   }//fi

	}

void showHumTemp(void)
{
	
    	RH();	
    	str[0]=U8RH_data_H;										              
	    str[1]=U8RH_data_L;
	    str[2]=U8T_data_H;
	    str[3]=U8T_data_L;
	   

		temp[0]='H';
		temp[1]='U';
		temp[2]='M';
		temp[3]=':';
 		temp[4]=(((str[0]/16)*16)+str[0]%16)/10+'0';
		temp[5]=(((str[0]/16)*16)+str[0]%16)%10+'0';
		temp[6]='.';
		temp[7]=(((str[1]/16)*16)+str[1]%16)/10+'0';
		temp[8]=(((str[1]/16)*16)+str[1]%16)%10+'0';

		temp[9]=' ';

		temp[10]='T';
		temp[11]='E';
		temp[12]='M';
		temp[13]='P';

  		temp[14]=(((str[2]/16)*16)+str[2]%16)/10+'0';
		temp[15]=(((str[2]/16)*16)+str[2]%16)%10+'0';
		temp[16]='.';
		temp[17]=(((str[3]/16)*16)+str[3]%16)/10+'0';
		temp[18]=(((str[3]/16)*16)+str[3]%16)%10+'0';
		temp[19]='\0';


}


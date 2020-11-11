/*******************************************************************************  
* 文件名称：蓝桥杯单片机第十届决赛试题视频讲解对应程序     
* 程序说明：如需了解有关本程序配套视频讲解，及比赛经验分享\
*					 欢迎访问淘宝店铺网址：shop117015787.taobao.com
* 日期版本：2020-03
*******************************************************************************/

#include<STC15F2K60S2.H>
#include <intrins.h>
#include <IIC.H>
#include <ONEWIRE.H>
#include "stdio.h"

#define somenop {_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_(); _nop_();}
sbit TX = P1^0;  //发射引脚
sbit RX = P1^1;  //接收引脚

uchar code tab[]={0XC0,0XF9,0XA4,0XB0,0X99,0X92,0X82,0XF8,0X80,0X90,\
									0X40,0X79,0X24,0X30,0X19,0X12,0X02,0X78,0X00,0X10,\
									0XBF,0XFF,0XC6,0XC7,0XC8,0X8C};//"-"  "灭"  "C"  "L"  "N"  "P"  ""
unsigned char Digbuf[8] = {10,11,12,13,14,15,16,17};
uchar Digcom=0;

unsigned int intr = 0;
unsigned int t = 0;
unsigned int distance;
unsigned char Length=0;
unsigned char Ultrasonic_Start = 0 ;

long Temper=0;
unsigned char Temp_Start = 0;

unsigned char Interface = 0 ;
unsigned char Switch = 0 ;

unsigned int Change_num = 0 ;
unsigned char Change_num1,Change_num2,Change_num3;
unsigned char Temper_Para = 30,Ultrasonic_Para = 35;
unsigned char Temper_ParaLast = 0,Ultrasonic_ParaLast = 0;

unsigned char AD_start = 1;

unsigned char LED_Bit = 0XFF;

unsigned char Tx_buf[11];
unsigned char Rx_buf[8];
unsigned char Receive_num = 0;
unsigned char Receive_Flag = 0,Receive_Over = 0;

unsigned char Temper1=0,Temper2=0;
unsigned char S12=0,S13 =0;

unsigned int Key_time = 0 ;

void delayms(int ms);
void allinit(void);
void keyscan(void);

void Timer2Init(void);
void send_wave(void);
void Timer0Init(void);
unsigned int Ultrasonic(void);
void UartInit(void);
void uart_tx(unsigned char *p,unsigned char length);

void main(void)
{
	allinit();
	P2|=0X80;P2&=0X9F;LED_Bit&=0XFB;P0=LED_Bit;//AD开机启动 点亮L3
	Change_num1=EEPROM_read(0x00);delayms(5);
	Change_num2=EEPROM_read(0x01);delayms(5);
	Change_num3=EEPROM_read(0x02);delayms(5);
	Change_num=Change_num1*10000+Change_num2*100+Change_num3;
	
	Timer2Init();
	Timer0Init();
	UartInit();
	ES=1;
	while(1)
	{
		
		if(Temp_Start==1)
		{
			Temp_Start=0;
			Temper=Tempget();
			if(Temper/10000>Temper_Para){P2|=0X80;P2&=0X9F;LED_Bit&=0XFE;P0=LED_Bit;}
			else {P2|=0X80;P2&=0X9F;LED_Bit|=0X01;P0=LED_Bit;}
		}
		if(Ultrasonic_Start==1)
		{
			Ultrasonic_Start=0;
			Length=Ultrasonic();		
			if(AD_start == 1)
			{
				if(Length<=Ultrasonic_Para){AD_write(102);P2|=0X80;P2&=0X9F;LED_Bit&=0XFD;P0=LED_Bit;}
				else if(Length>Ultrasonic_Para){AD_write(204);P2|=0X80;P2&=0X9F;LED_Bit|=0X02;P0=LED_Bit;}
			}
		}
		
		if(Interface==0)
		{
			if(Switch==0)
			{
				Digbuf[0]=22;Digbuf[1]=21;Digbuf[2]=21;Digbuf[3]=21;
				Digbuf[4]=Temper/100000;Digbuf[5]=Temper%100000/10000+10;Digbuf[6]=Temper%10000/1000;Digbuf[7]=Temper%1000/100;						
			}
			else if(Switch==1)
			{
				Digbuf[0]=23;Digbuf[1]=21;Digbuf[2]=21;Digbuf[3]=21;Digbuf[4]=21;Digbuf[5]=21;
				Digbuf[6]=Length%100/10;Digbuf[7]=Length%10;		
			}
			else if(Switch==2)
			{
				Digbuf[0]=24;Digbuf[1]=21;Digbuf[2]=21;
				if(Change_num>=10000){Digbuf[3]=Change_num/10000;Digbuf[4]=Change_num%10000/1000;Digbuf[5]=Change_num%1000/100;Digbuf[6]=Change_num%100/10;Digbuf[7]=Change_num%10;}
				else if(Change_num>=1000){Digbuf[3]=21;Digbuf[4]=Change_num/1000;Digbuf[5]=Change_num%1000/100;Digbuf[6]=Change_num%100/10;Digbuf[7]=Change_num%10;}
				else if(Change_num>=100){Digbuf[3]=21;Digbuf[4]=21;Digbuf[5]=Change_num/100;Digbuf[6]=Change_num%100/10;Digbuf[7]=Change_num%10;}
				else if(Change_num>=10){Digbuf[3]=21;Digbuf[4]=21;Digbuf[5]=21;Digbuf[6]=Change_num/10;Digbuf[7]=Change_num%10;}
				else {Digbuf[3]=21;Digbuf[4]=21;Digbuf[5]=21;Digbuf[6]=21;Digbuf[7]=Change_num%10;}								
			}
		}
		else if(Interface==1)
		{
			if(Switch==0)
			{
				Digbuf[0]=25;Digbuf[1]=21;Digbuf[2]=21;Digbuf[3]=1;Digbuf[4]=21;Digbuf[5]=21;
				Digbuf[6]=Temper_Para/10;Digbuf[7]=Temper_Para%10;	
			}
			else if(Switch==1)
			{
				Digbuf[0]=25;Digbuf[1]=21;Digbuf[2]=21;Digbuf[3]=2;Digbuf[4]=21;Digbuf[5]=21;
				Digbuf[6]=Ultrasonic_Para/10;Digbuf[7]=Ultrasonic_Para%10;		
			}
		}
		
		if(Receive_Over==1)
		{
			Receive_Over=0;
			Receive_num=0;
			if((Rx_buf[0]=='S')&(Rx_buf[1]=='T')&(Rx_buf[2]=='\r')&(Rx_buf[3]=='\n'))
			{
				Temper1=Temper/100000*10+Temper%100000/10000;
				Temper2=Temper%10000/1000*10+Temper%1000/100;
				sprintf((char*)Tx_buf,"$%0.2bd,%0.2bd.%0.2bd\r\n",Length,Temper1,Temper2);
				uart_tx(Tx_buf,11);
			}	
			else if((Rx_buf[0]=='P')&(Rx_buf[1]=='A')&(Rx_buf[2]=='R')&(Rx_buf[3]=='A')&(Rx_buf[4]=='\r')&(Rx_buf[5]=='\n'))
			{
				sprintf((char*)Tx_buf,"#%0.2bd,%0.2bd\r\n",Ultrasonic_Para,Temper_Para);
				uart_tx(Tx_buf,8);
			}
			else 
			{
				sprintf((char*)Tx_buf,"ERROR\r\n");
				uart_tx(Tx_buf,7);
			}
		}
		
		keyscan();
	}
}

void ser() interrupt 4   //串口中断
{
	if(RI)
	{
		Rx_buf[Receive_num]=SBUF;
		Receive_num++;
		Receive_Flag=1;
		RI=0;
	}
}


void Timer2Init(void)		//1毫秒@12.000MHz
{
	AUXR |= 0x04;		//定时器时钟1T模式
	T2L = 0x20;		//设置定时初值
	T2H = 0xD1;		//设置定时初值
	AUXR |= 0x10;		//定时器2开始计时
	IE2 |= 0x04;    //开定时器2中断
	EA=1;
}

//中断服务程序
void timer2int() interrupt 12           //中断入口
{
		P2|=0XC0;//打开位选573   U8
	  P2&=0XDF;
		P0=(1<<Digcom);
		P2|=0XE0;
		P2&=0XFF;//打开段选573   U7
		P0=tab[Digbuf[Digcom]];
	 if(++Digcom==8)Digcom=0;
	
		intr++;
		if(intr==499)
		{
			Temp_Start = 1 ;
		}
		else if(intr==999)
		{
			intr=0;
			Temp_Start = 1 ;
			Ultrasonic_Start=1;
		}
	
		if(Receive_Flag>=1)
		{
			Receive_Flag++;
			if(Receive_Flag>=30)
			{
				Receive_Flag=0;
				Receive_Over=1;
			}
		}
}



void keyscan(void)
{
	P35=0;P34=1;
	if(P33==0)//按键S12
	{
		delayms(5);
		if(P33==0){S12=1;}
		while(!P33){Key_time=Key_time+1;delayms(10);}
	}
	else if(P32==0)//按键S13
	{
		delayms(5);
		if(P32==0){S13=1;}
		while(!P32){Key_time=Key_time+1;delayms(10);}
	}

	P35=1;P34=0;
	if(P33==0)//按键S16
	{
		delayms(5);
		if(P33==0)
		{
			if(Interface==1)
			{
				if(Switch==0)Temper_Para=Temper_Para-2;
				else if(Switch==1)Ultrasonic_Para=Ultrasonic_Para-5;
			}	
		}
		while(!P33);
	}
	else if(P32==0)//按键S17
	{
		delayms(5);
		if(P32==0)
		{
			if(Interface==1)
			{
				if(Switch==0)Temper_Para=Temper_Para+2;
				else if(Switch==1)Ultrasonic_Para=Ultrasonic_Para+5;
			}
		}
		while(!P32);
	}	
	
		if((S12==1)&&(Key_time>=99))
		{
			S12=0;Key_time=0;
			Change_num=0;
			EEPROM_write(0x00,0);delayms(5);
			EEPROM_write(0x01,0);delayms(5);
			EEPROM_write(0x02,0);delayms(5);
		}
		else if((S12==1)&&(Key_time<99))
		{
			S12=0;Key_time=0;
			if(Interface==0)
			{
				if(Switch==0){Switch=1;}
				else if(Switch==1){Switch=2;}
				else if(Switch==2){Switch=0;}
			}
			else if(Interface==1)
			{
				if(Switch==0){Switch=1;}
				else if(Switch==1){Switch=0;}
			}
		}
		
		if((S13==1)&&(Key_time>=99))
		{
			S13=0;Key_time=0;
			if(AD_start==1){AD_start=0;AD_write(20);P2|=0X80;P2&=0X9F;LED_Bit|=0X04;P0=LED_Bit;}
			else if(AD_start==0){AD_start=1;P2|=0X80;P2&=0X9F;LED_Bit&=0XFB;P0=LED_Bit;}
		}
		else if((S13==1)&&(Key_time<99))
		{
			S13=0;Key_time=0;
			if(Interface==0)
			{
				Interface=1;Switch=0;
				Temper_ParaLast=Temper_Para;Ultrasonic_ParaLast=Ultrasonic_Para;
			}
			else if(Interface==1)
			{
				Interface=0;Switch=0;
				if((Temper_Para!=Temper_ParaLast)|(Ultrasonic_Para!=Ultrasonic_ParaLast))
				{
					Change_num=Change_num+1;
					EEPROM_write(0x00,Change_num/10000);delayms(5);
					EEPROM_write(0x01,Change_num%10000/100);delayms(5);
					EEPROM_write(0x02,Change_num%100);delayms(5);
				}
			}
		}
}

void uart_tx(unsigned char *p,unsigned char length)
{
	unsigned char i;
	for(i=0;i<length;i++)
	{
		SBUF = *(p+i);
		while(TI == 0);
		TI = 0;
	}
}

void UartInit(void)		//4800bps@12.000MHz
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x40;		//定时器1时钟为Fosc,即1T
	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
	TMOD &= 0x0F;		//设定定时器1为16位自动重装方式
	TL1 = 0x8F;		//设定定时初值
	TH1 = 0xFD;		//设定定时初值
	ET1 = 0;		//禁止定时器1中断
	TR1 = 1;		//启动定时器1
}


unsigned int Ultrasonic(void)
{
		send_wave();  //发送方波信号
		TH0 = 0;
		TL0 = 0;	
		TR0 = 1;  //启动计时
		while((RX == 1) && (TF0 == 0));  //等待收到脉冲
		TR0 = 0;  //关闭计时		
		
		if(TF0 == 1)//发生溢出
		{
			TF0 = 0;
			distance = 9999;  //无返回
		}
		else
		{		
			t = TH0;// 计算时间  
			t <<= 8;
			t |= TL0;
			distance = (unsigned int)(t*0.017);  //计算距离	
			distance = distance/12;
			//t*340   // t/1000000*340/2*100
		}
		
		return distance;
}

void Timer0Init(void)	
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
}

void send_wave(void)
{
	unsigned char i = 8;  //发送8个脉冲
	
	do
	{
		TX = 1;
		somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;		
		TX = 0;
		somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;		
	}
	while(i--);
}



void delayms(int ms)
{
	int i,j;
	for(i=ms;i>0;i--)
		for(j=845;j>0;j--);
}

void allinit(void)
{
	P2|=0XA0;
	P2&=0XBF;
	P0=0X00;//关闭蜂鸣器，继电器
	
	P2|=0X80;
	P2&=0X9F;
	P0=0XFF;//关闭LED灯
	
	P2|=0XC0;//打开位选573   U8
	P2&=0XDF;
	P0=0XFF;//选择所有数码管
	P2|=0XE0;//打开位选573   U7
	P2&=0XFF;
	P0=0XFF;//关闭所有数码管
}

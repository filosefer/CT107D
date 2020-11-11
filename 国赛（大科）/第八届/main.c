#include<STC15F2K60S2.H>
#include "intrins.h"
#include "IIC.h"
	
uchar code tab[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xbf,0xff,0XC6,0X8E}; 
uchar yi,er,san,si,wu,liu,qi,ba;
uchar num;

#define somenop {_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_(); _nop_();}
sbit TX = P1^0;  //发射引脚
sbit RX = P1^1;  //接收引脚
unsigned int t = 0;
unsigned int intr = 0;
bit s_flag;
unsigned int distance;

unsigned char num=0;
unsigned char Save[4];
unsigned char Status=0;
unsigned char Return_num=0;
unsigned char Blind=20;

void allinit(void);
void delayms(uint ms);
void keyscan(void);
void display1(uchar yi,uchar er);
void display2(uchar san,uchar si);
void display3(uchar wu,uchar liu);
void display4(uchar qi,uchar ba);
void send_wave(void);
void Timer0Init(void)	;
void Timer1Init(void);
unsigned int Ultrasonic(void);
bit LED=0;
bit LED_flag=0;
unsigned char LED_num=0;


void main(void)
{
	allinit();
	Timer0Init();
  Timer1Init();
	EA = 1;
	ET0 = 1;  //打开定时器0中断
	
	Save[0]=EEPROM_read(0x00);delayms(5);Save[1]=EEPROM_read(0x01);delayms(5);
	Save[2]=EEPROM_read(0x02);delayms(5);Save[3]=EEPROM_read(0x03);delayms(5);
	Blind=EEPROM_read(0x04);delayms(5);//num=EEPROM_read(0x05);
	
	yi=12;er=11;san=11;si=11;wu=11;liu=11;qi=11,ba=11;
	while(1)
	{
		keyscan();
		display1(yi,er);
		display2(san,si);
		display3(wu,liu);
		display4(qi,ba);
	}
}

void keyscan(void)
{
	if(P30==0)
	{
		delayms(5);
		if(P30==0)
		{
			if(Status==0)
			{
				Save[num]=Ultrasonic();
				yi=12;er=11;
				san=distance/100;si=distance%100/10;wu=distance%10;
				if(num==0){liu=Save[3]/100;qi=Save[3]%100/10;ba=Save[3]%10;num=1;}
				else if(num==1){liu=Save[0]/100;qi=Save[0]%100/10;ba=Save[0]%10;num=2;}
				else if(num==2){liu=Save[1]/100;qi=Save[1]%100/10;ba=Save[1]%10;num=3;}
				else if(num==3){liu=Save[2]/100;qi=Save[2]%100/10;ba=Save[2]%10;num=0;}	
				
				EEPROM_write(0x00,Save[0]);delayms(5);EEPROM_write(0x01,Save[1]);delayms(5);
				EEPROM_write(0x02,Save[2]);delayms(5);EEPROM_write(0x03,Save[3]);delayms(5);
				//EEPROM_write(0x05,num);
			}					
		}
		while(!P30);
	}
	else if(P31==0)
	{
		delayms(5);
		if(P31==0)
		{
			if(Status==0)
			{
				Status=1;
				P2=0X80;P0=0X7F;
				yi=Return_num+1;
				er=11;san=11;si=11;wu=11;
				liu=Save[Return_num]/100;qi=Save[Return_num]%100/10;ba=Save[Return_num]%10;
				Return_num++;
			}
			else if(Status==1)
			{
				Status=0;
				P2=0X80;P0=0XFF;
				yi=12;er=11;
				san=distance/100;si=distance%100/10;wu=distance%10;
				if(num==1){liu=Save[3]/100;qi=Save[3]%100/10;ba=Save[3]%10;}
				else if(num==2){liu=Save[0]/100;qi=Save[0]%100/10;ba=Save[0]%10;}
				else if(num==3){liu=Save[1]/100;qi=Save[1]%100/10;ba=Save[1]%10;}
				else if(num==0){liu=Save[2]/100;qi=Save[2]%100/10;ba=Save[2]%10;}	
			}
		}
		while(!P31);
	}
	else if(P32==0)
	{
		delayms(5);
		if(P32==0)
		{
			if(Status==0)
			{
				Status=2;
				P2=0X80;P0=0XBF;
				yi=13;er=11;san=11;si=11;wu=11;liu=11;
				qi=Blind/10;ba=Blind%10;
			}
			else if(Status==2)
			{
				Status=0;
				P2=0X80;P0=0XFF;
				yi=12;er=11;
				san=distance/100;si=distance%100/10;wu=distance%10;
				if(num==1){liu=Save[3]/100;qi=Save[3]%100/10;ba=Save[3]%10;}
				else if(num==2){liu=Save[0]/100;qi=Save[0]%100/10;ba=Save[0]%10;}
				else if(num==3){liu=Save[1]/100;qi=Save[1]%100/10;ba=Save[1]%10;}
				else if(num==0){liu=Save[2]/100;qi=Save[2]%100/10;ba=Save[2]%10;}	
			}
		}
		while(!P32);
	}
	else if(P33==0)
	{
		delayms(5);
		if(P33==0)
		{
			if(Status==1)
			{	
				yi=Return_num+1;er=11;san=11;si=11;wu=11;
				liu=Save[Return_num]/100;qi=Save[Return_num]%100/10;ba=Save[Return_num]%10;
				Return_num++;
				if(Return_num==4)Return_num=0;
			}
			else if(Status==2)
			{
				if(Blind==0){Blind=10;}
				else if(Blind==10){Blind=20;}
				else if(Blind==20){Blind=30;}
				else if(Blind==30){Blind=0;}
				yi=13;er=11;san=11;si=11;wu=11;liu=11;
				qi=Blind/10;ba=Blind%10;
				EEPROM_write(0x04,Blind);delayms(5);
			}
		}
		while(!P33);
	}
}

unsigned int Ultrasonic(void)
{
		send_wave();  //发送方波信号
		TH1 = 0;
		TL1 = 0;	
		TR1 = 1;  //启动计时
		while((RX == 1) && (TF1 == 0));  //等待收到脉冲
		TR1 = 0;  //关闭计时		
		
		if(TF1 == 1)//发生溢出
		{
			TF1 = 0;
			distance = 9999;  //无返回
		}
		else
		{		
			t = TH1;// 计算时间  
			t <<= 8;
			t |= TL1;
			distance = (unsigned int)(t*0.017);  //计算距离	
			distance = distance/12;
			//t*340   // t/1000000*340/2*100
		}
		
		if(distance<=Blind){AD_write(0x00);}
		else if(distance>255){AD_write(0xff);}
		else {AD_write(distance-Blind);}	

		LED=1;LED_num=0;intr=0;
		P2=0X80;P0=0XFE;		
		
		return distance;
}

void Timer0Init(void)		//2毫秒@11.0592MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x9A;		//设置定时初值
	TH0 = 0xA9;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
}

void Timer1Init(void)		//2毫秒@11.0592MHz
{
	AUXR |= 0x40;		//定时器时钟1T模式
	TMOD &= 0x0F;		//设置定时器模式
}

//定时器0中断服务函数
void isr_timer_0(void)  interrupt 1  //默认中断优先级 1
{
	intr++;
	if((intr == 250)&&(LED_num<3)&&(LED==1))
	{
		intr = 0;
		if(LED_flag==0){P2=0X80;P0=0XFF;LED_flag=1;LED_num++;}
		else if(LED_flag==1){P2=0X80;P0=0XFE;LED_flag=0;}
  }
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



void allinit(void)
{
	P2=0XA0;
	P0=0x00;//关闭蜂鸣器继电器
	
	P2=0X80;
	P0=0XFF;//关闭LED灯
	
	P2=0XC0;
	P0=0XFF;//选择所有数码管
	P2=0XFF;
	P0=0XFF;//关闭多有数码管	
}

void display1(uchar yi,uchar er)
{
	P2=0XC0;
	P0=0X01;//选择所有数码管
	P2=0XFF;
	P0=tab[yi];//关闭多有数码管	
	delayms(1);
	
	P2=0XC0;
	P0=0X02;//选择所有数码管
	P2=0XFF;
	P0=tab[er];//关闭多有数码管	
	delayms(1);
}

void display2(uchar san,uchar si)
{
	P2=0XC0;
	P0=0X04;//选择所有数码管
	P2=0XFF;
	P0=tab[san];//关闭多有数码管	
	delayms(1);
	
	P2=0XC0;
	P0=0X08;//选择所有数码管
	P2=0XFF;
	P0=tab[si];//关闭多有数码管	
	delayms(1);
}

void display3(uchar wu,uchar liu)
{
	P2=0XC0;
	P0=0X10;//选择所有数码管
	P2=0XFF;
	P0=tab[wu];//关闭多有数码管	
	delayms(1);
	
	P2=0XC0;
	P0=0X20;//选择所有数码管
	P2=0XFF;
	P0=tab[liu];//关闭多有数码管	
	delayms(1);
}

void display4(uchar qi,uchar ba)
{
	P2=0XC0;
	P0=0X40;//选择所有数码管
	P2=0XFF;
	P0=tab[qi];//关闭多有数码管	
	delayms(1);
	
	P2=0XC0;
	P0=0X80;//选择所有数码管
	P2=0XFF;
	P0=tab[ba];//关闭多有数码管	
	delayms(1);
}

void delayms(uint ms)
{
	uint i,j;
	for(i=ms;i>0;i--)
		for(j=845;j>0;j--);
}

//void Delay20us()		//@11.0592MHz
//{
//	unsigned char i;

//	_nop_();
//	_nop_();
//	_nop_();
//	i = 52;
//	while (--i);
//}

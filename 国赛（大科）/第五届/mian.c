/*******************************************************************************  
* 文件名称：蓝桥杯单片机第五届决赛试题--视频讲解对应程序
* 实验目的：多功能事件记录器         
* 程序说明：如需了解有关本程序配套视频讲解，及比赛经验分享\
*					 欢迎访问淘宝店铺网址：shop117015787.taobao.com
* 日期版本：2016-3/V1.0
*******************************************************************************/

#include<STC15F2K60S2.H>
#include<DEPUTY.H>
#include <stdio.h>

uchar code tab[]={0XC0,0XF9,0XA4,0XB0,0X99,0X92,0X82,0XF8,0X80,0X90,0XBF,0XFF,0xc6,0x89};
uchar yi,er,san,si,wu ,liu,qi,ba;

unsigned char Tx_buf1[] = "{20-20%}{23-50-00}{0}";
//unsigned char Password[7]="AAASSS";
//unsigned char Receiveword[7];
uchar Receive,Receive_num=0;

uchar flag,num;

unsigned char wendu,shidu;
uchar zidong=1,yanzheng=0,wrong=0;
uchar Sec_last=0;
unsigned char jiejin,line;
uchar qiehuan=0;
uchar come,go,jishi,start;

uchar ADD=0X00;

void delayms(int ms);
void allinit();
void keyscan();
void display1(uchar yi,uchar er);
void display2(uchar san,uchar si);
void display3(uchar wu,uchar liu);
void display4(uchar qi,uchar ba);

void UartInit(void);
void uart_tx(unsigned char *p,unsigned char length);

void main()
{
	uchar i;
	allinit();
	yi=8;er=0;san=10;si=3;wu=0;liu=11;qi=0;ba=0;
	dsinit();
	
	UartInit();
	EA=1;
	ES=1;
	while(1)
	{
		line=AD_Read(0x01);line=AD_Read(0x01);
		dsget();
		wendu=temget();
		shidu=AD_Read(0x03)/2.57;shidu=AD_Read(0x03)/2.57;
		
		if(line>100){jiejin=0;}
		else {jiejin=1;P2=0X80;P0&=0XFB;}
		
		if(qiehuan==0)
		{
			yi=wendu%100/10;er=wendu%10;san=12; liu=shidu/10;qi=shidu%10;ba=13;  si=11;wu=11;
		}
		else if(qiehuan==1)
		{
			yi=shijian[2]/10;er=shijian[2]%10;si=shijian[1]/10;wu=shijian[1]%10;qi=shijian[0]/10;ba=shijian[0]%10;
			//san=10;liu=10;
			if(shijian[0]%2==0)
			{
				san=10;liu=10;
			}
			else 
			{
				san=11;liu=11;
			}
		}
		else if(qiehuan==2)
		{
			yi=11;er=11;san=11;si=10;wu=0;liu=0;qi=jishi/10;ba=jishi%10;
		}
		
		if(zidong==0)
		{
			P2=0X80;P0&=0XFD;
			
			if(line>100)
			{
				if(start==1)
				{
					start=0;
					go=shijian[0];
					if(go>=come)jishi=go-come;
					else jishi=60-come+go;
					EEPROM_Write(ADD,wendu);delayms(2);ADD++;
					EEPROM_Write(ADD,shidu);delayms(2);ADD++;
					EEPROM_Write(ADD,shijian[2]);delayms(2);ADD++;
					EEPROM_Write(ADD,shijian[1]);delayms(2);ADD++;
					EEPROM_Write(ADD,shijian[0]);delayms(2);ADD++;
					EEPROM_Write(ADD,jishi);delayms(2);ADD++;	
					if(ADD==0X23)ADD=0X00;
				}
			}
			else 
			{
				if(start==0)
				{
					come=shijian[0];
					start=1;
				}
			}
			
			if(yanzheng==1)
			{
				yanzheng=0;
				ADD=0x00;
				for(i=0;i<5;i++)
				{
					wendu=EEPROM_Read(ADD);delayms(2);ADD++;
					shidu=EEPROM_Read(ADD);delayms(2);ADD++;
					shijian[2]=EEPROM_Read(ADD);delayms(2);ADD++;
					shijian[1]=EEPROM_Read(ADD);delayms(2);ADD++;					
					shijian[0]=EEPROM_Read(ADD);delayms(2);ADD++;
					jishi=EEPROM_Read(ADD);delayms(2);ADD++;
					sprintf((char*)Tx_buf1,"{%bd-%bd%%}{%0.2bd-%0.2bd-%0.2bd}{%bd}\r\n",wendu,shidu,shijian[2],shijian[1],shijian[0],jishi);
					uart_tx(Tx_buf1,23);
				}
				
			}
		}
		else if(zidong==1)
		{
			if(yanzheng==1)
			{
				if(Sec_last!=shijian[0])
				{
					Sec_last=shijian[0];
					sprintf((char*)Tx_buf1,"{%bd-%bd%%}{%0.2bd-%0.2bd-%0.2bd}{%bd}\r\n",wendu,shidu,shijian[2],shijian[1],shijian[0],jiejin);
					uart_tx(Tx_buf1,23);
				}
			}
			//yi=11;er=11;san=11;si=11;wu=11;liu=11;qi=11;ba=11;
			P2=0X80;P0&=0XFE;
		}	
//		wendu=temget();
//		yi=wendu%100/10;er=wendu%10;
		
		//line=AD_Read(0x01);
//		shidu=AD_Read(0x03)/2.57;
//		
//		liu=shidu/10;qi=shidu%10;//ba=13;
		
//		dsget();
//		yi=shijian[2]/10;er=shijian[2]%10;si=shijian[1]/10;wu=shijian[1]%10;qi=shijian[0]/10;ba=shijian[0]%10;san=10;liu=10;

		keyscan();
		display1(yi,er);
		display2(san,si);
		display3(wu,liu);
		display4(qi,ba);
	}
}


void keyscan()
{
	if(P32==0)
	{
		delayms(5);
		if(P32==0)
		{
			if(qiehuan==0)qiehuan=1;
			else if(qiehuan==1)qiehuan=2;
			else if(qiehuan==2)qiehuan=0;
		}
		while(!P32);
	}
	else if(P33==0)
	{
		delayms(5);
		if(P33==0)
		{
			if(zidong==0)
			{
				zidong=1;yanzheng=0;
			}
			else 
			{
				zidong=0;yanzheng=0;
			}
		}
		while(!P33);
	}
}

void ser() interrupt 4   //串口中断
{
	RI=0;
	Receive=SBUF;
	
	if(Receive_num==0)
	{
		if(Receive=='A')
		{
			Receive_num=1;
		}
	}
	else if(Receive_num==1)
	{
		if(Receive=='A')
		{
			Receive_num=2;
		}
	}
	else if(Receive_num==2)
	{
		if(Receive=='A')
		{
			Receive_num=3;
		}
	}
	else if(Receive_num==3)
	{
		if(Receive=='S')
		{
			Receive_num=4;
		}
	}	
	else if(Receive_num==4)
	{
		if(Receive=='S')
		{
			Receive_num=5;
		}
	}
	else if(Receive_num==5)
	{
		if(Receive=='S')
		{
			Receive_num=0;yanzheng=1;
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
		display1(yi,er);
		display2(san,si);
		display3(wu,liu);
		display4(qi,ba);	
	}
}

void UartInit(void)		//1200bps@11.0592MHz
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x40;		//定时器1时钟为Fosc,即1T
	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
	TMOD &= 0x0F;		//设定定时器1为16位自动重装方式
	TL1 = 0x00;		//设定定时初值
	TH1 = 0xF7;		//设定定时初值
	ET1 = 0;		//禁止定时器1中断
	TR1 = 1;		//启动定时器1
}


void delayms(int ms)
{
	int i,j;
	for(i=ms;i>0;i--)
		for(j=845;j>0;j--);
}

void allinit()
{
	P2=0XA0;
	P0=0X00;//关闭蜂鸣器，继电器
	
	P2=0X80;
	P0=0XFF;//关闭LED灯
	
	P2=0XC0;
	P0=0XFF;//选择所有数码管
	P2=0XFF;
	P0=0XFF;//关闭所有数码管
}

void display1(uchar yi,uchar er)
{
		P2=0XC0;//打开位选573   U8
		P0=0X01;//选择第一个数码管
		P2=0XFF;//打开段选573   U7
		P0=tab[yi];
		delayms(1);
		
		P2=0XC0;//打开位选573   U8
		P0=0X02;//选择第二个数码管
		P2=0XFF;//打开段选573   U7
		P0=tab[er];
		delayms(1);
}	

void display2(uchar san,uchar si)
{
		P2=0XC0;//打开位选573   U8
		P0=0X04;//选择第三个数码管
		P2=0XFF;//打开段选573   U7
		P0=tab[san];
		delayms(1);
		
		P2=0XC0;//打开位选573   U8
		P0=0X08;//选择第四个数码管
		P2=0XFF;//打开段选573   U7
		P0=tab[si];
		delayms(1);
}

void display3(uchar wu,uchar liu)
{
		P2=0XC0;//打开位选573   U8
		P0=0X10;//选择第一个数码管
		P2=0XFF;//打开段选573   U7
		P0=tab[wu];
		delayms(1);
		
		P2=0XC0;//打开位选573   U8
		P0=0X20;//选择第一个数码管
		P2=0XFF;//打开段选573   U7
		P0=tab[liu];
		delayms(1);
}

void display4(uchar qi,uchar ba)
{
		P2=0XC0;//打开位选573   U8
		P0=0X40;//选择第一个数码管
		P2=0XFF;//打开段选573   U7
		P0=tab[qi];
		delayms(1);
		
		P2=0XC0;//打开位选573   U8
		P0=0X80;//选择第一个数码管
		P2=0XFF;//打开段选573   U7
		P0=tab[ba];
		delayms(1);
	
		P2=0XC0;//打开位选573   U8
		P0=0X80;//选择第一个数码管
		P2=0XFF;//打开段选573   U7
		P0=0XFF;
}




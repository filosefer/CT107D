#include<stc15f2k60s2.h>
#include<iic.h>

typedef unsigned int uint;
typedef unsigned char uchar;

int yi,er,san,si,wu,liu,qi,ba;

uchar code smg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xc1,0x8c,0xc8,0xff,
0xc0&0x7f,0xf9&0x7f,0xa4&0x7f,0xb0&0x7f,0x99&0x7f,0x92&0x7f,0x82&0x7f,0xf8&0x7f,0x80&0x7f,0x90&0x7f};

int dianya,jishu,diancp,jiemian,flag,t,tt,liang,wuxiao,flagg,wuu;
void delay1ms(int ms);
void display1();
void display2();
void shangdian();
void Timer0Init(void)	;	//1毫秒@12.000MHz
void juzhen();
void main()
{
	shangdian();
	Timer0Init();
	diancp=a24c02read(0);
	diancp=diancp*10;
	while(1)
	{
		juzhen();
		dianya=getad(0x03);
		if(jiemian==0)//数据界面
		{
			yi=10;er=san=si=wu=13;liu=dianya/100+14;qi=dianya%100/10;ba=dianya%10;
		}
		else if(jiemian==1)//参数界面
		{
		  yi=11;er=san=si=wu=13;liu=diancp/100+14;qi=diancp%100/10;ba=diancp%10;
		}
		else if(jiemian==2)//计数界面
		{
			yi=12;er=san=si=wu=liu=13;qi=jishu/10;ba=jishu%10;

		}
	
	  if(dianya>diancp)
		{
		  flag=1;
			t=tt=0;
			TR0=0;
			liang=0;
			flagg=0;
			
		}
		else if(dianya<diancp)
		{
			
			TR0=1;
			flagg=1;
		
		}
		
		if((flag==1)&&(flagg==1))
		{
			jishu++;
			flag=0;
			flagg=0;
		}

				
		if((liang==0)&&(jishu%2==0)&&(wuu==0))
		{
			 P2=0X80;P0=0XFF;	
		}
		else if((liang==0)&&(jishu%2==0)&&(wuu==1))
		{
			 P2=0X80;P0=~(0X04);
		}
		else if((liang==0)&&(jishu%2==1)&&(wuu==0))
	  {
			P2=0X80;P0=~(0X02);
		}
		else if((liang==0)&&(jishu%2==1)&&(wuu==1))
	{  P2=0X80;P0=~(0X04|0x02);
		}
		else if((liang==1)&&(jishu%2==0)&&(wuu==0))
			{P2=0X80;P0=~(0X01);
		}
		else if((liang==1)&&(jishu%2==0)&&(wuu==1))
			{P2=0X80;P0=~(0X01|0x04);
		}
		else if((liang==1)&&(jishu%2==1)&&(wuu==0))
			{P2=0X80;P0=~(0X01|0x02);
				
		}
		else if((liang==1)&&(jishu%2==1)&&(wuu==1))
			{ P2=0X80;P0=~(0X01|0x02|0x04);
		}
	 display1();
   display2();
	
	}
	
	

}
void shangdian()
{
	P2=0XA0;P0=0X00;
	P2=0X80;P0=0XFF;
	P2=0XC0;P0=0XFF;
	P2=0XE0;P0=0XFF;
}
void delay1ms(int ms)
{
	int i;
	for(;ms>0;ms--)
	{
		for(i=845;i>0;i--) ;
	
	}

}
void Timer0Init(void)		//1毫秒@12.000MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x20;		//设置定时初值
	TH0 = 0xD1;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 0;		//定时器0开始计时
	EA=1;
	ET0=1;
}
void time0() interrupt 1
{
	t++;
	if(t==1000)
	{ t=0;
		tt++;
	  if(tt==5)
		{
			liang=1;

		}
	
	}


}

void display1()
{
	P2=0XC0;P0=0X01;
	P2=0XE0;P0=smg[yi];
	delay1ms(1);
	P2=0XC0;P0=0X02;
	P2=0XE0;P0=smg[er];
	delay1ms(1);	
	P2=0XC0;P0=0X04;
	P2=0XE0;P0=smg[san];
	delay1ms(1);
	P2=0XC0;P0=0X08;
	P2=0XE0;P0=smg[si];
	delay1ms(1);
}
void display2()
{
	P2=0XC0;P0=0X10;
	P2=0XE0;P0=smg[wu];
	delay1ms(1);
	P2=0XC0;P0=0X20;
	P2=0XE0;P0=smg[liu];
	delay1ms(1);	
	P2=0XC0;P0=0X40;
	P2=0XE0;P0=smg[qi];
	delay1ms(1);
	P2=0XC0;P0=0X80;
	P2=0XE0;P0=smg[ba];
	delay1ms(1);
	P2=0XC0;P0=0X80;
	P2=0XE0;P0=0xff;	
}
void juzhen()
{
	uchar temp;
		
P44=1;P42=1;P3=0XdF;
	temp=P3;
	temp=temp&0x0f;
	if(temp!=0x0f)
	{
		delay1ms(5);
		temp=P3;
		temp=temp&0x0f;
		if(temp!=0x0f)
		{
			temp=P3;
			switch(temp)
			{
			
				case 0xdb:
				if(jiemian==2){
				wuu=0;
				jishu=0;}
				else {
					wuxiao++;
					if(wuxiao==3)
					{	wuu=1;
				    wuxiao=0;
				}}
				break;
				case 0xd7:
					wuu=0;
				jiemian++;
				if(jiemian==2)
				{ 
				  a24c02xie(0,diancp/10);
					delay1ms(5);
					
				}
				if(jiemian==3) jiemian=0;
				break;

			}
			while(temp!=0x0f)
			{
				temp=P3;
		    temp=temp&0x0f;			
			}
		
		}
	
	}

P44=1;P42=1;P3=0XeF;
	temp=P3;
	temp=temp&0x0f;
	if(temp!=0x0f)
	{
		delay1ms(5);
		temp=P3;
		temp=temp&0x0f;
		if(temp!=0x0f)
		{
			temp=P3;
			switch(temp)
			{
				
				case 0xeb:
					if(jiemian==1){
						wuu=0;
				diancp=diancp-50;
         if(diancp==-50)
				 {
				  diancp=500;
				 }}
				else {wuxiao++;
					if(wuxiao==3)
					{wuu=1;wuxiao=0;}
				}
				break;
				case 0xe7: 
				if(jiemian==1){	
					wuu=0;
				diancp=diancp+50;
         if(diancp==550)
				 {
				  diancp=0;
				 }}
				else {wuxiao++;
					if(wuxiao==3)
					{	wuu=1;
				wuxiao=0;}}
				break;

			}
			while(temp!=0x0f)
			{
				temp=P3;
		    temp=temp&0x0f;			
			}
		
		}
	
	}

}

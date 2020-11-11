/*******************************************************************************  
* 文件名称：蓝桥杯单片机第三届决赛试题--视频讲解对应程序
* 实验目的：门禁系统         
* 程序说明：如需了解有关本程序配套视频讲解，及比赛经验分享\
*					 欢迎访问淘宝店铺网址：shop117015787.taobao.com
* 日期版本：2016-3/V1.0
*******************************************************************************/

#include<STC15F2K60S2.H>
#include<DEPUTY.H>
#include <intrins.h>

uchar code tab[]={0XC0,0XF9,0XA4,0XB0,0X99,0X92,0X82,0XF8,0X80,0X90,0XBF,0XFF};
uchar yi,er,san,si,wu ,liu,qi,ba;

#define somenop {_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_(); _nop_();}
sbit TX = P1^0;  //发射引脚
sbit RX = P1^1;  //接收引脚
unsigned int intr = 0;
bit s_flag;
unsigned int t = 0;
unsigned int distance;

uchar num ,flag;
uchar relay=0,buzz=0;
uint relay_num=0,buzz_num=0;
uchar i,k;

uchar shezhi,fuwei,queren,tuichu,wrong;
uchar password[6]={6,5,4,3,2,1};
uchar kw[6]={11,11,11,11,11,11};

uchar Oldm=0;
uchar ADD=0X55;

uchar tube=0,tube_num=1;


void send_wave(void);
void Timer0Init(void)	;
void Timer1Init(void);
void Ultrasonic(void);

void delayms(int ms);
void allinit();
void keyscan();
void display1(uchar yi,uchar er);
void display2(uchar san,uchar si);
void display3(uchar wu,uchar liu);
void display4(uchar qi,uchar ba);


void Password_deal(void);
void Reset_deal(void);

void main()
{	
	uchar z;
	allinit();
	yi=0;er=6;san=10;si=5;wu=9;liu=10;qi=0;ba=0;
	
//	for(z=0;z<6;z++)
//	{
//		EEPROM_Write(ADD,password[z]);
//		delayms(2);
//		ADD++;
//	}
	
	for(z=0;z<6;z++)
	{
		password[z]=EEPROM_Read(ADD);
		delayms(2);
		ADD++;
	}
	z=0;
	
	Timer0Init();
  Timer1Init();

	EA = 1;
	ET0 = 1;  //打开定时器0中断
	
	dsinit();	
	while(1)
	{
		keyscan();
		dsget();
		if(shezhi==0)
		{
			Ultrasonic();
			if(distance<30)
			{
				if((shijian[2]>=7)&&(shijian[2]<22))
				{
					yi=shijian[2]/10;er=shijian[2]%10;si=shijian[1]/10;wu=shijian[1]%10;qi=shijian[0]/10;ba=shijian[0]%10;san=10;liu=10;
					relay=1;P2=0XA0;P0=0X10;
				}
				else 
				{
					//yi=10;er=10;san=11,si=11,wu=11,liu=11,qi=11,ba=11;
					san=kw[0];si=kw[1];wu=kw[2];liu=kw[3];qi=kw[4];ba=kw[5];	
					Password_deal();
				}
			}
			else
			{
				yi=shijian[2]/10;er=shijian[2]%10;si=shijian[1]/10;wu=shijian[1]%10;qi=shijian[0]/10;ba=shijian[0]%10;san=10;liu=10;
			}
			
		}
		else if(shezhi==1)
		{
			Reset_deal();
			san=kw[0];si=kw[1];wu=kw[2];liu=kw[3];qi=kw[4];ba=kw[5];
		}		
		
		//Ultrasonic();
//		liu = distance/100;
//		qi = distance%100/10;
//		ba = distance%10; 
		//ba=num;
		//ba=num;
		//dsget();
		//yi=shijian[2]/10;er=shijian[2]%10;si=shijian[1]/10;wu=shijian[1]%10;qi=shijian[0]/10;ba=shijian[0]%10;san=10;liu=10;
		display1(yi,er);
		display2(san,si);
		display3(wu,liu);
		display4(qi,ba);
	}
}



void Reset_deal(void)
{
	if((shezhi==1)&&(Oldm==0))
	{yi=11;er=10;}
	else if((shezhi==1)&&(Oldm==1))
	{yi=10;er=11;}
		
	if(tuichu==1)
	{
		tuichu=0;shezhi=0;yi=10;er=10;Oldm=0;
		for (i=0;i<6;i++)
		{kw[i]=11;}
		i=0;
	}
	
	
	
	if(flag==1)
	{kw[i++]=num;flag=0;}
	
	if((queren==1)&&(Oldm==0))
	{
		queren=0;Oldm=1;
		for(i=0;i<6;i++)
		{
			if(password[i]!=kw[i])
			{wrong=1;Oldm=0;}
		}
		i=0;
		if(wrong==0)
		{
			for(i=0;i<6;i++)
			{kw[i]=11;}
			i=0;
		}
		else if(wrong==1)
		{
			wrong=0;
			k=k+1;
			for(i=0;i<6;i++)
			{kw[i]=11;}i=0;
			if(k==3)
			{buzz=1;P2=0XA0;P0=0X40;Oldm=0;}
		}
	}
	
	if(Oldm==1)
	{
		if(queren==1)
		{				
			for(i=0;i<6;i++)
			{password[i]=kw[i];}
			i=0;queren=0;Oldm=0;shezhi=0;yi=10;er=10;
			for(i=0;i<6;i++)
			{kw[i]=11;}i=0;
			
			ADD=0x55;
			for(i=0;i<6;i++)
			{
				EEPROM_Write(ADD,password[i]);
				delayms(2);
				ADD++;
			}
		}
	}
}

void Password_deal(void)
{
	yi=10;er=10;
	if(flag==1)
	{
		kw[i++]=num;
		flag=0;
	}
	
	if(queren==1)
	{
		queren=0;
		for(i=0;i<6;i++)
		{
			if(password[i]!=kw[i])
				wrong=1;
		}
		i=0;
		if(wrong==0)
		{
			for(i=0;i<6;i++)
			{kw[i]=11;}
			i=0;k=0;relay=1;P2=0XA0;P0=0X10;
		}
		if(wrong==1)
		{
			for(i=0;i<6;i++)
			{kw[i]=11;}i=0;
			wrong=0;
			k=k+1;
			if(k>=3)
			{buzz=1;P2=0XA0;P0=0X40;}
			//for(i=0;i<6;i--)
			//{kw[i]=11;}
			i=0;
		}
	}
}


void keyscan()
{
	uchar temp,j=0;
	P3=0XFF;P4=0XEF;
	temp=P3;
	temp=temp&0x0f;
	while(temp!=0x0f)
	{
		delayms(2);
		temp=P3;
		temp=temp&0x0f;
		while(temp!=0x0f)
		{
			temp=P3;
			switch(temp)
			{
				case 0xfe:num=0;flag=1;break;
				case 0xfd:num=4;flag=1;break;
				case 0xfb:num=8;flag=1;break;
			}
			while(temp!=0x0f)
			{
				temp=P3;
				temp=temp&0x0f;
			}
		}
	}
	
	P3=0XFF;P4=0XFB;
	temp=P3;
	temp=temp&0x0f;
	while(temp!=0x0f)
	{
		delayms(2);
		temp=P3;
		temp=temp&0x0f;
		while(temp!=0x0f)
		{
			temp=P3;
			switch(temp)
			{
				case 0xfe:num=1;flag=1;break;
				case 0xfd:num=5;flag=1;break;
				case 0xfb:num=9;flag=1;break;
			}
			while(temp!=0x0f)
			{
				temp=P3;
				temp=temp&0x0f;
			}
		}
	}
	
	P3=0XDF;
	temp=P3;
	temp=temp&0x0f;
	while(temp!=0x0f)
	{
		delayms(2);
		temp=P3;
		temp=temp&0x0f;
		while(temp!=0x0f)
		{
			temp=P3;
			switch(temp)
			{
				case 0xde:num=2;flag=1;break;
				case 0xdd:num=6;flag=1;break;
				case 0xdb:shezhi=1;yi=10;er=10;san=11;si=11;wu=11;liu=11;qi=11;ba=11;break;
				case 0xd7:queren=1;break;
			}
			while(temp!=0x0f)
			{
				temp=P3;
				temp=temp&0x0f;
			}
		}
	}
	
	P3=0XEF;
	temp=P3;
	temp=temp&0x0f;
	while(temp!=0x0f)
	{
		delayms(2);
		temp=P3;
		temp=temp&0x0f;
		while(temp!=0x0f)
		{
			temp=P3;
			switch(temp)
			{
				case 0xEE:num=3;flag=1;break;
				case 0xED:num=7;flag=1;break;
				case 0xEB:fuwei=1;
//				for()
				break;
				case 0xE7:tuichu=1;break;
			}
			while(temp!=0x0f)
			{
				temp=P3;
				temp=temp&0x0f;
			}
		}
	}
}

void Ultrasonic(void)
{
	if(s_flag) // 200毫秒更新一次数据 
	{
		s_flag = 0;
		send_wave();  //发送方波信号
		TR1 = 1;  //启动计时
		while((RX == 1) && (TF1 == 0))  //等待收到脉冲
		{
			if((TH1==0X40)|(TH1==0X80)|(TH1==0XA0)|(TH1==0XE0))
			{
				display1(yi,er);
				display2(san,si);
				display3(wu,liu);
				display4(qi,ba);
			}
		}
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
		TH1 = 0;
		TL1 = 0;
	}
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
	TL0 = 0x9A;		//设置定时初值
	TH0 = 0xA9;		//设置定时初值

	if(++intr == 100)
	{
		s_flag = 1;
		intr = 0;
  }
	
	if(relay==1)
	{
		relay_num++;
		if(relay_num==2500)
		{
			relay_num=0;
			relay=0;
			P2=0XA0;P0=0X00;
		}
	}
	
	if(buzz==1)
	{
		buzz_num++;
		if(buzz_num==1500)
		{
			buzz_num=0;
			buzz=0;
			P2=0XA0;P0=0X00;
		}
	}
}

//TX引脚发送40KHz方波信号驱动超声波发送探头
//使用软件延时注意RC振荡器频率
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
}


//问题0：夜晚，超声波工作，数码管显示时间，当超声波探测距离30CM事，数码管自动切换--        ，；
    //如果超声波探测不到，则会自动返回时间显示界面。

//问题一：夜晚，超声波不工作，数码管显示--        ，上电默认06:59:00 但是数码管不显示；
       //试题要求上电显示的时间 是啥好意思

//问题二：密码的重置应该在一天的任意时间短内都可以进行，而并非只有晚上；


//问题三：重置密码时，于此同时超声波是否探测













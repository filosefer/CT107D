/*******************************************************************************  
* 文件名称：蓝桥杯单片机第六届决赛试题--视频讲解对应程序
* 实验目的：智能物料传送系统       
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
unsigned int t = 0;
unsigned int intr = 0;
bit s_flag;
unsigned int distance;

uchar leibie=0,kezai=0,chuansong,miao,miao_1,miao_2,jiting;
uchar shan=0;
uchar AD_num;
unsigned int miao_t;
unsigned int jiting_t;

uchar shezhi=0;

void delayms(int ms);
void allinit();
void keyscan();
void display1(uchar yi,uchar er);
void display2(uchar san,uchar si);
void display3(uchar wu,uchar liu);
void display4(uchar qi,uchar ba);

void send_wave(void);
void Timer0Init(void)	;
void Timer1Init(void);
void Ultrasonic(void);

void main()
{
	allinit();
	yi=8;er=0;san=10;si=3;wu=0;liu=11;qi=0;ba=0;
	
//	EEPROM_Write(0X00,2);delayms(2);
//	EEPROM_Write(0X01,4);delayms(2);
	
	miao_1=EEPROM_Read(0X00);delayms(2);
	miao_2=EEPROM_Read(0X01);delayms(2);
	
	Timer0Init();
  Timer1Init();
	EA = 1;
	ET0 = 1;  //打开定时器0中断
	while(1)
	{
		AD_num=AD_Read(0X03);
		if((AD_num>0)&&(AD_num<51))
		{
			if(shezhi==0)
			{
				yi=11;er=11;san=11;si=11;wu=11;liu=11;qi=11;ba=11;
			}
			else if(shezhi==1)
			{
				if(shan<100){yi=3;er=11;san=11;si=miao_1/10;wu=miao_1%10;liu=11;qi=miao_2/10;ba=miao_2%10;shan++;}
				else {yi=3;er=11;san=11;si=11;wu=11;liu=11;qi=miao_2/10;ba=miao_2%10; shan++; if(shan>=200)shan=0;}
				
			}
			else if(shezhi==2)
			{
				if(shan<100){yi=3;er=11;san=11;si=miao_1/10;wu=miao_1%10;liu=11;qi=miao_2/10;ba=miao_2%10;shan++;}
				else {yi=3;er=11;san=11;si=miao_1/10;wu=miao_1%10;liu=11;qi=11;ba=11; shan++; if(shan>=200)shan=0;}
				
			}
			P2=0X80;P0=0XFE;
			P2=0XA0;P0=0X00; 
			kezai=0;
		}
		else if((AD_num>=51)&&(AD_num<204))
		{
			P2=0X80;P0=0XFD;
			kezai=1;
			if((chuansong==0)&&(jiting==0))
			{
				P2=0XA0;P0=0X00; 
				Ultrasonic();
				if(distance<=30){leibie=1;miao=miao_1;}
				else {leibie=2;miao=miao_2;}
				yi=1;er=11;san=11;si=distance%100/10;wu=distance%10;liu=11;qi=11;ba=leibie;
			}
			else if((chuansong==1)||(jiting==1))
			{
				if(miao>0)
				{
					yi=2;er=11;san=11;si=11;wu=11;liu=11;qi=miao/10;ba=miao%10;
					if(jiting==0){P2=0XA0;P0=0X10;}			
				}
				if(miao==0){chuansong=0;P2=0XA0;P0=0X00;}
				
				if(jiting==1)
				{
					if(jiting_t<=250){P2=0X80;P0=0XF7;}
					else {P2=0X80;P0=0XFF;}
				}
			}
		}
		else if(AD_num>=204)
		{
			P2=0XA0;P0=0X40;  
			if(shan<50)
			{
				P2=0X80;P0=0XFB;shan++;
			}
			else if(shan>=50)
			{
				P2=0X80;P0=0XFF;shan++;if(shan>=100)shan=0;
			}
			yi=10;er=10;san=10;si=10;wu=10;liu=10;qi=10;ba=10;
//				Ultrasonic();
//				if(distance<=30){leibie=1;miao=miao_1;}
//				else {leibie=2;miao=miao_2;}
//				yi=1;er=11;san=11;si=distance%100/10;wu=distance%10;liu=11;qi=11;ba=leibie;
			kezai=0;
		}
		
		keyscan();
		display1(yi,er);
		display2(san,si);
		display3(wu,liu);
		display4(qi,ba);
	}
}


void keyscan()
{
	if(P30==0)
	{
		delayms(5);
		if(P30==0)
		{
			if(shezhi==1)
			{
				miao_1++;
				if(miao_1==11)miao_1=1;
			}
			else if(shezhi==2)
			{
				miao_2++;
				if(miao_2==11)miao_2=1;
			}
		}
		while(!P30);
	}
	else if(P31==0)
	{
		delayms(5);
		if(P31==0)
		{
			if(shezhi==0)
			{
				if((AD_num>0)&&(AD_num<51))
				{
					shezhi=1;
				}
			}
			else if(shezhi==1)
			{
				if((AD_num>0)&&(AD_num<51))
				{
					shezhi=2;
				}
			}
			else if(shezhi==2)
			{
				shezhi=0;
				EEPROM_Write(0X00,miao_1);delayms(2);
				EEPROM_Write(0X01,miao_2);delayms(2);
			}
		}
		while(!P31);
	}
	else if(P32==0)
	{
		delayms(5);
		if(P32==0)
		{
			if((jiting==0)&&(chuansong==1))
			{
				jiting=1;chuansong=0;
				P2=0XA0;P0=0X00;
			}
			else if(jiting==1)
			{
				jiting=0;chuansong=1;
			}
		}
		while(!P32);
	}
	else if(P33==0)
	{
		delayms(5);
		if(P33==0)
		{
			if((kezai==1)&&(chuansong==0))
			{
				chuansong=1;
			}
		}
		while(!P33);
	}
}

void Ultrasonic(void)
{
	if(s_flag) // 200毫秒更新一次数据 
	{
		s_flag = 0;
		send_wave();  //发送方波信号
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

	if(++intr == 200)
	{
		s_flag = 1;
		intr = 0;
  }
	
	if(chuansong==1)
	{
		if(++miao_t==500)
		{
			miao_t=0;
			if(miao>0)miao--;
		}
	}
	
	if(jiting==1)
	{
		jiting_t++;
		if(jiting_t==500)jiting_t=0;
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




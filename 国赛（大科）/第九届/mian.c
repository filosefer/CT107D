/*******************************************************************************  
* 文件名称：蓝桥杯单片机第九届决赛试题视频讲解配套程序
* 实验目的：多功能测量仪表      
* 程序说明：如需了解有关本程序配套视频讲解，及比赛经验分享\
*					 欢迎访问淘宝店铺网址：shop117015787.taobao.com
* 日期版本：2019-2
*******************************************************************************/
#include<STC15F2K60S2.H>
#include<IIC.H>
#include<ONEWIRE.H>

#define uchar unsigned char 
#define uint unsigned int	

uchar code tab[]={0XC0,0XF9,0XA4,0XB0,0X99,0X92,0X82,0XF8,0X80,0X90,\
									0X40,0X79,0X24,0X30,0X19,0X12,0X02,0X78,0X00,0X10,0XBF,0XFF,0XC1,0X8E,0XC6,0X89,0X8C};
																																						//	U		F			C		H			P
unsigned char Digbuf[8] = {0,1,2,3,4,5,6,7};
uchar S1_Flag=0,S2_Flag=0,S3_Flag=0,S4_Flag=0;
uchar Digcom=0;
uchar Key_temp=0xff,key=0xff;
uchar AD=0;
long Temper=0;

unsigned int zheng=0,fan=0,zheng_now=0,fan_now=0,time,freq;
uchar t_flag=0;

uchar Qiehuan=0,Huixian=0,Set=0;
bit Chack=0;
uint Chack_tt=0;
bit AD_R=0,Temper_R=0;
bit Save=0;
uchar Yuzhi=1; 
uchar LED=0XFF;
bit LED_Flag=0;

void delayms(int ms);
void allinit(void);
void keyscan(void);

void Timer0Init(void);
void Timer2Init(void);
void Dig_dispose(void);
void Dig_Huixian(void);
void Dig_Set(void);

void main(void)
{
	allinit();
	Timer0Init();
	Timer2Init();
	Yuzhi=EEPROM_read(0X06);delayms(5);
	while(1)
	{
		keyscan();
		
		if(AD_R==1)
		{
			AD_R=0;AD=AD_read(0X03);
			if((AD>Yuzhi)&&(LED_Flag==0)){LED_Flag=1;LED=LED&0X7F;P2=0X80;P0=LED;}
			else if((AD>Yuzhi)&&(LED_Flag==1)){LED_Flag=0;LED=LED|0X80;P2=0X80;P0=LED;}
			else if(AD<Yuzhi){LED=LED|0X80;P2=0X80;P0=LED;}
		}
		if(Temper_R==1){Temper_R=0;Temper=Tempget();}
		if(Chack==1)
		{
			Chack=0;
			time=(zheng_now+fan_now)*5;
			freq=1000000/time;	
			Dig_dispose();
		}		
//		delayms(200);
	}
}


void Dig_dispose(void)
{
	if(Qiehuan==0)
	{
		Digbuf[0]=21;Digbuf[1]=21;Digbuf[2]=21;Digbuf[3]=21;
		Digbuf[4]=21;Digbuf[5]=21;Digbuf[6]=21;Digbuf[7]=21;
	}
	if(Qiehuan==1)
	{
		Digbuf[0]=22;Digbuf[1]=21;Digbuf[2]=21;Digbuf[3]=21;
		Digbuf[4]=21;Digbuf[5]=21;Digbuf[6]=AD/10+10;Digbuf[7]=AD%10;
	}
	else if(Qiehuan==2)
	{
		Digbuf[0]=23;Digbuf[1]=21;Digbuf[2]=21;
		Digbuf[3]=freq/10000;	Digbuf[4]=freq%10000/1000;;Digbuf[5]=freq%1000/100;Digbuf[6]=freq%100/10;Digbuf[7]=freq%10;	
	}
	else if(Qiehuan==3)
	{
		Digbuf[0]=24;Digbuf[1]=21;Digbuf[2]=21;Digbuf[3]=21;
		Digbuf[4]=Temper/100000;Digbuf[5]=Temper%100000/10000+10;Digbuf[6]=Temper%10000/1000;Digbuf[7]=Temper%1000/100;			
	}
}


void Timer0Init(void)		//5微秒@12.000MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0xC4;		//设置定时初值
	TH0 = 0xFF;		//设置定时初值
	TF0 = 0;		//清除TF0标志
}

void Timer0_Rountine(void) interrupt 1
{
	if(P34==0)
	{
		zheng++;
		if(t_flag==1)
		{
			t_flag=0;
			fan_now=fan;
			fan=0;
		}
	}
	else if(P34==1)
	{
		fan++;
		if(t_flag==0)
		{
			t_flag=1;
			zheng_now=zheng;
			zheng=0;
		}
	}	
}

void Timer2Init(void)		//1毫秒@12MHz
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
	 
	if((Save==0)&&(Huixian==0)&&(Set==0))
	{
		Chack_tt++;
		if(Chack_tt==50){	AD_R=1;}//AD_R=0,Temper_R=0,Freq_R=0;
		else if(Chack_tt==100){	Temper_R=1;}
		else if(Chack_tt==150){	TR0 = 1; ET0 = 1; }
		else if(Chack_tt==200){	Chack_tt=0;Chack=1;	TR0 = 0; ET0 = 0; }	
	}
}

void keyscan(void)
{
	Key_temp=P3;                     // 1111 1110
	if( (Key_temp&0x0f) != 0x0f)			//	1111 1110 & 0000  1111  =0000 1110
 	{
		delayms(5);
		Key_temp=P3;
		if( (Key_temp&0x0f) != 0x0f)
		{
			key=Key_temp&0x0f;           //key = 0000 1110
			if((key==0x0d)&&(Set==1))
			{
				delayms(800);
				if((key==0x0d)&&(Set==1))
				{
					Yuzhi=Yuzhi+1;
					if(Yuzhi==51)Yuzhi=1;
					Dig_Set();
				}
			}
		}
	}
	
	if(((key&0x0f) !=0x0f)&&((Key_temp&0x0f) == 0x0f))
	{
		delayms(5);
		if( (Key_temp&0x0f) == 0x0f)
		{
			switch(key)
			{
				case 0x0e:
					if(Set==0){Set=1;Dig_Set();}
					else if(Set==1){Set=0;EEPROM_write(0X06,Yuzhi);delayms(5);}
					key=0xff;
				break;
				case 0x0d:
					if(Set==0)
					{
						if(Huixian==0){Huixian=1;TR0 = 0; ET0 = 0; Dig_Huixian();P2=0X80;P0=0XFF;}
						else if(Huixian==1){Huixian=0;}
					}
					key=0xff;
				break;
				case 0x0b:
					Save=1; TR0 = 0; ET0 = 0; 
					EEPROM_write(0X00,AD);delayms(5);								//	保存AD值
					EEPROM_write(0X01,Temper/10000);delayms(5);			//	保存温度值整数位，
					EEPROM_write(0X02,Temper%10000/100);delayms(5);	//	保存温度值整数位，
					EEPROM_write(0X03,freq/10000);delayms(5);				//	保存频率值万位，
					EEPROM_write(0X04,freq%10000/100);delayms(5);		//	保存频率值千、百位，
					EEPROM_write(0X05,freq%100);delayms(5);					//	保存频率值十、个位，
					Save=0;
					key=0xff;
				break;
				case 0x07:
					if(Qiehuan==0){Qiehuan=1;Chack_tt=0;P2=0X80;LED=LED|0X07;LED=LED&0XFB;P0=LED;if(Huixian==1)Dig_Huixian();}
					else if(Qiehuan==1){Qiehuan=2;Chack_tt=0;P2=0X80;LED=LED|0X07;LED=LED&0XFD;P0=LED;if(Huixian==1)Dig_Huixian();}
					else if(Qiehuan==2){Qiehuan=3;Chack_tt=0;P2=0X80;LED=LED|0X07;LED=LED&0XFE;P0=LED;if(Huixian==1)Dig_Huixian();}
					else if(Qiehuan==3){Qiehuan=0;P2=0X80;LED=LED|0X07;LED=LED&0XFF;if(Huixian==1)Dig_Huixian();}
					if((Huixian==1)||(Set==1)){P2=0X80;P0=0XFF;}
					key=0xff;
				break;
			}	
		}
	}
}

void Dig_Set(void)
{
		Digbuf[0]=26;Digbuf[1]=21;Digbuf[2]=21;Digbuf[3]=21;
		Digbuf[4]=21;Digbuf[5]=21;Digbuf[6]=Yuzhi/10+10;Digbuf[7]=Yuzhi%10;	
}

void Dig_Huixian(void)
{
	unsigned char E_AD;
	long E_Temper1,E_Temper2,E_Temper;
	unsigned int E_freq1,E_freq2,E_freq3,E_freq;
	if(Qiehuan==0)
	{
		Digbuf[0]=21;Digbuf[1]=21;Digbuf[2]=21;Digbuf[3]=21;
		Digbuf[4]=21;Digbuf[5]=21;Digbuf[6]=21;Digbuf[7]=21;
	}
	if(Qiehuan==1)
	{
		E_AD=EEPROM_read(0X00);
		Digbuf[0]=25;Digbuf[1]=22;Digbuf[2]=21;Digbuf[3]=21;
		Digbuf[4]=21;Digbuf[5]=21;Digbuf[6]=E_AD/10+10;Digbuf[7]=E_AD%10;
	}
	else if(Qiehuan==2)
	{
		E_freq1=EEPROM_read(0X03);delayms(5);E_freq2=EEPROM_read(0X04);delayms(5);E_freq3=EEPROM_read(0X05);
		E_freq=E_freq1*10000+E_freq2*100+E_freq3;
		Digbuf[0]=25;Digbuf[1]=23;Digbuf[2]=21;
		Digbuf[3]=E_freq/10000;	Digbuf[4]=E_freq%10000/1000;;Digbuf[5]=E_freq%1000/100;Digbuf[6]=E_freq%100/10;Digbuf[7]=E_freq%10;				
	}
	else if(Qiehuan==3)
	{
		E_Temper1=EEPROM_read(0X01);delayms(5);E_Temper2=EEPROM_read(0X02);
		E_Temper=E_Temper1*10000+E_Temper2*100;
		Digbuf[0]=25;Digbuf[1]=24;Digbuf[2]=21;Digbuf[3]=21;
		Digbuf[4]=E_Temper/100000;Digbuf[5]=E_Temper%100000/10000+10;Digbuf[6]=E_Temper%10000/1000;Digbuf[7]=E_Temper%1000/100;	
	}	
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

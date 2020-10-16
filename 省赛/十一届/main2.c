#include<reg52.h>
#include<iic.h>
#define uchar unsigned char
#define uint unsigned int 
sfr P4=0xc0;
sbit P42=P4^2;		
sbit P44=P4^4;

sbit L1=P0^0;
sbit L2=P0^1;
sbit L3=P0^2;

uchar code duanxuan[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xc1,0x8c,0xc8};		 // 0-9 U P N			   1100 1000
uchar moshi=1,jishu,value,baojing,chufa,ADC,wuxiao,yujing;
uint dianya,canshu=300,time,time1;
																									 
void delay(uint i )
{
  while(i--);
}

void HC573(uchar num)
{
	 switch(num)
	  {
	  	 case(4): P2=(P2&0X1f)	| 0X80; break;
		 case(5): P2=(P2&0X1f)	| 0Xa0; break;
		 case(6): P2=(P2&0X1f)	| 0Xc0; break;
		 case(7): P2=(P2&0X1f)	| 0Xe0; break;
		 case(0): P2=(P2&0X1f)	| 0X00; break;
	  }
}

void init()
{
	 HC573(4);
	 P0=0XFF;
	 HC573(5);
	 P0=0X00;
	 canshu=Read_24C02(0x00)*10;
	 
	 EA=1;
	 ET0=1;
	 TR0=1;
	 TMOD = 0X01;
	 TH0=(65536-5000)/255;
	 TL0=(65536-5000)%255;
}

void t0serve() interrupt 1
{
	TH0=(65536-5000)/255;
	TL0=(65536-5000)%255;
	if((yujing==1)&&(baojing==0))time++;					  //报警后计时不会增加		
	if(time == 1000)
	{
		baojing=1;
		time=0;
	}

	time1++;
	if(time1== 100)
	{
	   ADC=1;
	   time1=0;
	}
}

void SMG(uchar pos,uchar value)
{
	 HC573(6);
	 P0=0X01<<pos;
	 HC573(7);
	 P0=duanxuan[value];
	 delay(100);
	 P0=0Xff;
}					  

void SMGX(uchar pos,uchar value)
{
	 HC573(6);
	 P0=0X01<<pos;
	 HC573(7);
	 P0=duanxuan[value] & 0x7f;
	 delay(100);
	 P0=0Xff;
}

void display()
{
	   if(moshi==1)
	   {
		 SMG(0,10);
	
		 SMGX(5,dianya/100);
		 SMG(6,dianya%100/10);
		 SMG(7,dianya%10);
	   }
	
	    if(moshi==2)
	   {
		 SMG(0,11);
	
		 SMGX(5,canshu/100);
		 SMG(6,canshu%100/10);
		 SMG(7,canshu%10);
	   }
	
	   if(moshi==3)
	   {
		 SMG(0,12);
	
	
		 SMG(6,jishu/10);
		 SMG(7,jishu%10);
	   }
}

void key()											   //键盘扫描
{
	P3=0X0F; P42=0; P44=0;
	 if(P3!=0X0F)
	 {
	   delay(1000);
		if(P3!=0X0F)
		 {
		 	P3=0X0F; P42=0; P44=0;
			switch(P3)
			  {
			  	  case(0x0e): value=1; break;
				  case(0x0d): value=5; break;
				  case(0x0b): value=9; break;
				  case(0x07): value=13; break;
			  }

			 P3=0Xf0; P42=1; P44=1;
			 if(P44==0)	value=value; 
			 if(P42==0)	value=value+1;
			 if(P3==0xd0)	value=value+2;
			 if(P3==0xe0)	value=value+3;

			 while((P44==0)||(P42==0)||(P3==0xd0)||(P3==0xe0));
		 }
	 }

	 if(value==15)
	 {
		  value=0;
	 	  moshi++;
		  if(moshi==3)	    						   //进入计数模式时写入参数数据
		  {
		  	 Write_24C02(0x00, canshu/10);
			 delay(1000);
		  }
		  if(moshi==4)moshi=1;
	 }

	 if(value==11)
	 {
		value=0;
	 	if(moshi==3)jishu=0;

		else 
		{
		   wuxiao++;
		}
	 }

	 if(value==16)
	 {
		value=0;
	 	if(moshi==2)
		{
		   canshu=canshu+50;
		   if(canshu>=500)canshu=0;
		   wuxiao=0;
		}

		else 
		{
		   wuxiao++;
		}
	 }

	 if(value==12)
	 {
		value=0;
	 	if(moshi==2)
		{
		   if(canshu==0)canshu=550;
		   canshu=canshu-50;
		   if(canshu<50)canshu=0;		  
		   wuxiao=0;
		}

		else 
		{
		   wuxiao++;		  
		}
	 }
}

void LED()										   //灯光显示函数
{
	  HC573(0);
	  if(wuxiao>=3)								   //无效三次以后L3点亮
	  {
	  	 
		 L3=0;
	  }											   //无效清零以后L3熄灭
	  else if(wuxiao==0)
	  {
	  	
		 L3=1;
	  }

	  if(jishu%2==1)							  //计数为奇数L2点亮，偶数熄灭
	  {
	  	 
		 L2=0;
	  }
	  else
	  {
	  	
		 L2=1;
	  }


	  if(baojing==1)							 //计时五秒后L1点亮，电压大于参数后熄灭
	  {
	  	 
		 L1=0;
	  }
	  else
	  {
	  	
		 L1=1;
	  }
	  HC573(4);
}

void main()
{
	init();
   while(1)
   {
		 if(ADC==1)								//0.5 秒采集一次电压值
		 {
	   	   dianya=Read_rb2();	
		   ADC=0;
		 }

		 if(dianya>canshu)chufa=1;				//电压值大于参数值然后下降至参数值才会触发计数
		 if((chufa==1)&&(dianya<=canshu))
		 {
		 	jishu++;
			chufa=0;
		 }

		 if(dianya<canshu)yujing=1;				//电压小于参数开始计时，否则不计时，L1熄灭
		 else if(dianya>=canshu)
			{
				yujing=0;
				baojing=0;
			}
	
		 

		 display();
		 key();
		 LED();
   }
}

/*-------------------------------------------------
//第四届超声波报警实时时钟
超声波的读数没有写，其余部分已写完
程序在小灯闪烁时，偶尔有乱跳的情况
2020.3.1
---------------------------------------------------
*/
#include<STC15F2K60S2.H>
#include<ds1302.h>
#include<iic.h>
sbit s7=P3^0;
sbit s6=P3^1;
sbit s5=P3^2;
sbit s4=P3^3;
char temp_h,temp_m;
unsigned char eeprom;
unsigned char distance=25;
unsigned char flag_yemian,flag_timeset,flag_temp,flag_disset,flag_led;
unsigned char time0_t;
unsigned char time_init[]={59,50,11,0,0,0,0};
unsigned char time_read[7];
unsigned char seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xbf,0xff};
unsigned char yi,er,san,si,wu,liu,qi,ba;
void delayms(int ms);
void display_12(yi,er);
void display_34(san,si);
void display_56(wu,liu);
void display_78(qi,ba);
void allinit();
void ds1302_init();
void ds1302_read();
void Timer0Init(void);
void key_scan();
void eeprom_write(unsigned char add,unsigned char dat);
unsigned char eeprom_read(unsigned char add);
void main(){
	allinit();
	Timer0Init();EA=1;ET0=1;
	ds1302_init();
	eeprom=eeprom_read(0x01);
	yi=10;er=10;san=10;si=10;wu=10;liu=10;qi=10;ba=10;
	while(1){
		//时钟读取
		ds1302_read();
		//时钟状态显示界面
		if(flag_yemian==0){
			if(flag_temp==0){
				//判断边界
				if(temp_h+time_read[2]<0) temp_h=temp_h+24;
				time_read[2]=(temp_h+time_read[2])%24;
				if(temp_m+time_read[1]<0) temp_h=temp_h+60;
				time_read[1]=(temp_m+time_read[1])%60;
				yi=time_read[2]/10;er=time_read[2]%10;san=10;si=time_read[1]/10;
				wu=time_read[1]%10;liu=10;qi=time_read[0]/10;ba=time_read[0]%10;
			}
			//设置时
			else if(flag_temp==1){
				time_read[2]=(temp_h+time_read[2])%24;
				time_read[1]=(temp_m+time_read[1])%60;
				yi=11;er=11;san=10;si=time_read[1]/10;
				wu=time_read[1]%10;liu=10;qi=time_read[0]/10;ba=time_read[0]%10;
			}
			//设置分
			else if(flag_temp==2){
				time_read[2]=(temp_h+time_read[2])%24;
				time_read[1]=(temp_m+time_read[1])%60;
				yi=time_read[2]/10;er=time_read[2]%10;san=10;si=11;
				wu=11;liu=10;qi=time_read[0]/10;ba=time_read[0]%10;
			}
		}
		//超声波状态界面
		else if(flag_yemian==1){
			if(flag_disset==0){
				yi=10;er=10;san=10;si=10;wu=10;liu=distance/100;qi=distance%100/10;ba=distance%10;
			}
			else if(flag_disset==1){
				yi=10;er=10;san=10;si=10;wu=10;liu=eeprom/100;qi=eeprom%100/10;ba=eeprom%10;
			}
		}
		//蜂鸣器判断
		if(distance<eeprom){
			P2=0XA0;P0=0X40;P2=P2&0X2F;
		}
		else{P2=0XA0;P0=0X00;}
		//小灯闪烁判断
		if(distance<(1.2*eeprom)){
			flag_led=1;
		}
		else flag_led=0;
		key_scan();
		display_12(yi,er);
		display_34(san,si);
		display_56(wu,liu);
		display_78(qi,ba);
	}
}	
//按键
void key_scan(){
	//界面
	if(s7==0){
		delayms(5);
		if(s7==0){
			flag_yemian++;
			if(flag_yemian==2){
				flag_yemian=0;
				flag_timeset=0;
			}
		}
		while(!s7);
	}
	//设置
	else if(s6==0){
		delayms(5);
		if(s6==0){
			if(flag_yemian==0){
				flag_timeset++;
				if(flag_timeset==3)
					flag_timeset=0;
			}
			else if(flag_yemian==1){
				flag_disset++;
				if(flag_disset==2){
					flag_disset=0;
					eeprom_write(0x01,eeprom);
				}
			}
		}
		while(!s6);
	}
	//加
	else if(s5==0){
		delayms(5);
		if(s5==0){
			if(flag_yemian==0){
				if(flag_timeset==1)
					temp_h++;
				if(flag_timeset==2)
					temp_m++;
			}
			else if(flag_yemian==1){
				if(flag_disset==1) eeprom++;
			}
		}
		while(!s5);
	}
	//减
	else if(s4==0){
		delayms(5);
		if(s4==0){
			if(flag_yemian==0){
				if(flag_timeset==1)
					temp_h--;
				if(flag_timeset==2)
					temp_m--;
			}
			else if(flag_yemian==1){
				if(flag_disset==1) eeprom--;
			}
		}
		while(!s4);
	}
}
//eeprom读取
unsigned char eeprom_read(unsigned char add){
	unsigned char temp;
	IIC_Start();
	IIC_SendByte(0xa0);
	IIC_WaitAck();
	IIC_SendByte(add);
	IIC_WaitAck();
	IIC_Stop();
	IIC_Start();
	IIC_SendByte(0xa1);
	IIC_WaitAck();
	temp=IIC_RecByte();
	IIC_Stop();
	return temp;	
}
//eeprom写入
void eeprom_write(unsigned char add,unsigned char dat){
	IIC_Start();
	IIC_SendByte(0xa0);
	IIC_WaitAck();
	IIC_SendByte(add);
	IIC_WaitAck();
	IIC_SendByte(dat);
	IIC_WaitAck();
	IIC_Stop();
}
void Timer0Init(void)	
{
	AUXR |= 0x80;	
	TMOD &= 0xF0;		
	TL0 = 0x00;		
	TH0 = 0x28;	
	TF0 = 0;		
	TR0 = 1;	
}
//定时器0用于控制时钟闪烁和小灯
void Time0() interrupt 1{
	time0_t++;
	if(flag_yemian==0){
		if(time0_t==100){
			flag_temp=flag_timeset;
		}
		else if(time0_t==200){
			flag_temp=0;
			time0_t=0;
		}
	}
	if(flag_led==1){
		if(time0_t<100){
			P2=0X80;P0=0XFE;P2=P2&0X2F;
		}
		else if(time0_t<200){
			P2=0X80;P0=0XFF;P2=P2&0X2F;
			if(time0_t==200)
				time0_t=0;
		}
	}
}
//时钟初始化函数
void ds1302_init(){
	unsigned char i,add=0x80;
	Write_Ds1302_Byte(0x8e,0x00);
	for(i=0;i<7;i++){
		Write_Ds1302_Byte(add,time_init[i]);
		add=add+2;
	}
	Write_Ds1302_Byte(add,0x80);
}
//时钟读取函数
void ds1302_read(){
	unsigned char i,add=0x81;
	Write_Ds1302_Byte(0x8e,0x00);
	for(i=0;i<7;i++){
		time_read[i]=Read_Ds1302_Byte(add);
		add=add+2;
	}
	Write_Ds1302_Byte(add,0x80);
}
//初始化
void allinit(){
	P2=0XA0;P0=0X40;
	P2=0XC0;P0=0Xff;P2=0XE0;P0=0X00;
	P2=0X80;P0=0XFE;
	delayms(1000);
	P2=0XA0;P0=0X00;
	P2=0XC0;P0=0Xff;P2=0XE0;P0=0XFF;
	P2=0X80;P0=0XFF;
}
void delayms(int ms){
	int q,w;
	for(q=0;q<ms;q++)
		for(w=0;w<845;w++);
}
//数码管
void display_12(yi,er){
	P2=0XC0;P0=0X01;P2=0XE0;P0=seg[yi];delayms(1);
	P2=0XC0;P0=0X02;P2=0XE0;P0=seg[er];delayms(1);
}
void display_34(san,si){
	P2=0XC0;P0=0X04;P2=0XE0;P0=seg[san];delayms(1);
	P2=0XC0;P0=0X08;P2=0XE0;P0=seg[si];delayms(1);
}
void display_56(wu,liu){
	P2=0XC0;P0=0X10;P2=0XE0;P0=seg[wu];delayms(1);
	P2=0XC0;P0=0X20;P2=0XE0;P0=seg[liu];delayms(1);
}
void display_78(qi,ba){
	P2=0XC0;P0=0X40;P2=0XE0;P0=seg[qi];delayms(1);
	P2=0XC0;P0=0X80;P2=0XE0;P0=seg[ba];delayms(1);
	P2=0XC0;P0=0XFF;P2=0XE0;P0=0XFF;delayms(1);
}

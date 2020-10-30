/*
--------------------------------
第七届频率电压采集
频率没写，其余部分写完。
2020.3.8
--------------------------------
*/
#include<STC15F2K60S2.H>
#include<ds1302.h>
#include<iic.h>
char s11_count_h,s11_count_m,s11_count_s,count_h,count_m;
unsigned int time0_t,dianya,dianya_h,dianya_m,dianya_temp;
unsigned char flag_yemian,flag_time0,s4_count,dianya_count;
unsigned char seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xbf,0xff};
unsigned char yi,er,san,si,wu,liu,qi,ba;
unsigned char time_init[]={55,59,23,0,0,0,0};
unsigned char time_read[7];
void delayms(int ms);
void display_12(yi,er);
void display_34(san,si);
void display_56(wu,liu);
void display_78(qi,ba);
void allinit();
void key_scan();
void ds1302_init();
void ds1302_read();
void Timer0Init(void);
unsigned char iic_rb2_read();
void eeprom_write(unsigned char add,unsigned char dat);
unsigned char eeprom_read(unsigned char add);
void dianya_pd();
void main(){
	allinit();
	ds1302_init();
	Timer0Init();EA=1;ET0=1;
	while(1){
		//eeprom上下限读取
		dianya_h=eeprom_read(0x01);dianya_h=dianya_h+count_h;
		dianya_m=eeprom_read(0x02);dianya_m=dianya_m+count_m;
		//电压读取Rb2及转换
		dianya=iic_rb2_read()*1.96;
		//时钟读取及设置
		ds1302_read();
		time_read[2]=(time_read[2]+s11_count_h)%24;
		time_read[1]=(time_read[1]+s11_count_m)%60;
		time_read[0]=(time_read[0]+s11_count_s)%60;
		//时钟显示界面
		if(flag_yemian==0){
			//时闪烁
			if(s4_count==1 & flag_time0==1){
				yi=11;er=11;san=10;si=time_read[1]/10;wu=time_read[1]%10;liu=10;qi=time_read[0]/10;ba=time_read[0]%10;
			}
			//分闪烁
			else if(s4_count==2& flag_time0==1){
				yi=time_read[2]/10;er=time_read[2]%10;san=10;si=11;wu=11;liu=10;qi=time_read[0]/10;ba=time_read[0]%10;
			}
			//秒闪烁
			else if(s4_count==3& flag_time0==1){
				yi=time_read[2]/10;er=time_read[2]%10;san=10;si=time_read[1]/10;wu=time_read[1]%10;liu=10;qi=11;ba=11;
			}
			//正常显示
			else {
				yi=time_read[2]/10;er=time_read[2]%10;san=10;si=time_read[1]/10;wu=time_read[1]%10;liu=10;qi=time_read[0]/10;ba=time_read[0]%10;
			}
		}
		//电压测量界面
		else if(flag_yemian==1){
			//上限闪烁
			if(s4_count==1&flag_time0==1){
				yi=11;er=11;san=11;si=11;wu=dianya_m/10;liu=dianya_m%10;qi=0;ba=0;
			}
			//下限闪烁
			else if(s4_count==2&flag_time0==1){
				yi=dianya_h/10;er=dianya_h%10;san=0;si=0;wu=11;liu=11;qi=11;ba=11;
			}
			//显示当前电压
			else if(s4_count==0){
				yi=10;er=1;san=10;si=11;wu=dianya/100;liu=dianya%100/10;qi=dianya%10;ba=0;
			}
			//显示正常上下限
			else{
				yi=dianya_h/10;er=dianya_h%10;san=0;si=0;wu=dianya_m/10;liu=dianya_m%10;qi=0;ba=0;
			}
		}
		//频率测量界面
		else if(flag_yemian==2){
			yi=10;er=2;san=10;si=0;wu=1;liu=0;qi=0;ba=0;
		}
		//查询界面
		else if(flag_yemian==3){
			//显示类型
			if(s4_count==0){
				yi=11;er=11;san=11;si=11;wu=11;liu=11;qi=0;ba=eeprom_read(0x03);
			}
			//显示波动时间
			else if(s4_count==1){
				yi=eeprom_read(0x04)/10;er=eeprom_read(0x04)%10;san=10;si=eeprom_read(0x05)/10;wu=eeprom_read(0x05)%10;
				liu=10;qi=eeprom_read(0x06)/10;ba=eeprom_read(0x06)%10;
			}
		}
		//每50进行一次判断，电压是否记录
		if(dianya_count==50){
			dianya_count=0;
			dianya_pd();
			dianya_temp=dianya;
		}
		dianya_count++;
		key_scan();
		display_12(yi,er);
		display_34(san,si);
		display_56(wu,liu);
		display_78(qi,ba);
	}
}
//矩阵按键
void key_scan(){
	unsigned char temp;
	P44=0;P42=1;P3=0XFF;
	temp=P3;temp=temp&0x0f;
	if(temp!=0x0f){
		delayms(5);
		temp=P3;temp=temp&0x0f;
		if(temp!=0x0f){
			temp=P3;
			switch(temp){
				//切换时钟测量
				case 0xfe:
					if(flag_yemian!=0){
						s4_count=0;
						flag_yemian=0;
					}
					//完成时钟设置后将时钟初始化
					else if(flag_yemian==0){
						s4_count=0;
						flag_yemian=0;
						time_init[0]=time_read[0];
						time_init[1]=time_read[1];
						time_init[2]=time_read[2];
						ds1302_init();
						s11_count_h=0;s11_count_m=0;s11_count_s=0;
					}
					break;
				//切换电压测量
				case 0xfd:
					flag_yemian=1;
					s4_count=0;
					break;
				//切换频率测量
				case 0xfb:
					s4_count=0;
					flag_yemian=2;
					break;
				//功能设置
				case 0xf7:
					//时钟界面设置
					if(flag_yemian==0){
						s4_count++;
						if(s4_count==4)
							s4_count=1;
					}
					//电压页面设置
					else if(flag_yemian==1){
						s4_count++;
						if(s4_count==3){
							s4_count=1;
							eeprom_write(0x01,dianya_h+count_m);delayms(10);
							eeprom_write(0x02,dianya_m+count_m);delayms(10);
							count_h=0;count_m=0;
						}
					}
					//记录页面设置
					else if(flag_yemian==3){
						s4_count++;
						if(s4_count==2){
							s4_count=0;
						}
					}
					
					break;
			}
			while(temp!=0x0f){
			temp=P3;temp=temp&0x0f;
			}
		}
	}
	P44=1;P42=0;P3=0XFF;
	temp=P3;temp=temp&0x0f;
	if(temp!=0x0f){
		temp=P3;temp=temp&0x0f;
		if(temp!=0x0f){
			temp=P3;
			switch(temp){
				//加
				case 0xfe:
					if(flag_yemian==0){
						if(s4_count==1)s11_count_h++;
						else if(s4_count==2)s11_count_m++;
						else if(s4_count==3)s11_count_s++;
					}
					else if(flag_yemian==1){
						if(s4_count==1)count_h=count_h+5;
						else if(s4_count==2)count_m=count_m+5;
					}
					break;
				//减
				case 0xfd:
					if(flag_yemian==0){
						if(s4_count==1)s11_count_h--;
						else if(s4_count==2)s11_count_m--;
						else if(s4_count==3)s11_count_s--;
						if(s11_count_h<0)s11_count_h=s11_count_h+24;
						if(s11_count_m<0)s11_count_m=s11_count_m+60;
						if(s11_count_s<0)s11_count_s=s11_count_s+60;
					}
					else if(flag_yemian==1){
						if(s4_count==1)count_h=count_h-5;
						else if(s4_count==2)count_m=count_m-5;
					}
					break;
				//查询功能
				case 0xfb:
					s4_count=0;
					flag_yemian=3;
					break;
			}
			while(temp!=0x0f){
			temp=P3;temp=temp&0x0f;
			}
		}
	}
}
//电压判断
void dianya_pd(){
	if(dianya_temp>dianya_m*10){
		if(dianya_temp<dianya_h*10){
			//当从正常进入高电压时，记录
			if(dianya>dianya_h*10){
				eeprom_write(0x03,1);delayms(5);
				eeprom_write(0x04,time_read[2]);delayms(5);
				eeprom_write(0x05,time_read[1]);delayms(5);
				eeprom_write(0x06,time_read[0]);delayms(5);
			}
			//当从正常进入低电压时，记录
			else if(dianya<dianya_m*10){
				eeprom_write(0x03,0);delayms(5);
				eeprom_write(0x04,time_read[2]);delayms(5);
				eeprom_write(0x05,time_read[1]);delayms(5);
				eeprom_write(0x06,time_read[0]);delayms(5);
			}
		}
	}
}
//eeprom获取
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
//rb2获取
unsigned char iic_rb2_read(){
	unsigned char temp;
	IIC_Start();
	IIC_SendByte(0x90);
	IIC_WaitAck();
	IIC_SendByte(0x03);
	IIC_WaitAck();
	IIC_Stop();
	IIC_Start();
	IIC_SendByte(0x91);
	IIC_WaitAck();
	temp=IIC_RecByte();
	IIC_Stop();
	return temp;
}
//定时器0功能
void time0() interrupt 1{
	time0_t++;
	if(time0_t==200){
		flag_time0=1;
	}
	else if(time0_t==400){
		time0_t=0;
		flag_time0=0;
	}
}
//定时器0初始化
void Timer0Init(void)		
{
	AUXR |= 0x80;		
	TMOD &= 0xF0;		
	TL0 = 0x00;	
	TH0 = 0x28;		
	TF0 = 0;		
	TR0 = 1;		
}
//时钟初始化
void ds1302_init(){
	unsigned char i,add=0x80;
	Write_Ds1302_Byte(0x8e,0x00);
	for(i=0;i<7;i++){
		Write_Ds1302_Byte(add,time_init[i]);
		add=add+2;
	}
	Write_Ds1302_Byte(add,0x80);
}
//时钟读取
void ds1302_read(){
	unsigned char i,add=0x81;
	Write_Ds1302_Byte(0x8e,0x00);
	for(i=0;i<7;i++){
		time_read[i]=Read_Ds1302_Byte(add);
		add=add+2;
	}
	Write_Ds1302_Byte(add,0x80);
}
//上电初始化
void allinit(){
	P2=0XA0;P0=0X00;
	P2=0X80;P0=0XFF;
	P2=0XC0;P0=0XFF;P2=0XE0;P0=0XFF;
}
//延时
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

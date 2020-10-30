/*
-----------------------------------
第五届国赛多功能事件记录器
2020.3.2
串口部分未完成
其余部分完成EEPROM部分自动存储完成
问题：
IIC通信中rb2和光敏电阻的获取是反的，03得到了光敏电阻，01得到了rb2阻值
-----------------------------------
*/
#include<STC15F2K60S2.H>
#include<ds1302.h>
#include<onewire.h>
#include<iic.h>
sbit s5=P3^2;
sbit s4=P3^3;
sbit led1=P0^0;
sbit led2=P0^1;
sbit led3=P0^2;
sbit led4=P0^3;
int shidu;
bit jiange;
unsigned char eep[5][6];
unsigned char time0_t,close_t,close_temp,read_temp,eep_count;
unsigned char flag_yemian,flag_mode,flag_close;
unsigned char time_init[]={55,59,23,0,0,0,0};
unsigned char time_read[7];
unsigned char wendu,dianya,urdat,gm,eep_temp;
unsigned char seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xbf,0xff,0xc6,0x89};
unsigned char yi,er,san,si,wu,liu,qi,ba;
void delayms(int ms);
void allinit();
void display_12(yi,er);
void display_34(san,si);
void display_56(wu,liu);
void display_78(qi,ba);
void DS1302_READ();
void DS1302_INIT();
unsigned char onewire_read();
unsigned char iic_rb2(unsigned char add);
void Timer0Init(void);
void uart_init();
void sendstring(unsigned char *str);
void uart_working();
void key_scan();
unsigned char eeprom_read(unsigned char add);
void eeprom_write(unsigned char add,unsigned char dat);
void eep_input();
//void eep_output();
void main(){
	allinit();
	DS1302_INIT();
	uart_init();
	Timer0Init(); EA=1;ET0=1;ES=1;
	yi=10;er=10;san=10;si=10;wu=10;liu=10;qi=10;ba=10;
	sendstring("hello\r\n");
	close_temp=iic_rb2(0x03);
	while(1){
		//eep_output();
		uart_working();
		DS1302_READ();
		wendu=onewire_read();
		gm=iic_rb2(0x03);
		//对靠近物体进行判断
		if(gm<(close_temp-10)){
			flag_close=1;
			P2=0X80;led3=0;
		}
		else if(gm>(close_temp-10)){
			if(flag_close==1){
				read_temp=close_t;
				if(flag_mode==1){
					//eep保存
					eep_input();
					eep_count++;
					if(eep_count==5)eep_count=0;
				}
			}
			flag_close=0;
			close_t=0;
			P2=0X80;led3=1;
		}
		dianya=iic_rb2(0x01);
		shidu=3.92*dianya;
		//界面一温度湿度
		if(flag_yemian==0){
			yi=wendu/10;er=wendu%10;san=12;si=eep_temp/10;wu=eep_temp%10;liu=shidu/100;qi=shidu%100/10;ba=13;
		}
		//界面二时钟显示
		else if(flag_yemian==1){
			if(jiange==0){
				yi=time_read[2]/10;er=time_read[2]%10;san=10;si=time_read[1]/10;wu=time_read[1]%10;liu=10;qi=time_read[0]/10;ba=time_read[0]%10;
			}
			else if(jiange==1){
				yi=time_read[2]/10;er=time_read[2]%10;san=11;si=time_read[1]/10;wu=time_read[1]%10;liu=11;qi=time_read[0]/10;ba=time_read[0]%10;
			}
		}
		//界面三靠近物体时间显示
		else if(flag_yemian==2){
			yi=11;er=11;san=11;si=10;wu=close_t/1000;liu=read_temp%1000/100;qi=read_temp%100/10;ba=read_temp%10;
		}
		//自动传输模式
		if(flag_mode==0){
			P2=0X80;led1=0;led2=1;
		}
		//自动记录模式
		else if(flag_mode==1){
			P2=0X80;led2=0;led1=1;
		}
		//按键
		key_scan();
		//数码管
		display_12(yi,er);
		display_34(san,si);
		display_56(wu,liu);
		display_78(qi,ba);
	}
}
//按键
void key_scan(){
	//模式转换
	if(s4==0){
		delayms(5);
		if(s4==0){
			flag_mode++;
			if(flag_mode==2)
				flag_mode=0;
		}
		while(!s4);
	}
	//界面切换
	else if(s5==0){
		delayms(5);
		if(s5==0){
			flag_yemian++;
			if(flag_yemian==3)
				flag_yemian=0;
		}
		while(!s5);
	}
}
//eep函数写入
void eep_input(){
	if(eep_count==0){
		eeprom_write(0x01,wendu);eeprom_write(0x02,shidu);eeprom_write(0x03,time_read[2]);
		eeprom_write(0x04,time_read[1]);delayms(5);eeprom_write(0x05,time_read[0]);delayms(5);eeprom_write(0x06,read_temp);delayms(5);}
	else if(eep_count==1){
		eeprom_write(0x11,wendu);eeprom_write(0x12,shidu);eeprom_write(0x13,time_read[2]);
		eeprom_write(0x14,time_read[1]);delayms(5);eeprom_write(0x15,time_read[0]);delayms(5);eeprom_write(0x16,read_temp);delayms(5);}
	else if(eep_count==2){
		eeprom_write(0x21,wendu);eeprom_write(0x22,shidu);eeprom_write(0x23,time_read[2]);
		eeprom_write(0x24,time_read[1]);delayms(5);eeprom_write(0x25,time_read[0]);delayms(5);eeprom_write(0x26,read_temp);delayms(5);}
	else if(eep_count==3){
		eeprom_write(0x31,wendu);eeprom_write(0x32,shidu);eeprom_write(0x33,time_read[2]);
		eeprom_write(0x34,time_read[1]);delayms(5);eeprom_write(0x35,time_read[0]);delayms(5);eeprom_write(0x36,read_temp);delayms(5);}
	else if(eep_count==4){
		eeprom_write(0x41,wendu);eeprom_write(0x42,shidu);eeprom_write(0x43,time_read[2]);
		eeprom_write(0x44,time_read[1]);delayms(5);eeprom_write(0x45,time_read[0]);delayms(5);eeprom_write(0x46,read_temp);delayms(5);}
}
//eep函数读取
//void eep_output(){
//	eep[0][0]=eeprom_read(0x01);eep[0][1]=eeprom_read(0x02);eep[0][2]=eeprom_read(0x03);eep[0][3]=eeprom_read(0x04);
//	eep[0][4]=eeprom_read(0x05);eep[0][5]=eeprom_read(0x06);
//	eep[1][0]=eeprom_read(0x11);eep[1][1]=eeprom_read(0x12);eep[0][2]=eeprom_read(0x13);eep[1][3]=eeprom_read(0x14);
//	eep[1][4]=eeprom_read(0x15);eep[1][5]=eeprom_read(0x16);
//	eep[2][0]=eeprom_read(0x21);eep[2][1]=eeprom_read(0x22);eep[2][2]=eeprom_read(0x23);eep[2][3]=eeprom_read(0x24);
//	eep[2][4]=eeprom_read(0x25);eep[2][5]=eeprom_read(0x26);
//	eep[3][0]=eeprom_read(0x31);eep[3][1]=eeprom_read(0x32);eep[3][2]=eeprom_read(0x33);eep[3][3]=eeprom_read(0x34);
//	eep[3][4]=eeprom_read(0x35);eep[3][5]=eeprom_read(0x36);
//	eep[4][0]=eeprom_read(0x41);eep[4][1]=eeprom_read(0x42);eep[4][2]=eeprom_read(0x43);eep[4][3]=eeprom_read(0x44);
//	eep[4][4]=eeprom_read(0x45);eep[4][5]=eeprom_read(0x46);
//}
//eep读取
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
//eep写入
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
//uart初始化
void uart_init(){
	TMOD=0X20;
	AUXR&=0x40;
	TH1=0XFD;TL1=0XFD;
	TR1=1;SCON=0X50;
}
void uart_time1() interrupt 4{
	if(RI==1){
		RI=0;
		urdat=SBUF;
	}
}
//接收字符工作
void uart_working(){
	if(urdat!=0){
		switch(urdat){
			case 0x0A:sendstring("love\r\n");urdat=0;break;
		}
	}
}
//发送字符
void sendbyte(unsigned char dat){
	SBUF=dat;
	while(TI==0);
	TI=0;
}
//发送字串
void sendstring(unsigned char *str){
	while(*str !='\0'){
	sendbyte(*str++);
	}
}
//定时器0控制间隔符闪烁
void time0() interrupt 1{
	time0_t++;
	if(flag_yemian==1){
		if(time0_t==100){
			jiange=1;
		}
		else if(time0_t>200){
			jiange=0;
			time0_t=0;
		}
	}
	else {
		if(time0_t==200){
			if(flag_close==1)
				close_t++;
			time0_t=0;
		}
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
//RB2电压获取
unsigned char iic_rb2(unsigned char add){
	unsigned char temp;
	IIC_Start();
	IIC_SendByte(0x90);
	IIC_WaitAck();
	IIC_SendByte(add);
	IIC_WaitAck();
	IIC_Stop();
	IIC_Start();
	IIC_SendByte(0x91);
	IIC_WaitAck();
	temp=IIC_RecByte();
	IIC_Stop();
	return temp;
}	
//温度获取
unsigned char onewire_read(){
	unsigned char temp,high,low;
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0x44);
	delayms(1);
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0xbe);
	low=Read_DS18B20();
	high=Read_DS18B20();
	init_ds18b20();
	temp=high<<4;
	temp=temp|(low>>4);
	return temp;
}
//时钟初始化
void DS1302_INIT(){
	unsigned char i,add=0x80;
	Write_Ds1302_Byte(0x8e,0x00);
	for(i=0;i<7;i++){
		Write_Ds1302_Byte(add,time_init[i]);
		add=add+2;
	}
	Write_Ds1302_Byte(add,0x80);
}
//时钟获取
void DS1302_READ(){
	unsigned char i,add=0x81;
	Write_Ds1302_Byte(0x8e,0x00);
	for(i=0;i<7;i++){
		time_read[i]=Read_Ds1302_Byte(add);
		add=add+2;
	}
	Write_Ds1302_Byte(add,0x80);
}
//延时函数
void delayms(int ms){
	int q,w;
	for(q=0;q<ms;q++)
		for(w=0;w<845;w++);
}
//初始化
void allinit(){
	P2=0XA0;P0=0X00;
	P2=0XC0;P0=0XFF;P2=0XE0;P0=0XFF;
	P2=0X80;P0=0XFF;
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

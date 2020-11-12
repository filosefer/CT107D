/*
---------------------
第九届国赛多功能测量仪表
2020.3.29
完成
---------------------
*/


#include<STC15F2K60S2.H>
#include<intrins.h>
#include<onewire.h>
#include<iic.h>

sbit s7=P3^0;
sbit s6=P3^1;
sbit s5=P3^2;
sbit s4=P3^3;

bit flag_shanshuo;
long wendu;
unsigned int count_freq,freq,rb2_read;
unsigned char t_1,rb2_save,freq_save_1,freq_save_2,wendu_save_1,wendu_save_2,dis_h_1,dis_h_2,dianya_set;
unsigned char flag_yemian,flag_h,flag_longset;
unsigned char yi,er,san,si,wu,liu,qi,ba;
unsigned char seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10, 0xbf,0xff,0xc1,0xc6,0x8e,0x89,0x8c};
void allinit();
void delayms(int ms);
void display_12(yi,er);
void display_34(san,si);
void display_56(wu,liu);
void display_78(qi,ba);
void Timer0Init(void);
void Timer1Init(void);
unsigned char iic_read(unsigned char add);
void eeprom_write(unsigned char add,unsigned char dat);
unsigned char eeprom_read(unsigned char add);
long ds18b20_get();
void key_scan();
void led_set();	
//主函数
void main(){
	//初始化
	allinit();
	Timer0Init();Timer1Init();
	EA=1;ET0=1;ET1=1;
	dianya_set=eeprom_read(0x10);delayms(5);
	while(1){
		//实时读取电压
		rb2_read=iic_read(0x03)*2;
		//小灯亮灭函数
		led_set();		
		//电压显示界面
		if(flag_yemian==0){
			yi=22;er=21;san=21;si=21;wu=21;liu=21;qi=rb2_read/100+10;ba=rb2_read%100/10;
		}
		//频率显示界面
		else if(flag_yemian==1){
			yi=24;er=21;san=21;si=freq/10000;wu=freq%10000/1000;liu=freq%1000/100;qi=freq%100/10;ba=freq%10;
		}
		//温度显示界面
		else if(flag_yemian==2){
			wendu=ds18b20_get();
			yi=23;er=21;san=21;si=21;wu=wendu/100000;liu=wendu%100000/10000+10;qi=wendu%10000/1000;ba=wendu%1000/100;
		}
		//数据回显界面
		else if(flag_yemian==3){
			//电压显示
			if(flag_h==0){
				dis_h_1=eeprom_read(0x01);
				yi=25;er=22;san=21;si=21;wu=21;liu=21;qi=dis_h_1/10+10;ba=dis_h_1%10;
			}
			//温度显示
			else if(flag_h==1){
				dis_h_1=eeprom_read(0x02);
				dis_h_2=eeprom_read(0x03);
				yi=25;er=23;san=21;si=21;wu=dis_h_1/10;liu=dis_h_1%10+10;qi=dis_h_2/10;ba=dis_h_2%10;
			}
			//频率显示
			else if(flag_h==2){
				dis_h_1=eeprom_read(0x04);
				dis_h_2=eeprom_read(0x05);
				yi=25;er=24;san=21;si=dis_h_1/10;wu=dis_h_1%10;liu=dis_h_2/10;qi=dis_h_2%10;ba=0;
			}
		}
		//电压阈值设置界面
		else if(flag_yemian==4){
			yi=26;er=21;san=21;si=21;wu=21;liu=21;qi=dianya_set/10+10;ba=dianya_set%10;
		}
		//按键
		key_scan();
		display_12(yi,er);
		display_34(san,si);
		display_56(wu,liu);
		display_78(qi,ba);
	}
}
//独立按键
void key_scan(){
	//s4控制切换界面，回显数据
	if(s4==0){
		delayms(5);
		if(s4==0){
			//回显界面
			if(flag_yemian==3){
				flag_h++;
				if(flag_h==3)
					flag_h=0;
			}
			//测量界面
			else{
				flag_yemian++;
				if(flag_yemian==3)
					flag_yemian=0;
			}
		}
		while(!s4);
	}
	//s5存储
	else if(s5==0){
		delayms(5);
		if(s5==0){
			//保存电压
			rb2_read=iic_read(0x03)*2;
			rb2_save=rb2_read/100*10+rb2_read%100/10;
			eeprom_write(0x01,rb2_save);delayms(5);
			//保存温度，拆分开
			wendu=ds18b20_get();
			wendu_save_1=wendu/100000*10+wendu%100000/10000;
			wendu_save_2=wendu%10000/1000*10+wendu%1000/100;
			eeprom_write(0x02,wendu_save_1);delayms(5);
			eeprom_write(0x03,wendu_save_2);delayms(5);
			//保存频率拆分开
			freq_save_1=freq/10000*10+freq%10000/1000;
			freq_save_2=freq%1000/100*10+freq%100/10;
			eeprom_write(0x04,freq_save_1);delayms(5);
			eeprom_write(0x05,freq_save_2);delayms(5);
		}
		while(!s5);
	}
	//s6回显界面及设置阈值
	else if(s6==0){
		delayms(5);
		if(s6==0){
			//回显界面
			if(flag_yemian==3)
				flag_yemian=0;
			//阈值设定
			else if(flag_yemian==4){
				flag_longset=1;
				t_1=0;
				dianya_set++;
				if(dianya_set>50)
					dianya_set=1;
			}
			else
				flag_yemian=3;
		}
		while(!s6){
			//阈值设定时，按下超过0.8s，快速增加
			if(flag_yemian==4){
				if(flag_longset==2){
					dianya_set++;
					if(dianya_set>50)
						dianya_set=1;
				}
				yi=26;er=21;san=21;si=21;wu=21;liu=21;qi=dianya_set/10+10;ba=dianya_set%10;
				display_12(yi,er);display_34(san,si);display_56(wu,liu);display_78(qi,ba);
			}
		}
		flag_longset=0;
	}
	//s7控制电压阈值设定保存
	else if(s7==0){
		delayms(5);
		if(s7==0){
			if(flag_yemian==4){
				flag_yemian=0;
				eeprom_write(0x10,dianya_set);delayms(5);
			}
			else
				flag_yemian=4;
		}
		while(!s7);
	}
}
//小灯亮灭函数
void led_set(){
	//判断电压
	if(rb2_read>dianya_set*10){
		if(flag_shanshuo==0) {
			P2=0X80;P0=0X7F;
		}
		else {
			P2=0X80;P0=0XFF;
		}
	}
	else {
		P2=0X80;P0=0XFF;
	}
	//判断当前测量
	if(flag_yemian==0){
		P2=0X80;P0=~(~P0|0X04);
	}
	else if(flag_yemian==1){
		P2=0X80;P0=~(~P0|0X02);
	}
	else if(flag_yemian==2){
		P2=0X80;P0=~(~P0|0X01);
	}
	else {
		P2=0X80;P0=P0;
	}
}
//DS18B20温度获取
long ds18b20_get(){
	unsigned char low,high;
	long temp;
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0x44);
	delayms(1);
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0xbe);
	delayms(1);
	low=Read_DS18B20();
	high=Read_DS18B20();
	init_ds18b20();
	temp=high&0x0f;
	temp<<=8;
	temp|=low;
	temp=temp*625;
	return temp;
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
//IIc读取
unsigned char iic_read(unsigned char add){
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
//定时器0用来计数
void time0() interrupt 1{
	count_freq++;
}
//定时器1用来定时
void time1() interrupt 3{
	t_1++;
	if(flag_longset==1){
		if(t_1==160)
			flag_longset=2;
	}
	if(rb2_read>dianya_set*10){
		if(t_1%40==0){
			flag_shanshuo=!flag_shanshuo;
		}
	}
	if(t_1==200){
		freq=count_freq;
		count_freq=0;
		t_1=0;
	}
}
//定时器1定时5ms
void Timer1Init(void)
{
	AUXR |= 0x40;
	TMOD &= 0x0F;	
	TL1 = 0x00;		
	TH1 = 0x28;		
	TF1 = 0;	
	TR1 = 1;	
}
//定时器0计数模式
void Timer0Init(void)	
{
	AUXR |= 0x80;	
	TMOD &= 0x0F;	
	TMOD |= 0x04;	
	TL0 = 0XFF;		
	TH0 = 0XFF;		
	TF0 = 0;		
	TR0 = 1;
}
//初始化
void allinit(){
	P2=0XA0;P0=0X00;
	P2=0XC0;P0=0XFF;P2=0XE0;P0=0XFF;
	P2=0X80;P0=0XFF;
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

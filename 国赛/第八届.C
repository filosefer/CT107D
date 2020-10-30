/*
-----------------------------
第八届国赛超声波测距机
2020.3.22
DA输出电压未测量
*/
#include<stc15f2k60s2.h>
#include<intrins.h>
#include<iic.h>
#define somenop {_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();}
sbit TX=P1^0;
sbit RX=P1^1;
sbit s7=P3^0;
sbit s6=P3^1;
sbit s5=P3^2;
sbit s4=P3^3;
bit flag_1000,flag_200,flag_led1;
unsigned char eep[4];
unsigned char seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xbf,0xff,0xc6,0x8e};
unsigned char yi,er,san,si,wu,liu,qi,ba;
unsigned char flag_yemian;
unsigned char count_wave=1,AD_read,count_eep,count_led1;
unsigned int distance,distance_last,distance_set,t_0,count_0;
void delayms(int ms);
void dislpay_12(yi,er);
void dislpay_34(san,si);
void dislpay_56(wu,liu);
void dislpay_78(qi,ba);
void allinit();
void send_wave();
unsigned char rect_wave();
void Timer0Init(void);
void Timer1Init(void);
void key_scan();
void DA_write(unsigned char dat);
unsigned char eeprom_read(unsigned char add);
void eeprom_write(unsigned char add,unsigned char dat);
void distance_write();
//主函数
void main(){
	allinit();
	Timer0Init();Timer1Init();
	EA=1;ET0=1;
	distance_set=eeprom_read(0x06);
	while(1){
		//测距显示界面
		if(flag_yemian==0){
			yi=12;er=11;san=distance/100;si=distance%100/10;wu=distance%10;liu=distance_last/100;qi=distance_last%100/10;ba=distance_last%10;
		}
		//数据回显界面
		else if(flag_yemian==1){
			eep[0]=eeprom_read(0x01);eep[1]=eeprom_read(0x02);
			eep[2]=eeprom_read(0x03);eep[3]=eeprom_read(0x04);
			yi=count_wave;er=11;san=11;si=11;wu=11;liu=eep[count_wave-1]/100;qi=eep[count_wave-1]%100/10;ba=eep[count_wave-1]%10;
		}
		//参数设置界面
		else if(flag_yemian==2){
			yi=13;er=11;san=11;si=11;wu=11;liu=11;qi=distance_set%100/10;ba=distance_set%10;
		}
		key_scan();
		dislpay_12(yi,er);
		dislpay_34(san,si);
		dislpay_56(wu,liu);
		dislpay_78(qi,ba);
	}
}	
//按键
void key_scan(){
	//s7控制调整
	if(s7==0){
		delayms(5);
		if(s7==0){
			//若为数据显示
			if(flag_yemian==1){
				count_wave++;
				if(count_wave==5)
					count_wave=1;
			}
			//若为盲区设置
			else if(flag_yemian==2){
				distance_set=distance_set+10;
				if(distance_set>30)
					distance_set=distance_set-30;
			}
		}
		while(!s7);
	}
	//s6控制盲区显示
	else if(s6==0){
		delayms(5);
		if(s6==0){
			//再次按下退出并保存
			if(flag_yemian==2){
				P2=0X80;P0=0XFF;
				flag_yemian=0;
				eeprom_write(0x06,distance_set);delayms(5);
				distance_set=eeprom_read(0x06);
			}
			else {
				flag_yemian=2;
				P2=0X80;P0=0XBF;
			}
		}
		while(!s6);
	}
	//s5控制测距显示
	else if(s5==0){
		delayms(5);
		if(s5==0){
			if(flag_yemian==1){
				flag_yemian=0;
				P2=0X80;P0=0XFF;
			}
			else{
				flag_yemian=1;
				P2=0X80;P0=0X7F;
			}
		}
		while(!s5);
	}
	//s4控制超声波测距
	else if(s4==0){
		delayms(5);
		if(s4==0){
			flag_yemian=0;
			distance=rect_wave();
			distance_write();
			count_eep++;
			flag_led1=1;count_led1=0;
			if(count_eep==4) count_eep=0;
			//进行盲区判断
			if(distance<distance_set)
				DA_write(0x00);
			else if((distance-distance_set)>250)
				DA_write(0xff);
			else 
				DA_write(distance-distance_set);
			
		}
		while(!s4);
	}
}
//超声波距离写入与读取
void distance_write(){
	if(count_eep==0){
		eeprom_write(0x01,distance);delayms(5);
		distance=eeprom_read(0x01);
		distance_last=eeprom_read(0x04);
	}
	else if(count_eep==1){
		eeprom_write(0x02,distance);delayms(5);
		distance=eeprom_read(0x02);
		distance_last=eeprom_read(0x01);
	}
	else if(count_eep==2){
		eeprom_write(0x03,distance);delayms(5);
		distance=eeprom_read(0x03);
		distance_last=eeprom_read(0x02);
	}
	else if(count_eep==3){
		eeprom_write(0x04,distance);delayms(5);
		distance=eeprom_read(0x04);
		distance_last=eeprom_read(0x03);
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
//DA输出电压
void DA_write(unsigned char dat){
	IIC_Start();
	IIC_SendByte(0x90);
	IIC_WaitAck();
	IIC_SendByte(0x40);
	IIC_WaitAck();
	IIC_SendByte(dat);
	IIC_WaitAck();
	IIC_Stop();
}
//超声波发送，拉高40us
void send_wave(){
	unsigned char i=8;
	while(i--){
		TX=1;
		somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;
		somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;
		TX=0;
		somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;
		somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;
	}
}
//超声波接收
unsigned char rect_wave(){
	unsigned int temp;
	//发送
	send_wave();
	TR1=1;
	//接收，接收到或溢出
	while((RX==1)&&(TF1==0));
	TR1=0;
	//如果溢出
	if(TF1==1){
		TF1=0;
		temp=999;
	}
	//如果接收到信号
	else {
		temp=TH1;
		temp=(temp<<8)|TL1;
		TH1=0;TL1=0;
		temp=(unsigned int)(temp*0.017/12);
	}
	return temp;
}
//定时器0计时200ms
void time0() interrupt 1{
	t_0++;
	if(t_0==100){
		t_0=0;
		count_0++;
		flag_200=!flag_200;
	}
	if(count_0==5){
		count_0=0;
		flag_1000=!flag_1000;
		count_led1++;
		if(count_led1==7){
			flag_led1=0;
			count_led1=0;
		}
		if(flag_led1==1){
			if(flag_1000==0){
				P2=0X80;P0=0XFE;
			}
			else {
				P2=0X80;P0=0XFF;
			}
		}
	}
}
//定时器0定时2毫秒
void Timer0Init(void)	
{
	AUXR |= 0x80;	
	TMOD &= 0xF0;
	TL0 = 0x9A;
	TH0 = 0xA9;
	TF0 = 0;
	TR0 = 1;
}
//定时器1用于超声波接收计时
void Timer1Init(void)	
{
	AUXR |= 0x40;
	TMOD &= 0x0F;	
	TL1 = 0;
	TH1 = 0;
}
//延时
void delayms(int ms){
	int q,w;
	for(q=0;q<ms;q++)
		for(w=0;w<845;w++);
}
//数码管
void dislpay_12(yi,er){
	P2=0XC0;P0=0X01;P2=0XE0;P0=seg[yi];delayms(1);
	P2=0XC0;P0=0X02;P2=0XE0;P0=seg[er];delayms(1);
}
void dislpay_34(san,si){
	P2=0XC0;P0=0X04;P2=0XE0;P0=seg[san];delayms(1);
	P2=0XC0;P0=0X08;P2=0XE0;P0=seg[si];delayms(1);
}
void dislpay_56(wu,liu){
	P2=0XC0;P0=0X10;P2=0XE0;P0=seg[wu];delayms(1);
	P2=0XC0;P0=0X20;P2=0XE0;P0=seg[liu];delayms(1);
}
void dislpay_78(qi,ba){
	P2=0XC0;P0=0X40;P2=0XE0;P0=seg[qi];delayms(1);
	P2=0XC0;P0=0X80;P2=0XE0;P0=seg[ba];delayms(1);
	P2=0XC0;P0=0XFF;P2=0XE0;P0=0XFF;delayms(1);
}
//初始化
void allinit(){
	P2=0XA0;P0=0X00;
	P2=0XC0;P0=0XFF;P2=0XE0;P0=0XFF;
	P2=0X80;P0=0XFF;
}

/*
----------------------------------
第六届国赛
本来想把数码管写中断，可是一直闪啊闪啊闪啊，没找到错误
还有一个问题是小灯的开闭，单独用P0^1的话其余小灯会闪啊闪啊闪啊
此部分超声波没写。
2020.3.5
----------------------------------
*/
#include<STC15F2K60S2.H>
#include<iic.h>
sbit s7=P3^0;
sbit s6=P3^1;
sbit s5=P3^2;
sbit s4=P3^3;
int dianya;
unsigned char count_s6;
unsigned char time_move,time_set1,time_set2;
unsigned char distance=31,type,distance_set=30;
unsigned char time0_i,time1_i;
unsigned char yi,er,san,si,wu,liu,qi,ba;
unsigned char seg[]={0XC0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xbf,0xff};
unsigned char flag_yemian=3,flag_move,flag_stop,time0_flag;
void Timer1Init(void);
void Timer0Init(void);
void delayms(int ms);
void allinit();
void display_12(yi,er);
void display_34(san,si);
void display_56(wu,liu);
void display_78(qi,ba);
void dianya_pd();
unsigned char iic_read();
void distance_pd();
void key_scan();
void eeprom_write(unsigned char add ,unsigned char dat);
unsigned char eeprom_read(unsigned char add);
void main(){
	allinit();
	Timer0Init();
	Timer1Init();
	EA=1;ET0=1;ET1=1;
	yi=10;er=10;san=10;si=10;wu=10;liu=10;qi=10;ba=10;
	time_set1=eeprom_read(0x01);
	time_set2=eeprom_read(0x02);
	while(1){
		//电压读取转换
		dianya=1.96*iic_read();
		//初始界面黑屏
		if(flag_yemian==3){
			dianya_pd();
			yi=11;er=11;san=11;si=11;wu=11;liu=11;qi=11;ba=11;
		}
		//界面1进行判断
		else if(flag_yemian==0){
			dianya_pd();
			distance_pd();
			yi=1;er=11;san=11;si=distance/10;wu=distance%10;liu=11;qi=11;ba=type;
		}
		//界面2显示剩余运送时间
		else if(flag_yemian==1){
			yi=2;er=11;san=11;si=11;wu=11;liu=11;qi=time_move/10;ba=time_move%10;
		}
		//界面3设置倒计时
		else if(flag_yemian==2){
			//设置1
			if(count_s6==1){
				if(time0_flag==0){
					yi=3;er=11;san=11;si=time_set1/10;wu=time_set1%10;liu=11;qi=time_set2/10;ba=time_set2%10;
				}
				else if(time0_flag==1){
					yi=3;er=11;san=11;si=11;wu=11;liu=11;qi=time_set2/10;ba=time_set2%10;
				}
			}
			//设置2
			else if(count_s6==2){
				if(time0_flag==0){
					yi=3;er=11;san=11;si=time_set1/10;wu=time_set1%10;liu=11;qi=time_set2/10;ba=time_set2%10;
				}
				else if(time0_flag==1){
					yi=3;er=11;san=11;si=time_set1/10;wu=time_set1%10;liu=11;qi=11;ba=11;
				}
			}
		}
		key_scan();
		display_12(yi,er);
		display_34(san,si);
		display_56(wu,liu);
		display_78(qi,ba);
	}
}
//按键部分
void key_scan(){
	//s7调整计时
	if(s7==0){
		delayms(5);
		if(s7==0){
			if(dianya==0){
				if(count_s6==1){
					time_set1++;
					if(time_set1==11)
						time_set1=1;
				}
				else if(count_s6==2){
					time_set2++;
					if(time_set2==11)
						time_set2=1;
				}
			}
		}
		while(!s7);
	}
	//设置计时
	else if(s6==0){
		delayms(5);
		if(s6==0){
			if(dianya==0){
				flag_yemian=2;
				count_s6++;
				if(count_s6==3){
					eeprom_write(0x01 ,time_set1);delayms(2);
					eeprom_write(0x02 ,time_set2);delayms(2);
					flag_yemian=3;
					count_s6=0;
				}
			}
		}
		while(!s6);
	}
	//紧急停止
	else if(s5==0){
		delayms(5);
		if(s5==0){
			if(flag_stop==0){
				if(time_move>0){
					P2=(P2&0x1f)|0XA0;P0=0X00;P2&=0X1F;
					flag_stop=1;
					ET1=0;
				}
			}
			else if(flag_stop==1){
				if(time_move>0){
					P2=(P2&0x1f)|0XA0;P0=0X10;P2&=0X1F;
					P2=(P2&0x1f)|0X80;P0=0XFF;P2&=0X1F;
					flag_stop=0;
					ET1=1;
				}
			}
		}
		while(!s5);
	}
	//启动传送
	else if(s4==0){
		delayms(5);
		if(s4==0){
			if(flag_move==1){
				if(flag_yemian==0){
					if(type==1)time_move=time_set1;
					else if(type==2)time_move=time_set2;
					P2=(P2&0x1f)|0XA0;P0=0X10;P2&=0X1F;
					flag_yemian=1;
				}
			}
		}
		while(!s4);
	}
}
//EEP读取
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
//EEP写入
void eeprom_write(unsigned char add ,unsigned char dat){
	IIC_Start();
	IIC_SendByte(0xa0);
	IIC_WaitAck();
	IIC_SendByte(add);
	IIC_WaitAck();
	IIC_SendByte(dat);
	IIC_WaitAck();
	IIC_Stop();
}
//距离判断
void distance_pd(){
	if(dianya>0){
		if(distance>distance_set){
			{
				type=2;
			}
		}
		else type=1;
	}
	else type=0;
}
//过载判断
void dianya_pd(){
		if(dianya==0){
			flag_yemian=3;
			flag_move=0;
			P2=(P2&0x1f)|0X80;P0=0Xff;P2&=0X1F;
			P2=(P2&0x1f)|0XA0;P0=0X00;P2&=0X1F;
		}
		else if(dianya<100){
			flag_yemian=0;
			flag_move=1;
			P2=(P2&0x1f)|0X80;P0=~0x01;P2&=0X1F;
			P2=(P2&0x1f)|0XA0;P0=0X00;P2&=0X1F;
		}
		else if(dianya > 400){
			flag_yemian=0;
			flag_move=0;
			P2=(P2&0x1f)|0XA0;P0=0X40;P2&=0X1F;
		}
		else {
			flag_yemian=0;
			flag_move=1;
			P2=(P2&0x1f)|0X80;P0=~0x02;P2&=0X1F;
			P2=(P2&0x1f)|0XA0;P0=0X00;P2&=0X1F;
		}
}
//定时器0控制闪烁
void time0() interrupt 1{
	time0_i++;
	if(time0_i<50){
		time0_flag=0;
		if(dianya > 400){
			P2=0X80;P0=~0x04;P2&=0X1F;
		}
		else if(flag_stop==1){
			P2=(P2&0x1f)|0X80;P0=~0x08;P2&=0X1F;
		}
	}
	else if(time0_i<100){
		time0_flag=1;
		if(dianya > 400){
			P2=(P2&0x1f)|0X80;P0=0XFF;P2&=0X1F;
		}
		else if(flag_stop==1){
			P2=(P2&0x1f)|0X80;P0=0XFF;P2&=0X1F;
		}
	}
	else time0_i=0;
}
//定时器1控制倒计时
void time1() interrupt 3{
	time1_i++;
	if(time1_i==200){
		time1_i=0;
		if(time_move>0)
			time_move--;
		else{
			if(flag_yemian==1){
				flag_yemian=0;
				P2=(P2&0x1f)|0XA0;P0=0X00;P2&=0X1F;
			}
		}
	}
}
//定时器0控制5ms
void Timer0Init(void)
{
	AUXR |= 0x80;
	TMOD &= 0xF0;
	TL0 = 0x00;	
	TH0 = 0x28;	
	TF0 = 0;	
	TR0 = 1;	
}
//定时器1控制5ms
void Timer1Init(void)		
{
	AUXR |= 0x40;		
	TMOD &= 0x0F;		
	TL1 = 0x00;		
	TH1 = 0x28;	
	TF1 = 0;	
	TR1 = 1;		
}
//IIC读取
unsigned char iic_read(){
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
//初始化
void allinit(){
	P2=0XA0;P0=0X00;
	P2=0XC0;P0=0XFF;P2=0XE0;P0=0XFF;
	P2=0X80;P0=0XFF;
}
void delayms(int ms){
	int q,w;
	for(q=0;q<ms;q++)
		for(w=0;w<845;w++);
}
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

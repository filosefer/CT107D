/第三届国赛门禁系统
#include<STC15F2K60S2.H>
#include<ds1302.h>
#include<iic.h>
unsigned char ack_i,ack_count,bell_count;
unsigned char eep_i,input_i=6,input_temp;
unsigned char flag_yemian;
unsigned char eep_read[6],input_word[]={11,11,11,11,11,11,11,11};
unsigned char seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xbf,0xff};
unsigned char yi,er,san,si,wu,liu,qi,ba;
unsigned char time_init[]={0,59,6,0,0,0,0};
unsigned char time_read[7];
void delayms(int ms);
void allinit();
void display_12(yi,er);
void display_34(san,si);
void display_56(wu,liu);
void display_78(qi,ba);
void ds1302_init();
void ds1302_read();
void key_scan();
unsigned char EEPROM_READ(unsigned char add);
void EEPROM_write(unsigned char add,unsigned char dat);
void main(){
	allinit();
	ds1302_init();
	yi=10;er=10;san=10;si=10;wu=10;liu=10;qi=10;ba=10;
	while(1){
		ds1302_read();
		eep_read[0]=EEPROM_READ(0x01);delayms(1);eep_read[1]=EEPROM_READ(0x02);delayms(1);eep_read[2]=EEPROM_READ(0x03);delayms(1);
		eep_read[3]=EEPROM_READ(0x04);delayms(1);eep_read[4]=EEPROM_READ(0x05);delayms(1);eep_read[5]=EEPROM_READ(0x06);delayms(1);
		//时钟显示界面
		if(time_read[2]<7|time_read[2]>22){
			yi=time_read[2]/10;er=time_read[2]%10;san=10;si=time_read[1]/10;wu=time_read[1]%10;liu=10;qi=time_read[0]/10;ba=time_read[0]%10;
		}
		else{
			//输入密码界面
			if(flag_yemian==0){
				input_word[input_i]=input_temp;
				yi=10;er=10;san=input_word[5];si=input_word[4];wu=input_word[3];liu=input_word[2];qi=input_word[1];ba=input_word[0];
				//yi=10;er=10;san=eep_read[5];si=eep_read[4];wu=eep_read[3];liu=eep_read[2];qi=eep_read[1];ba=eep_read[0];
			}
			//密码修改界面
			else if(flag_yemian==1){
				input_word[input_i]=input_temp;
				yi=11;er=10;san=input_word[5];si=input_word[4];wu=input_word[3];liu=input_word[2];qi=input_word[1];ba=input_word[0];
			}
			//密码设置界面
			else if(flag_yemian==2){
				input_word[input_i]=input_temp;
				yi=10;er=11;san=input_word[5];si=input_word[4];wu=input_word[3];liu=input_word[2];qi=input_word[1];ba=input_word[0];
			}
		}
		//矩阵按键
		key_scan();
		display_12(yi,er);
		display_34(san,si);
		display_56(wu,liu);
		display_78(qi,ba);
	}
}
void key_scan(){
	unsigned char temp;
	P44=0;P42=1;P3=0XFF;
	temp=P3;
	temp=temp&0x0f;
	if(temp!=0x0f){
		delayms(5);
		temp=P3;temp=temp&0x0f;
		if(temp!=0x0f){
			temp=P3;
			switch(temp){
				//0
				case 0xfe:input_temp=0;break;
				//4
				case 0xfd:input_temp=4;break;
				//8
				case 0xfb:input_temp=8;break;
				case 0xf7:break;
			}
			while(temp!=0x0f){
				temp=P3;temp=temp&0x0f;
			}
			if(flag_yemian==0|flag_yemian==1|flag_yemian==2){input_i--;}
		}
	}
	P44=1;P42=0;P3=0XFF;
	temp=P3;
	temp=temp&0x0f;
	if(temp!=0x0f){
		delayms(5);
		temp=P3;temp=temp&0x0f;
		if(temp!=0x0f){
			temp=P3;
			switch(temp){
				//1
				case 0xfe:input_temp=1;break;
				//5
				case 0xfd:input_temp=5;break;
				//9
				case 0xfb:input_temp=9;break;
				case 0xf7:break;
			}
			while(temp!=0x0f){
				temp=P3;temp=temp&0x0f;
			}
			if(flag_yemian==0|flag_yemian==1|flag_yemian==2){input_i--;}
		}
	}
	P44=1;P42=1;P3=0XDF;
	temp=P3;
	temp=temp&0x0f;
	if(temp!=0x0f){
		delayms(5);
		temp=P3;temp=temp&0x0f;
		if(temp!=0x0f){
			temp=P3;
			switch(temp){
				//2
				case 0xde:input_temp=2;break;
				//6
				case 0xdd:input_temp=6;break;
				//进入密码修改界面输入原密码
				case 0xdb:
					flag_yemian=1;
					for(ack_i=0;ack_i<6;ack_i++){
						input_word[ack_i]=11;
					}
					input_i=7;
					break;
				//确认按键
				case 0xd7:
					//密码修改界面，写入密码，及重置
					if(flag_yemian==2){
						for(ack_i=0;ack_i<6;ack_i++){
							eep_read[ack_i]=input_word[ack_i];
							EEPROM_write(0x01+ack_i,input_word[ack_i]);delayms(5);
							input_word[ack_i]=11;
						}
						input_i=7;	
						flag_yemian=0;
					}
					else{
						//进行密码正确的判断
						for(ack_i=0;ack_i<6;ack_i++){
							if(input_word[ack_i]==eep_read[ack_i])
								ack_count++;
						}
						//如果全正确
						if(ack_count==6){
							//密码状态时开门
							if(flag_yemian==0){
								P2=0XA0;P0=0X10;P2=P2&0X2F;
								delayms(5000);
								P2=0XA0;P0=0X00;P2=P2&0X2F;
								for(ack_i=0;ack_i<6;ack_i++){
								input_word[ack_i]=11;
								}
								input_i=7;
							}
							//修改密码状态时进入密码修改界面
							else if(flag_yemian==1){
								flag_yemian=2;
								for(ack_i=0;ack_i<6;ack_i++){
								input_word[ack_i]=11;
								}
								input_i=7;	
							}
						}
						//如果输入不正确，报警
						else{
							bell_count++;
							if(bell_count==3)
							{
								P2=0XA0;P0=0X40;
								delayms(3000);
								P2=0XA0;P0=0X00;
								//如果在修改密码界面则退出修改
								if(flag_yemian==1){flag_yemian=0;}
								bell_count=0;
							}
								for(ack_i=0;ack_i<6;ack_i++){
									input_word[ack_i]=11;
								}
								input_i=7;	
						}
						ack_count=0;
					}
					break;
			}
			while(temp!=0x0f){
				temp=P3;temp=temp&0x0f;
			}
			if(flag_yemian==0|flag_yemian==1|flag_yemian==2){input_i--;}
		}
	}
	P44=1;P42=1;P3=0XEF;
	temp=P3;
	temp=temp&0x0f;
	if(temp!=0x0f){
		delayms(5);
		temp=P3;temp=temp&0x0f;
		if(temp!=0x0f){
			temp=P3;
			switch(temp){
				//3
				case 0xee:input_temp=3;break;
				//7
				case 0xed:input_temp=7;break;
				//复位，写入初始密码654321
				case 0xeb:
					flag_yemian=0;
					EEPROM_write(0x01,1);delayms(5);EEPROM_write(0x02,2);delayms(5);EEPROM_write(0x03,3);delayms(5);
					EEPROM_write(0x04,4);delayms(5);EEPROM_write(0x05,5);delayms(5);EEPROM_write(0x06,6);delayms(5);
					for(ack_i=0;ack_i<6;ack_i++){
						input_word[ack_i]=11;
					}
					input_i=7;
					break;
				//退出到密码输入状态
				case 0xe7:
					flag_yemian=0;
					for(ack_i=0;ack_i<6;ack_i++){
						input_word[ack_i]=11;
					}
					input_i=7;
					break;
			}
			while(temp!=0x0f){
				temp=P3;temp=temp&0x0f;
			}
			if(flag_yemian==0|flag_yemian==1|flag_yemian==2){input_i--;}
		}
	}
}
//eeprom读取
unsigned char EEPROM_READ(unsigned char add){
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
void EEPROM_write(unsigned char add,unsigned char dat){
	IIC_Start();
	IIC_SendByte(0xa0);
	IIC_WaitAck();
	IIC_SendByte(add);
	IIC_WaitAck();
	IIC_SendByte(dat);
	IIC_WaitAck();
	IIC_Stop();
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

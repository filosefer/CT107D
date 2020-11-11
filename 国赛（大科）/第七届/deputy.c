#include<STC15F2K60S2.H>
#include<DEPUTY.H>
#include <intrins.h>


#define somenop {_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();}    

#define SlaveAddrW 0xA0
#define SlaveAddrR 0xA1

//总线引脚定义
sbit SDA = P2^1;  /* 数据线 */
sbit SCL = P2^0;  /* 时钟线 */


sbit SCK=P1^7;		
sbit IO=P2^3;		
sbit RST = P1^3;   // DS1302复位	

uchar shijian[]={55,59,23,18,2,4,16};

//----------------------------------时钟-------------------------------------------------------------//

void Write_Ds1302_Byte(unsigned  char temp) 
{
	unsigned char i;
	for (i=0;i<8;i++)     	
	{ 
		SCK=0;
		IO=temp&0x01;
		temp>>=1; 
		SCK=1;
	}
}   

void Write_Ds1302( unsigned char address,unsigned char dat )     
{
 	RST=0;
	_nop_();
 	SCK=0;
	_nop_();
 	RST=1;	
   	_nop_();  
 	Write_Ds1302_Byte(address);	
 	Write_Ds1302_Byte((dat/10<<4)|(dat%10));		
 	RST=0; 
}

unsigned char Read_Ds1302 ( unsigned char address )
{
 	unsigned char i,temp=0x00,dat1,dat2;
 	RST=0;
	_nop_();
 	SCK=0;
	_nop_();
 	RST=1;
	_nop_();
 	Write_Ds1302_Byte(address);
 	for (i=0;i<8;i++) 	
 	{		
		SCK=0;
		temp>>=1;	
 		if(IO)
 		temp|=0x80;	
 		SCK=1;
	} 
 	RST=0;
	_nop_();
 	RST=0;
	SCK=0;
	_nop_();
	SCK=1;
	_nop_();
	IO=0;
	_nop_();
	IO=1;
	_nop_();
	
	
	dat1=temp/16;
	dat2=temp%16;
	temp=dat1*10+dat2;
	
	return temp;			
}


void dsinit(void)
{
	uchar i,add;
	add=0x80;
	Write_Ds1302(0x8e,0x00);
	for(i=0;i<7;i++)
	{
		Write_Ds1302(add,shijian[i]);
		add=add+2;
	}
	Write_Ds1302(0x8e,0x80);
}

void dsget(void)
{
	uchar i,add;
	add=0x81;
	Write_Ds1302(0x8e,0x00);
	for(i=0;i<7;i++)
	{
		shijian[i]=Read_Ds1302(add);
		add=add+2;
	}
	Write_Ds1302(0x8e,0x80);
}

//-------------------------------------IIC----------------------------------------------------------//

//总线启动条件
void IIC_Start(void)
{
	SDA = 1;
	SCL = 1;
	somenop;
	SDA = 0;
	somenop;
	SCL = 0;	
}

//总线停止条件
void IIC_Stop(void)
{
	SDA = 0;
	SCL = 1;
	somenop;
	SDA = 1;
}

//应答位控制
//void IIC_Ack(bit ackbit)
//{
//	if(ackbit) 
//	{	
//		SDA = 0;
//	}
//	else 
//	{
//		SDA = 1;
//	}
//	somenop;
//	SCL = 1;
//	somenop;
//	SCL = 0;
//	SDA = 1; 
//	somenop;
//}

//等待应答
bit IIC_WaitAck(void)
{
	SDA = 1;
	somenop;
	SCL = 1;
	somenop;
	if(SDA)    
	{   
		SCL = 0;
		IIC_Stop();
		return 0;
	}
	else  
	{ 
		SCL = 0;
		return 1;
	}
}

//通过I2C总线发送数据
void IIC_SendByte(unsigned char byt)
{
	unsigned char i;
	for(i=0;i<8;i++)
	{   
		if(byt&0x80) 
		{	
			SDA = 1;
		}
		else 
		{
			SDA = 0;
		}
		somenop;
		SCL = 1;
		byt <<= 1;
		somenop;
		SCL = 0;
	}
}

//从I2C总线上接收数据
unsigned char IIC_RecByte(void)
{
	unsigned char da;
	unsigned char i;
	
	for(i=0;i<8;i++)
	{   
		SCL = 1;
		somenop;
		da <<= 1;
		if(SDA) 
		da |= 0x01;
		SCL = 0;
		somenop;
	}
	return da;
}

void  EEPROM_Write(uchar add,uchar date)
{
	IIC_Start();
	IIC_SendByte(0xa0);
	IIC_WaitAck();
	IIC_SendByte(add);
	IIC_WaitAck();
	IIC_SendByte(date);
	IIC_WaitAck();
	IIC_Stop();
}

uchar EEPROM_Read(uchar add)
{
	uchar date;
	IIC_Start();
	IIC_SendByte(0xa0);
	IIC_WaitAck();
	IIC_SendByte(add);
	IIC_WaitAck();
	
	IIC_Start();
	IIC_SendByte(0xa1);
	IIC_WaitAck();
	date=IIC_RecByte();
	IIC_Stop();
	
	return date;
}

uchar AD_Read(uchar add)
{
	uchar date;
	IIC_Start();
	IIC_SendByte(0x90);
	IIC_WaitAck();
	IIC_SendByte(add);
	IIC_WaitAck();
	
	IIC_Start();
	IIC_SendByte(0x91);
	IIC_WaitAck();
	date=IIC_RecByte();
	IIC_Stop();
	
	return date;
}



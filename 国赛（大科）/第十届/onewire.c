/*
  程序说明: 单总线驱动程序
  软件环境: Keil uVision 4.10 
  硬件环境: CT107单片机综合实训平台(外部晶振12MHz) STC89C52RC单片机
  日    期: 2011-8-9
*/
#include "reg52.h"

sbit DQ = P1^4;  //单总线接口

//单总线延时函数
void Delay_OneWire(unsigned int t)  //STC89C52RC
{
	while(t--);
}

//通过单总线向DS18B20写一个字节
void Write_DS18B20(unsigned char dat)
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		DQ = 0;
		DQ = dat&0x01;
		Delay_OneWire(40);
		DQ = 1;
		dat >>= 1;
	}
	Delay_OneWire(40);
}

//从DS18B20读取一个字节
unsigned char Read_DS18B20(void)
{
	unsigned char i;
	unsigned char dat;
  
	for(i=0;i<8;i++)
	{
		DQ = 0;
		dat >>= 1;
		DQ = 1;
		if(DQ)
		{
			dat |= 0x80;
		}	    
		Delay_OneWire(40);
	}
	return dat;
}

//DS18B20设备初始化
bit init_ds18b20(void)
{
  	bit initflag = 0;
  	
  	DQ = 1;
  	Delay_OneWire(90);
  	DQ = 0;
  	Delay_OneWire(640);
  	DQ = 1;
  	Delay_OneWire(80); 
    initflag = DQ;     
  	Delay_OneWire(40);
  
  	return initflag;
}

long Tempget(void)
{
	unsigned char low,high;
	long temp;
	init_ds18b20();
	Write_DS18B20(0XCC);
	Write_DS18B20(0X44);
	Delay_OneWire(200); 
	
	init_ds18b20();
	Write_DS18B20(0XCC);
	Write_DS18B20(0XBE);

	low=Read_DS18B20();
	high=Read_DS18B20();
	
//	temp=high<<4;
//	temp|=(low>>4);
	
//精度为0.0625摄氏度   
	temp = (high&0x0f);
	temp <<= 8;
	temp |= low;

	temp=temp*625;
	
	return temp;
}







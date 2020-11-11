/*
  ����˵��: ��������������
  �������: Keil uVision 4.10 
  Ӳ������: CT107��Ƭ���ۺ�ʵѵƽ̨(�ⲿ����12MHz) STC89C52RC��Ƭ��
  ��    ��: 2011-8-9
*/
#include "reg52.h"

sbit DQ = P1^4;  //�����߽ӿ�

//��������ʱ����
void Delay_OneWire(unsigned int t)  //STC89C52RC
{
	while(t--);
}

//ͨ����������DS18B20дһ���ֽ�
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

//��DS18B20��ȡһ���ֽ�
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

//DS18B20�豸��ʼ��
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
	
//����Ϊ0.0625���϶�   
	temp = (high&0x0f);
	temp <<= 8;
	temp |= low;

	temp=temp*625;
	
	return temp;
}







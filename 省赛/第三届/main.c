#include<STC15F2K60S2.H>
#include<intrins.h>
#define uchar unsigned char
	

void Delay1ms();
void Delay100us();

uchar yi,er,san,si,wu,liu,qi,ba;
uchar table[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
										0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xff};

uchar guang;
uchar tt=0;
int _10ml=0;
int jiage=0;
//=======================================================
#define _IIC_H

void IIC_Start(void); 
void IIC_Stop(void);  
bit IIC_WaitAck(void);  
void IIC_SendAck(bit ackbit); 
void IIC_SendByte(unsigned char byt); 
unsigned char IIC_RecByte(void); 
#define DELAY_TIME 5

#define SlaveAddrW 0xA0
#define SlaveAddrR 0xA1

//总线引脚定义
sbit SDA = P2^1;  /* 数据线 */
sbit SCL = P2^0;  /* 时钟线 */
//======================================================

void Init_HC138(uchar n)
{
	switch(n)
	{
		case 4:
			P2=(P2&0x1f)|0x80;
			break;
		case 5:
			P2=(P2&0x1f)|0xa0;
			break;
		case 6:
			P2=(P2&0x1f)|0xc0;
			break;
		case 7:
			P2=(P2&0x1f)|0xe0;
			break;
	}
}

void Init_All()
{
	Init_HC138(4);
	P0=0xff;
	Init_HC138(5);
	P0=0x00;
}

void SMG_Show(uchar dat,uchar pos)
{
	Init_HC138(6);
	P0=0x01<<pos;
	Init_HC138(7);
	P0=dat;
}

void display1(uchar yi,uchar er)
{
	SMG_Show(table[yi],0);
	Delay1ms();
	
	SMG_Show(table[er],1);
	Delay1ms();
}

void display2(uchar san,uchar si)
{
	SMG_Show(table[san],2);
	Delay1ms();
	
	SMG_Show(table[si],3);
	Delay1ms();
}

void display3(uchar wu,uchar liu)
{
	SMG_Show(table[wu],4);
	Delay1ms();
	
	SMG_Show(table[liu],5);
	Delay1ms();
}

void display4(uchar qi,uchar ba)
{
	SMG_Show(table[qi],6);
	Delay1ms();
	
	SMG_Show(table[ba],7);
	Delay1ms();
}

void Delay1ms()		//@11.0592MHz
{
	unsigned char i, j;

	_nop_();
	_nop_();
	_nop_();
	i = 11;
	j = 190;
	do
	{
		while (--j);
	} while (--i);
}

void key4()
{
	if(P30==0)
	{
		Delay100us();
		if(P30==0)
		{
			_10ml=0;
			wu=0;liu=0;qi=0;ba=0;
			ET0=1;EA=1;
			Init_HC138(5);
			P0=0x10;
		}
		while(!P30);
	}
	
		if(P31==0)
		{
			Delay100us();
		if(P31==0)
		{

			ET0=0;EA=0;
			Init_HC138(5);
			P0=0x00;
			
			jiage=5*_10ml;
			wu=jiage/10000;liu=jiage%10000/1000+10;qi=jiage%1000/100;ba=jiage%100/10;
		}
		while(!P31);
	}
}
void Delay100us()		//@11.0592MHz
{
	unsigned char i, j;

	_nop_();
	_nop_();
	i = 2;
	j = 15;
	do
	{
		while (--j);
	} while (--i);
}

//============================================================
void Timer0Init(void)		//5毫秒@11.0592MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x00;		//设置定时初值
	TH0 = 0x28;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
}

void Timer0() interrupt 1 //5ms
{
	tt++;
	if(tt==20)
	{
		tt=0;
		_10ml++;
		wu=_10ml/1000;liu=_10ml%1000/100+10;qi=_10ml%100/10;ba=_10ml%10;
	}
}

//============================================================
void IIC_Delay(unsigned char i)
{
	  i=i*12;
    do{_nop_();}
    while(i--);        
}
//总线启动条件
void IIC_Start(void)
{
    SDA = 1;
    SCL = 1;
    IIC_Delay(DELAY_TIME);
    SDA = 0;
    IIC_Delay(DELAY_TIME);
    SCL = 0;	
}

//总线停止条件
void IIC_Stop(void)
{
    SDA = 0;
    SCL = 1;
    IIC_Delay(DELAY_TIME);
    SDA = 1;
    IIC_Delay(DELAY_TIME);
}



//等待应答
bit IIC_WaitAck(void)
{
    bit ackbit;
	
    SCL  = 1;
    IIC_Delay(DELAY_TIME);
    ackbit = SDA;
    SCL = 0;
    IIC_Delay(DELAY_TIME);
    return ackbit;
}

//通过I2C总线发送数据
void IIC_SendByte(unsigned char byt)
{
    unsigned char i;

    for(i=0; i<8; i++)
    {
        SCL  = 0;
        IIC_Delay(DELAY_TIME);
        if(byt & 0x80) SDA  = 1;
        else SDA  = 0;
        IIC_Delay(DELAY_TIME);
        SCL = 1;
        byt <<= 1;
        IIC_Delay(DELAY_TIME);
    }
    SCL  = 0;  
}

//从I2C总线上接收数据
unsigned char IIC_RecByte(void)
{
    unsigned char i, da;
    for(i=0; i<8; i++)
    {   
    	SCL = 1;
	IIC_Delay(DELAY_TIME);
	da <<= 1;
	if(SDA) da |= 1;
	SCL = 0;
	IIC_Delay(DELAY_TIME);
    }
    return da;    
}

uchar AD_read()
{
	uchar temp;
	IIC_Start();
	IIC_SendByte(0x90);
	IIC_WaitAck();
	IIC_SendByte(0x01);
	IIC_WaitAck();
	IIC_Stop();
	
	IIC_Start();
	IIC_SendByte(0x91);
	IIC_WaitAck();
	temp=IIC_RecByte();
	IIC_Stop();
	
	return temp;
}
//=============================================================================

void main()
{
	Init_All();
	Timer0Init();
	while(1)
	{
//		guang=AD_read();
//		yi=1;er=2;san=3;si=4;wu=5;liu=1;qi=guang/10;ba=guang%10;
		
		yi=20;er=10;san=5;si=0;
		

		
		if(guang<64)
		{
			Init_HC138(4);
			P0=0xfe;
		}
		else 
		{
			Init_HC138(4);
			P0=0xff;
		}
		key4();
		display1(yi,er);
		display2(san,si);
		display3(wu,liu);
		display4(qi,ba);
		Init_HC138(6);
		P0=0x00;
	}
}

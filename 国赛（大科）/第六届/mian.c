/*******************************************************************************  
* �ļ����ƣ����ű���Ƭ���������������--��Ƶ�����Ӧ����
* ʵ��Ŀ�ģ��������ϴ���ϵͳ       
* ����˵���������˽��йر�����������Ƶ���⣬�������������\
*					 ��ӭ�����Ա�������ַ��shop117015787.taobao.com
* ���ڰ汾��2016-3/V1.0
*******************************************************************************/
#include<STC15F2K60S2.H>
#include<DEPUTY.H>
#include <intrins.h>


uchar code tab[]={0XC0,0XF9,0XA4,0XB0,0X99,0X92,0X82,0XF8,0X80,0X90,0XBF,0XFF};
uchar yi,er,san,si,wu ,liu,qi,ba;

#define somenop {_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_(); _nop_();}
sbit TX = P1^0;  //��������
sbit RX = P1^1;  //��������
unsigned int t = 0;
unsigned int intr = 0;
bit s_flag;
unsigned int distance;

uchar leibie=0,kezai=0,chuansong,miao,miao_1,miao_2,jiting;
uchar shan=0;
uchar AD_num;
unsigned int miao_t;
unsigned int jiting_t;

uchar shezhi=0;

void delayms(int ms);
void allinit();
void keyscan();
void display1(uchar yi,uchar er);
void display2(uchar san,uchar si);
void display3(uchar wu,uchar liu);
void display4(uchar qi,uchar ba);

void send_wave(void);
void Timer0Init(void)	;
void Timer1Init(void);
void Ultrasonic(void);

void main()
{
	allinit();
	yi=8;er=0;san=10;si=3;wu=0;liu=11;qi=0;ba=0;
	
//	EEPROM_Write(0X00,2);delayms(2);
//	EEPROM_Write(0X01,4);delayms(2);
	
	miao_1=EEPROM_Read(0X00);delayms(2);
	miao_2=EEPROM_Read(0X01);delayms(2);
	
	Timer0Init();
  Timer1Init();
	EA = 1;
	ET0 = 1;  //�򿪶�ʱ��0�ж�
	while(1)
	{
		AD_num=AD_Read(0X03);
		if((AD_num>0)&&(AD_num<51))
		{
			if(shezhi==0)
			{
				yi=11;er=11;san=11;si=11;wu=11;liu=11;qi=11;ba=11;
			}
			else if(shezhi==1)
			{
				if(shan<100){yi=3;er=11;san=11;si=miao_1/10;wu=miao_1%10;liu=11;qi=miao_2/10;ba=miao_2%10;shan++;}
				else {yi=3;er=11;san=11;si=11;wu=11;liu=11;qi=miao_2/10;ba=miao_2%10; shan++; if(shan>=200)shan=0;}
				
			}
			else if(shezhi==2)
			{
				if(shan<100){yi=3;er=11;san=11;si=miao_1/10;wu=miao_1%10;liu=11;qi=miao_2/10;ba=miao_2%10;shan++;}
				else {yi=3;er=11;san=11;si=miao_1/10;wu=miao_1%10;liu=11;qi=11;ba=11; shan++; if(shan>=200)shan=0;}
				
			}
			P2=0X80;P0=0XFE;
			P2=0XA0;P0=0X00; 
			kezai=0;
		}
		else if((AD_num>=51)&&(AD_num<204))
		{
			P2=0X80;P0=0XFD;
			kezai=1;
			if((chuansong==0)&&(jiting==0))
			{
				P2=0XA0;P0=0X00; 
				Ultrasonic();
				if(distance<=30){leibie=1;miao=miao_1;}
				else {leibie=2;miao=miao_2;}
				yi=1;er=11;san=11;si=distance%100/10;wu=distance%10;liu=11;qi=11;ba=leibie;
			}
			else if((chuansong==1)||(jiting==1))
			{
				if(miao>0)
				{
					yi=2;er=11;san=11;si=11;wu=11;liu=11;qi=miao/10;ba=miao%10;
					if(jiting==0){P2=0XA0;P0=0X10;}			
				}
				if(miao==0){chuansong=0;P2=0XA0;P0=0X00;}
				
				if(jiting==1)
				{
					if(jiting_t<=250){P2=0X80;P0=0XF7;}
					else {P2=0X80;P0=0XFF;}
				}
			}
		}
		else if(AD_num>=204)
		{
			P2=0XA0;P0=0X40;  
			if(shan<50)
			{
				P2=0X80;P0=0XFB;shan++;
			}
			else if(shan>=50)
			{
				P2=0X80;P0=0XFF;shan++;if(shan>=100)shan=0;
			}
			yi=10;er=10;san=10;si=10;wu=10;liu=10;qi=10;ba=10;
//				Ultrasonic();
//				if(distance<=30){leibie=1;miao=miao_1;}
//				else {leibie=2;miao=miao_2;}
//				yi=1;er=11;san=11;si=distance%100/10;wu=distance%10;liu=11;qi=11;ba=leibie;
			kezai=0;
		}
		
		keyscan();
		display1(yi,er);
		display2(san,si);
		display3(wu,liu);
		display4(qi,ba);
	}
}


void keyscan()
{
	if(P30==0)
	{
		delayms(5);
		if(P30==0)
		{
			if(shezhi==1)
			{
				miao_1++;
				if(miao_1==11)miao_1=1;
			}
			else if(shezhi==2)
			{
				miao_2++;
				if(miao_2==11)miao_2=1;
			}
		}
		while(!P30);
	}
	else if(P31==0)
	{
		delayms(5);
		if(P31==0)
		{
			if(shezhi==0)
			{
				if((AD_num>0)&&(AD_num<51))
				{
					shezhi=1;
				}
			}
			else if(shezhi==1)
			{
				if((AD_num>0)&&(AD_num<51))
				{
					shezhi=2;
				}
			}
			else if(shezhi==2)
			{
				shezhi=0;
				EEPROM_Write(0X00,miao_1);delayms(2);
				EEPROM_Write(0X01,miao_2);delayms(2);
			}
		}
		while(!P31);
	}
	else if(P32==0)
	{
		delayms(5);
		if(P32==0)
		{
			if((jiting==0)&&(chuansong==1))
			{
				jiting=1;chuansong=0;
				P2=0XA0;P0=0X00;
			}
			else if(jiting==1)
			{
				jiting=0;chuansong=1;
			}
		}
		while(!P32);
	}
	else if(P33==0)
	{
		delayms(5);
		if(P33==0)
		{
			if((kezai==1)&&(chuansong==0))
			{
				chuansong=1;
			}
		}
		while(!P33);
	}
}

void Ultrasonic(void)
{
	if(s_flag) // 200�������һ������ 
	{
		s_flag = 0;
		send_wave();  //���ͷ����ź�
		TR1 = 1;  //������ʱ
		while((RX == 1) && (TF1 == 0));  //�ȴ��յ�����
		TR1 = 0;  //�رռ�ʱ		
		
		if(TF1 == 1)//�������
		{
			TF1 = 0;
			distance = 9999;  //�޷���
		}
		else
		{		
			t = TH1;// ����ʱ��  
			t <<= 8;
			t |= TL1;
			distance = (unsigned int)(t*0.017);  //�������	
			distance = distance/12;
			//t*340   // t/1000000*340/2*100
		}
		TH1 = 0;
		TL1 = 0;
	}
}

void Timer0Init(void)		//2����@11.0592MHz
{
	AUXR |= 0x80;		//��ʱ��ʱ��1Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TL0 = 0x9A;		//���ö�ʱ��ֵ
	TH0 = 0xA9;		//���ö�ʱ��ֵ
	TF0 = 0;		//���TF0��־
	TR0 = 1;		//��ʱ��0��ʼ��ʱ
}

void Timer1Init(void)		//2����@11.0592MHz
{
	AUXR |= 0x40;		//��ʱ��ʱ��1Tģʽ
	TMOD &= 0x0F;		//���ö�ʱ��ģʽ
}

//��ʱ��0�жϷ�����
void isr_timer_0(void)  interrupt 1  //Ĭ���ж����ȼ� 1
{
	TL0 = 0x9A;		//���ö�ʱ��ֵ
	TH0 = 0xA9;		//���ö�ʱ��ֵ

	if(++intr == 200)
	{
		s_flag = 1;
		intr = 0;
  }
	
	if(chuansong==1)
	{
		if(++miao_t==500)
		{
			miao_t=0;
			if(miao>0)miao--;
		}
	}
	
	if(jiting==1)
	{
		jiting_t++;
		if(jiting_t==500)jiting_t=0;
	}
}

void send_wave(void)
{
	unsigned char i = 8;  //����8������
	
	do
	{
		TX = 1;
		somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;		
		TX = 0;
		somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;		
	}
	while(i--);
}




void delayms(int ms)
{
	int i,j;
	for(i=ms;i>0;i--)
		for(j=845;j>0;j--);
}

void allinit()
{
	P2=0XA0;
	P0=0X00;//�رշ��������̵���
	
	P2=0X80;
	P0=0XFF;//�ر�LED��
	
	P2=0XC0;
	P0=0XFF;//ѡ�����������
	P2=0XFF;
	P0=0XFF;//�ر����������
}

void display1(uchar yi,uchar er)
{
		P2=0XC0;//��λѡ573   U8
		P0=0X01;//ѡ���һ�������
		P2=0XFF;//�򿪶�ѡ573   U7
		P0=tab[yi];
		delayms(1);
		
		P2=0XC0;//��λѡ573   U8
		P0=0X02;//ѡ��ڶ��������
		P2=0XFF;//�򿪶�ѡ573   U7
		P0=tab[er];
		delayms(1);
}	

void display2(uchar san,uchar si)
{
		P2=0XC0;//��λѡ573   U8
		P0=0X04;//ѡ������������
		P2=0XFF;//�򿪶�ѡ573   U7
		P0=tab[san];
		delayms(1);
		
		P2=0XC0;//��λѡ573   U8
		P0=0X08;//ѡ����ĸ������
		P2=0XFF;//�򿪶�ѡ573   U7
		P0=tab[si];
		delayms(1);
}

void display3(uchar wu,uchar liu)
{
		P2=0XC0;//��λѡ573   U8
		P0=0X10;//ѡ���һ�������
		P2=0XFF;//�򿪶�ѡ573   U7
		P0=tab[wu];
		delayms(1);
		
		P2=0XC0;//��λѡ573   U8
		P0=0X20;//ѡ���һ�������
		P2=0XFF;//�򿪶�ѡ573   U7
		P0=tab[liu];
		delayms(1);
}

void display4(uchar qi,uchar ba)
{
		P2=0XC0;//��λѡ573   U8
		P0=0X40;//ѡ���һ�������
		P2=0XFF;//�򿪶�ѡ573   U7
		P0=tab[qi];
		delayms(1);
		
		P2=0XC0;//��λѡ573   U8
		P0=0X80;//ѡ���һ�������
		P2=0XFF;//�򿪶�ѡ573   U7
		P0=tab[ba];
		delayms(1);
}




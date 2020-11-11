/*******************************************************************************  
* �ļ����ƣ����ű���Ƭ�����߽��������--��Ƶ�����Ӧ����
* ʵ��Ŀ�ģ���ѹ��Ƶ�ʲɼ��豸        
* ����˵���������˽��йر�����������Ƶ���⣬�������������\
*					 ��ӭ�����Ա�������ַ��shop117015787.taobao.com
* ���ڰ汾��2016-3/V1.0
*******************************************************************************/

#include<STC15F2K60S2.H>
#include<DEPUTY.H>


uchar code tab[]={0XC0,0XF9,0XA4,0XB0,0X99,0X92,0X82,0XF8,0X80,0X90,0XBF,0XFF};
uchar yi,er,san,si,wu ,liu,qi,ba;
uchar num ,flag;

unsigned int t = 0;
unsigned int intr = 0;
bit s_flag;

unsigned int zheng=0,fan=0,zheng_now=0,fan_now=0,time,freq;
uchar t_flag=0;

uchar low,high;
uint num_p;

uchar jia=0,jian=0;
uchar shizhong=1,dianya=0,pinlv=0,chaxun=0,gongneng=0;
uint vol=0;
uchar shangxian,xiaxian;
uchar shan=0;

uchar cun_shi,cun_fen,cun_miao,cun_lei;

void delayms(int ms);
void allinit();
void keyscan();
void display1(uchar yi,uchar er);
void display2(uchar san,uchar si);
void display3(uchar wu,uchar liu);
void display4(uchar qi,uchar ba);

void Timer0Init(void);		//5΢��@12.000MHz
void Timer1Init(void);		//2����@12.000MHz

void main()
{
	allinit();
	yi=8;er=0;san=10;si=3;wu=0;liu=11;qi=0;ba=0;
	
//	EEPROM_Write(0X00,20);delayms(2);
//	EEPROM_Write(0X01,10);delayms(2);
	shangxian=EEPROM_Read(0X00);delayms(2);
	xiaxian=EEPROM_Read(0X01);delayms(2);
	
	dsinit();
	Timer0Init();		
	Timer1Init();		
	EA = 1;
	ET1 = 1; 
	while(1)
	{
		dsget();
		if(shizhong==1)
		{
			if(gongneng==0)
			{
				yi=shijian[2]/10;er=shijian[2]%10;si=shijian[1]/10;wu=shijian[1]%10;qi=shijian[0]/10;ba=shijian[0]%10;san=10;liu=10;
			}
			else if(gongneng==1)
			{
				if(shijian[0]%2==0)
				{
					yi=shijian[2]/10;er=shijian[2]%10;si=shijian[1]/10;wu=shijian[1]%10;qi=shijian[0]/10;ba=shijian[0]%10;san=10;liu=10;
				}
				else if(shijian[0]%2==1)
				{
					yi=11;er=11;si=shijian[1]/10;wu=shijian[1]%10;qi=shijian[0]/10;ba=shijian[0]%10;san=10;liu=10;					
				}
				if(jia==1)
				{
					jia=0;
					shijian[2]=shijian[2]+1;
					dsinit();
				}
				if(jian==1)
				{
					jian=0;
					shijian[2]=shijian[2]-1;
					dsinit();
				}
			}
			else if(gongneng==2)
			{
				if(shijian[0]%2==0)
				{
					yi=shijian[2]/10;er=shijian[2]%10;si=shijian[1]/10;wu=shijian[1]%10;qi=shijian[0]/10;ba=shijian[0]%10;san=10;liu=10;
				}
				else if(shijian[0]%2==1)
				{
					yi=shijian[2]/10;er=shijian[2]%10;si=11;wu=11;qi=shijian[0]/10;ba=shijian[0]%10;san=10;liu=10;					
				}
				if(jia==1)
				{
					jia=0;
					shijian[1]=shijian[1]+1;
					dsinit();
				}
				if(jian==1)
				{
					jian=0;
					shijian[1]=shijian[1]-1;
					dsinit();
				}
			}
			else if(gongneng==3)
			{
				if(shijian[0]%2==0)
				{
					yi=shijian[2]/10;er=shijian[2]%10;si=shijian[1]/10;wu=shijian[1]%10;qi=shijian[0]/10;ba=shijian[0]%10;san=10;liu=10;
				}
				else if(shijian[0]%2==1)
				{
					yi=shijian[2]/10;er=shijian[2]%10;si=shijian[1]/10;wu=shijian[1]%10;qi=11;ba=11;san=10;liu=10;					
				}
				if(jia==1)
				{
					jia=0;
					shijian[0]=shijian[0]+1;
					dsinit();
				}
				if(jian==1)
				{
					jian=0;
					shijian[0]=shijian[0]-1;
					dsinit();
				}
			}
		}
		else if(dianya==1)
		{
			vol=AD_Read(0x03)*19.6;
			if(gongneng==0)
			{
				yi=10;er=1;san=10;si=11;
				wu=vol/1000;liu=vol%1000/100;qi=vol%100/10;ba=vol%10;
			}
			else if(gongneng==1)
			{
				if(shan<100)
				{
					shan++;
					yi=shangxian/10;er=shangxian%10;wu=xiaxian/10;liu=xiaxian%10;	san=0;si=0;qi=0;ba=0;
				}
				else if(shan>=100)
				{
					shan++;if(shan==200)shan=0;
					yi=11;er=11;wu=xiaxian/10;liu=xiaxian%10;san=11;si=11;
				}	
				if(jia==1)
				{
					jia=0;
					shangxian=shangxian+5;
					EEPROM_Write(0X00,shangxian);delayms(2);
				}
				if(jian==1)
				{
					jian=0;
					shangxian=shangxian-5;
					EEPROM_Write(0X00,shangxian);delayms(2);
				}
			}
			else if(gongneng==2)
			{
				if(shan<100)
				{
					shan++;
					yi=shangxian/10;er=shangxian%10;wu=xiaxian/10;liu=xiaxian%10;	san=0;si=0;qi=0;ba=0;
				}
				else if(shan>=100)
				{
					shan++;if(shan==200)shan=0;
					yi=shangxian/10;er=shangxian%10;wu=11;liu=11;qi=11;ba=11;
				}	
				if(jia==1)
				{
					jia=0;
					xiaxian=xiaxian+5;
					EEPROM_Write(0X01,xiaxian);delayms(2);
				}
				if(jian==1)
				{
					jian=0;
					xiaxian=xiaxian-5;
					EEPROM_Write(0X01,xiaxian);delayms(2);
				}
			}		
			
			if((vol>shangxian)&&(EEPROM_Read(0X13)!=1))
			{
				EEPROM_Write(0x10,shijian[2]);delayms(2);
				EEPROM_Write(0X11,shijian[1]);delayms(2);
				EEPROM_Write(0X12,shijian[0]);delayms(2);
				EEPROM_Write(0X13,1);delayms(2);
			}
			if((vol<xiaxian)&&(EEPROM_Read(0X13)!=0))
			{
				EEPROM_Write(0x10,shijian[2]);delayms(2);
				EEPROM_Write(0X11,shijian[1]);delayms(2);
				EEPROM_Write(0X12,shijian[0]);delayms(2);
				EEPROM_Write(0X13,0);delayms(2);
			}
		}
		else if(pinlv==1)
		{
			yi=10;er=2;san=10;
			time=(zheng_now+fan_now)*5;
			freq=1000000/time;
			if(gongneng==0)
			{
				si=freq/10000;wu=freq%10000/1000;liu=freq%1000/100;qi=freq%100/10;ba=freq%10;
			}
			else if(gongneng==1)
			{
				si=time/10000;wu=time%10000/1000;liu=time%1000/100;qi=time%100/10;ba=time%10;
			}	
		}
		else if(chaxun==1)
		{
			if(gongneng==0)
			{
				yi=11;er=11;san=11;si=11;wu=11;liu=11;qi=cun_lei/10;ba=cun_lei%10;
			}
			else if(gongneng==1)
			{
				san=10;liu=10;
				yi=cun_shi/10;er=cun_shi%10;si=cun_fen/10;wu=cun_fen%10;qi=cun_miao/10;ba=cun_miao%10;
			}
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
	uchar temp;
	P3=0XFF;P4=0XEF;
	//P44=0;P42=1;P30=1;P31=1;P32=1;P33=1;
	temp=P3;
	temp=temp&0x1f;//0011 1111//0001 1111
	while(temp!=0x1f)
	{
		delayms(1);
		temp=P3;
		temp=temp&0x1f;
		while(temp!=0x1f)
		{
			temp=P3;
			switch(temp)
			{
				case 0xfe:shizhong=1,dianya=0,pinlv=0,chaxun=0;gongneng=0;break;
				case 0xfd:shizhong=0,dianya=1,pinlv=0,chaxun=0;gongneng=0;break;
				case 0xfb:shizhong=0,dianya=0,pinlv=1,chaxun=0;gongneng=0;break;
				case 0xf7:
					if(shizhong==1)
					{
						if(gongneng==0)gongneng=1;
						else if(gongneng==1)gongneng=2;
						else if(gongneng==2)gongneng=3;
						else if(gongneng==3)gongneng=0;
					}
					else if(dianya==1)
					{
						if(gongneng==0)gongneng=1;
						else if(gongneng==1)gongneng=2;
						else if(gongneng==2)gongneng=1;
					}
					else if(pinlv==1)
					{
						if(gongneng==0)gongneng=1;
						else if(gongneng==1)gongneng=0;
					}	
					else if(chaxun==1)
					{
						if(gongneng==0)gongneng=1;
						else if(gongneng==1)gongneng=0;
					}	
					break;
			}			
			while(temp!=0x1f)
			{
				temp=P3;
				temp=temp&0x1f;
			}
		}
	}
	
	P3=0XFF;P4=0XFB;
	//P44=1;P42=0;P30=1;P31=1;P32=1;P33=1;
	temp=P3;
	temp=temp&0x1f;
	while(temp!=0x1f)
	{
		delayms(1);
		temp=P3;
		temp=temp&0x1f;
		while(temp!=0x1f)
		{
			temp=P3;
			switch(temp)
			{
				case 0xfe:jia=1;break;
				case 0xfd:jian=1;break;
				case 0xfb:shizhong=0,dianya=0,pinlv=0,chaxun=1;gongneng=0;
				cun_shi=EEPROM_Read(0X10);delayms(2);
				cun_fen=EEPROM_Read(0X11);delayms(2);
				cun_miao=EEPROM_Read(0X12);delayms(2);
				cun_lei=EEPROM_Read(0X13);delayms(2);	
					break;
			}
			while(temp!=0x1f)
			{
				temp=P3;
				temp=temp&0x1f;
			}
		}
	}
}


void Timer0Init(void)		//5΢��@12.000MHz
{
	AUXR |= 0x80;		//��ʱ��ʱ��1Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TL0 = 0xC4;		//���ö�ʱ��ֵ
	TH0 = 0xFF;		//���ö�ʱ��ֵ
	TF0 = 0;		//���TF0��־
}

void Timer0_Rountine(void) interrupt 1
{
	if(P34==0)
	{
		zheng++;
		if(t_flag==1)
		{
			t_flag=0;
			fan_now=fan;
			fan=0;
		}
	}
	else if(P34==1)
	{
		fan++;
		if(t_flag==0)
		{
			t_flag=1;
			zheng_now=zheng;
			zheng=0;
		}
	}

//	if(P34==0)
//	{
//		if(high==1)
//		{
//			time=num_p;
//			num_p=0;
//			high=0;
//		}
//		num_p++;
//	}
//	else if(P34==1)
//	{
//		num_p++;
//		high=1;
//	}
	
}

void Timer1Init(void)		//2����@12.000MHz
{
	AUXR |= 0x40;		//��ʱ��ʱ��1Tģʽ
	TMOD &= 0x0F;		//���ö�ʱ��ģʽ
	TL1 = 0x40;		//���ö�ʱ��ֵ
	TH1 = 0xA2;		//���ö�ʱ��ֵ
	TF1 = 0;		//���TF1��־
	TR1 = 1;		//��ʱ��1��ʼ��ʱ
}

void Timer1_Rountine(void) interrupt 3
{
	if(pinlv==1)
	{
		intr++;
		if(intr==495)
		{
			TR0=1;ET0 = 1;
		}
		else if(intr== 500)
		{
			intr = 0;
			TR0=0;ET0 = 0;
		}
	}
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




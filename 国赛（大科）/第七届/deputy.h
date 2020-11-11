#ifndef __DEPUTY_H_
#define __DEPUTY_H_


#define uchar unsigned char 
#define uint unsigned int	

extern uchar shijian[];
	
void Write_Ds1302_Byte(unsigned  char temp);
void Write_Ds1302( unsigned char address,unsigned char dat );
unsigned char Read_Ds1302 ( unsigned char address );

void dsinit(void);
void dsget(void);

void IIC_Start(void); 
void IIC_Stop(void);  
void IIC_Ack(bit ackbit); 
void IIC_SendByte(unsigned char byt); 
bit IIC_WaitAck(void);  
unsigned char IIC_RecByte(void); 
void  EEPROM_Write(uchar add,uchar date);
uchar EEPROM_Read(uchar add);
uchar AD_Read(uchar add);



#endif
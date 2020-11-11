#ifndef _IIC_H
#define _IIC_H

#define uchar unsigned char 
#define uint unsigned int	

void IIC_Start(void); 
void IIC_Stop(void);  
bit IIC_WaitAck(void);  
void IIC_SendAck(bit ackbit); 
void IIC_SendByte(unsigned char byt); 
unsigned char IIC_RecByte(void); 

uchar EEPROM_read(uchar add);
void EEPROM_write(uchar add,uchar dat);
void AD_write(uchar dat);

#endif
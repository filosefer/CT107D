#ifndef _IIC_H
#define _IIC_H

//º¯ÊýÉùÃ÷
void IIC_Start(void); 
void IIC_Stop(void);  
void IIC_Ack(bit ackbit); 
void IIC_SendByte(unsigned char byt); 
bit IIC_WaitAck(void);  
unsigned char IIC_RecByte(void); 
unsigned char AD_read(unsigned char add);
unsigned char EEPROM_read(unsigned char add);
void EEPROM_write(unsigned char add,unsigned char dat);

#endif
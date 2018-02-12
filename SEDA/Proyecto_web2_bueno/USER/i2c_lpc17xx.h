//
//  LPC17xx.h
//  
//
//  Created by Boby Nicusor Constantin on 6/1/18.
//

#ifndef LPC17xx_h
#define LPC17xx_h

#include <LPC17xx.H>

void I2Cdelay(void);
void pulso_SCL(void);
void I2CSendByte(unsigned char byte);
void I2CSendAddr(unsigned char addr, unsigned char rw);
unsigned char I2CGetByte(unsigned char ACK);
void I2CSendStop(void);


#endif /* LPC17xx_h */

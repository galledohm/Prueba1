#include "DS1621.h"
#include <string.h>
#include <i2c_lpc17xx.h>
#include <stdlib.h>

void config_DS1621(void)
{
	I2CSendAddr(0x48,0);			//Dir.Slave 0x48(A2=A1=A0=0) + escritura
	I2CSendByte(0xAC);				//Acceso al reg.onfiguración
	I2CSendByte(0x02);				//Modo conversión continua --> según el reg.conf datasheet esto debería ser 0x01(?)
	I2CSendStop();
	I2Cdelay();
	I2CSendAddr(0x48,0);			//Dir.Slave 0x48(A2=A1=A0=0) + lectura
	I2CSendByte(0xEE);				//Inicio conversión, continua, de la temperatura
	I2CSendStop();
}

float leer_DS1621()
{
	char*test="25.5";
	signed char entero,decimal;
	float f_entero=0.0,f_decimal=0.0;
	entero=atof(test);
	I2CSendAddr(0x48,0);													//Dir.Slave 0x48(A2=A1=A0=0) + escritura
	I2CSendByte(0xAA);														//Leemos temperatura
	I2CSendAddr(0x48,1);
	entero = I2CGetByte('0');		//Leemos parte entera de la temperatura (8b de mayor peso)	
	decimal = (I2CGetByte('1') >> 7);				//Leemos parte decimal de la temperatura (8ºbit de los ocho bits de menor peso)
	I2CSendStop();																//Bus en reposo
	f_entero=(float)entero;
	f_decimal =(float)decimal;
	if(decimal==1){
		if(f_entero>0)
			{f_entero=f_entero+0.5;}
		else 
			{f_entero=f_entero-0.5;}
	}
	return (f_entero);													//Convertimos a float
}


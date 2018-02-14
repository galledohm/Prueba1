#include "BMP180.h"

void I2CRestart (void)
{
	I2CSendStop();
	I2Cdelay();
}

void check_communication (void)
{
	I2CSendAddr(0x77,0);					//Dirección slave + escritura
	I2CSendByte(0xD0);						//Leemos parámetro ID para comprobar comunicación con dispositivo
	I2Cdelay();
	I2CSendAddr(0x77,1);					//Dirección slave + lectura
	ID_BMP = I2CGetByte('1');		
	I2CSendStop();
}

void read_calibration_data (void)
{
	//AC1
	I2CSendAddr(0x77,0);					//Dirección slave + escritura
	I2CSendByte(0xAA);						//Leemos parámetro AC1-MSB
	I2Cdelay();
	I2CSendAddr(0x77,1);					//Dirección slave + lectura
	AC1 = I2CGetByte('1') << 8;		//MSB
	I2CSendStop();
	I2CSendAddr(0x77,0);					//Dirección slave + escritura
	I2CSendByte(0xAB);						//Leemos parámetro AC1-LSB
	I2Cdelay();
	I2CSendAddr(0x77,1);					//Dirección slave + lectura
	AC1 += I2CGetByte('1');				//LSB
	I2CRestart();
	
	//AC2
	I2CSendAddr(0x77,0);					//Dirección slave + escritura
	I2CSendByte(0xAC);						//Leemos parámetro AC2-MSB
	I2Cdelay();
	I2CSendAddr(0x77,1);					//Dirección slave + lectura
	AC2 = I2CGetByte('1') << 8;		//MSB
	I2CSendStop();
	I2CSendAddr(0x77,0);					//Dirección slave + escritura
	I2CSendByte(0xAD);						//Leemos parámetro AC2-LSB
	I2Cdelay();
	I2CSendAddr(0x77,1);					//Dirección slave + lectura
	AC2 += I2CGetByte('1');				//LSB
	I2CRestart();
	
	//AC3
	I2CSendAddr(0x77,0);					//Dirección slave + escritura
	I2CSendByte(0xAE);						//Leemos parámetro AC3-MSB
	I2Cdelay();
	I2CSendAddr(0x77,1);					//Dirección slave + lectura
	AC3 = I2CGetByte('1') << 8;		//MSB
	I2CSendStop();
	I2CSendAddr(0x77,0);					//Dirección slave + escritura
	I2CSendByte(0xAF);						//Leemos parámetro AC3-LSB
	I2Cdelay();
	I2CSendAddr(0x77,1);					//Dirección slave + lectura
	AC3 += I2CGetByte('1');				//LSB
	I2CRestart();
	
	//AC4
	I2CSendAddr(0x77,0);					//Dirección slave + escritura
	I2CSendByte(0xB0);						//Leemos parámetro AC4-MSB
	I2Cdelay();
	I2CSendAddr(0x77,1);					//Dirección slave + lectura
	AC4 = I2CGetByte('1') << 8;		//MSB
	I2CSendStop();
	I2CSendAddr(0x77,0);					//Dirección slave + escritura
	I2CSendByte(0xB1);						//Leemos parámetro AC4-LSB
	I2Cdelay();
	I2CSendAddr(0x77,1);					//Dirección slave + lectura
	AC4 += I2CGetByte('1');				//LSB
	I2CRestart();
	
	//AC5
	I2CSendAddr(0x77,1);					//Dirección slave + escritura
	I2CSendByte(0xB2);						//Leemos parámetro AC5-MSB
	I2Cdelay();
	I2CSendAddr(0x77,1);					//Dirección slave + lectura
	AC5 = I2CGetByte('1') << 8;		//MSB
	I2CSendStop();
	I2CSendAddr(0x77,1);					//Dirección slave + escritura
	I2CSendByte(0xB3);						//Leemos parámetro AC5-LSB
	I2Cdelay();
	I2CSendAddr(0x77,1);					//Dirección slave + lectura
	AC5 += I2CGetByte('1');				//LSB
	I2CRestart();
	
	//AC6
	I2CSendAddr(0x77,0);					//Dirección slave + escritura
	I2CSendByte(0xB4);						//Leemos parámetro AC6-MSB
	I2Cdelay();
	I2CSendAddr(0x77,1);					//Dirección slave + lectura
	AC6 = I2CGetByte('1') << 8;		//MSB
	I2CSendStop();
	I2CSendAddr(0x77,0);					//Dirección slave + escritura
	I2CSendByte(0xB4);						//Leemos parámetro AC6-LSB
	I2Cdelay();
	I2CSendAddr(0x77,1);					//Dirección slave + lectura
	AC6 += I2CGetByte('1');				//LSB
	I2CRestart();
	
	//B1
	I2CSendAddr(0x77,0);					//Dirección slave + escritura
	I2CSendByte(0xB6);						//Leemos parámetro B1-MSB
	I2Cdelay();
	I2CSendAddr(0x77,1);					//Dirección slave + lectura
	B1 = I2CGetByte('1') << 8;		//MSB
	I2CSendStop();
	I2CSendAddr(0x77,0);					//Dirección slave + escritura
	I2CSendByte(0xB7);						//Leemos parámetro B1-LSB
	I2Cdelay();
	I2CSendAddr(0x77,1);					//Dirección slave + lectura
	B1 += I2CGetByte('1');				//LSB
	I2CRestart();
	
	//B2
	I2CSendAddr(0x77,0);					//Dirección slave + escritura
	I2CSendByte(0xB8);						//Leemos parámetro B2-MSB
	I2Cdelay();
	I2CSendAddr(0x77,1);					//Dirección slave + lectura
	B2 = I2CGetByte('0') << 8;		//MSB
	I2CSendStop();
	I2CSendAddr(0x77,0);					//Dirección slave + escritura
	I2CSendByte(0xB9);						//Leemos parámetro B2-LSB
	I2Cdelay();
	I2CSendAddr(0x77,1);					//Dirección slave + lectura
	B2 += I2CGetByte('1');				//LSB
	I2CRestart();
	
	//MB
	I2CSendAddr(0x77,0);					//Dirección slave + escritura
	I2CSendByte(0xBA);						//Leemos parámetro MB-MSB
	I2Cdelay();
	I2CSendAddr(0x77,1);					//Dirección slave + lectura
	MB = I2CGetByte('1') << 8;		//MSB
	I2CSendStop();
	I2CSendAddr(0x77,0);					//Dirección slave + escritura
	I2CSendByte(0xBB);						//Leemos parámetro MB-LSB
	I2Cdelay();
	I2CSendAddr(0x77,1);					//Dirección slave + lectura
	MB += I2CGetByte('1');				//LSB
	I2CRestart();
	
	//MC
	I2CSendAddr(0x77,1);					//Dirección slave + escritura
	I2CSendByte(0xBC);						//Leemos parámetro MC-MSB
	I2Cdelay();
	I2CSendAddr(0x77,1);					//Dirección slave + lectura
	MC = I2CGetByte('1') << 8;		//MSB
	I2CSendStop();
	I2CSendAddr(0x77,1);					//Dirección slave + escritura
	I2CSendByte(0xBD);						//Leemos parámetro MC-LSB
	I2Cdelay();
	I2CSendAddr(0x77,1);					//Dirección slave + lectura
	MC += I2CGetByte('1');				//LSB
	I2CRestart();
	
	//MD
	I2CSendAddr(0x77,0);					//Dirección slave + escritura
	I2CSendByte(0xBE);						//Leemos parámetro MD-MSB
	I2Cdelay();
	I2CSendAddr(0x77,1);					//Dirección slave + lectura
	MD = I2CGetByte('0') << 8;		//MSB
	I2CSendStop();
	I2CSendAddr(0x77,0);					//Dirección slave + escritura
	I2CSendByte(0xBE);						//Leemos parámetro MD-LSB
	I2Cdelay();
	I2CSendAddr(0x77,1);					//Dirección slave + lectura
	MD += I2CGetByte('1');				//LSB
	I2CSendStop();
}


void read_uncompensated_temp (void)
{
	I2CSendAddr(0x77,0);					//Dirección slave + escritura
	I2CSendByte(0xF4);						//Seleccionamos reg.medidas
	I2CSendByte(0x2E);						//Configuramos para temperatura
	I2Cdelay();										//Esperamos 4.5ms
	I2CSendAddr(0x77,0);					//Dirección slave + escritura
	I2CSendByte(0xF6);						//Leemos MSB Temp
	I2Cdelay();	
	I2CSendAddr(0x77,1);
	UT = I2CGetByte('1') << 8;		//MSB
	I2CSendStop();								
	I2CSendAddr(0x77,0);					//Dirección slave + lectura
	I2CSendByte(0xF7);
	I2Cdelay();	
	I2CSendAddr(0x77,1);
	UT += I2CGetByte('1');				//LSB
}

void read_uncompensated_press (void)
{
	I2CSendAddr(0x77,0);					//Dirección slave + escritura
	I2CSendByte(0xF4);						//Seleccionamos reg.medidas
	I2CSendByte(0x74);						//Configuramos para presión (oss=1 ->Modo std, 7.5ms de conversión,2muestras)
	I2Cdelay();										
	I2Cdelay();										//Esperamos 7.5ms en total
	I2CSendAddr(0x77,0);					//Dirección slave + escritura
	I2CSendByte(0xF6);						//Leemos MSB Temp
	I2Cdelay();	
	I2CSendAddr(0x77,1);
	UP = I2CGetByte('1') << 16;		//MSB
	I2CSendStop();								
	I2CSendAddr(0x77,0);					//Dirección slave + lectura
	I2CSendByte(0xF7);
	I2Cdelay();	
	I2CSendAddr(0x77,1);
	UP += I2CGetByte('1') << 8;				//LSB. Nota: Podríamos leer el XLSB si empleasemos el modo de conversión "Ultra high resolution"
	I2CSendStop();
	
	UP = UP >> (8-1);								//Desplazamos 8-oss (datasheet requirement).
}

int calculate_temp (void)					//Tª calibrada
{
	X1 = (UT - AC6) * AC5/32768;
	X2 = MC * 2048/(X1 + MD);
	B5 = X1 + X2;
	t_bmp = (B5 + 8)/64;
	return (t_bmp*0.1);							//Temperatura en ºC
}

int calculate_press (void)				//Presión calibrada
{
	B6 = B5 - 4000;
	X1 = (B2 * (B6*B6/4096))/2048;
	X2 = AC2 * B6/2048;
	X3 = X1 + X2;
	B3 = (((AC1 * 4 + X3)<< 1)+2)/4;
	X1 = AC3 * B6/8192;
	X2 = (B1*(B6*B6/4096))/65536;
	X3 = ((X1 + X2)+2)/4;
	B4 = AC4*(unsigned int)(X3+32768)/32768;
	B7 = ((unsigned int)UP - B3)*(5000 >> 1);
	
	if (B7 < 0x80000000)
		p_bmp = (B7 * 2)/B4;
	else
		p_bmp = (B7/B4)+2;
	
	X1 = (p_bmp/256)*(p_bmp/256);
	X1 = (X1 * 3038)/65536;
	X2 = (-7357*p_bmp)/65536;
	p_bmp = p_bmp + (X1+X2+3791)/16;
	return ((SEA_LVL_PRESS-p_bmp)*0.01);								//Presión en hPa	
}

float calculate_altitude (void)			//Altitud absoluta respecto al nivel del mar en metros
{
	float altitude = 0.0;
	float pressure = SEA_LVL_PRESS-p_bmp;
	altitude = 44330*(1-pow((pressure/SEA_LVL_PRESS),(1/5.255)));
	return altitude;
}
	
	
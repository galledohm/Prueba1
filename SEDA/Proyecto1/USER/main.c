#include <LPC17xx.H>
#include "init.h"
#include "PWM.h"
#include "DMA.h"
#include <string.h>
#include <stdlib.h>
#include <i2c_lpc17xx.h>
#include "BMP180.h"

#define pi 3.141516
#define radio_spinner 0.008 // (8mm) En metros , para obtener la velocidad en m/s

/* ---------------------------------------------------- Variables ----------------------------------------------------*/
float temp_LM35 = 0, humedad = 0;
volatile uint32_t vel_anemometro = 0;
uint16_t umbral_temp = 25; 									//Límite a partir del cual se activa el ventilador interno (PWM)
int bmp_t, bmp_p;

//UART
char buffer[30];		// Buffer de recepción de 30 caracteres
char *ptr_rx;			// puntero de recepción
char rx_completa;		// Flag de recepción de cadena que se activa a "1" al recibir la tecla return CR(ASCII=13)
char *ptr_tx;			// puntero de transmisión
char tx_completa;		// Flag de transmisión de cadena que se activa al transmitir el caracter null (fin de cadena)
char fin=0;

/* ---------------------------------------------------- Funciones de atención a la interrupción TIMERS ----------------------------------------------------*/

void TIMER0_IRQHandler (void)				// Interrumpe cada segundo
{
	
	LPC_ADC->ADCR|=(1<<16); // BURST=1 --> Cada 65TclkADC se toma una muestra de cada canal comenzando desde el más bajo (bit LSB de CR[0..7])
		
	vel_anemometro = LPC_TIM3->TC * 2 * pi * radio_spinner; // Medida en m/s
	LPC_TIM3->TCR |= 1 << 1; 				// Reset contador (Timer3)
	LPC_TIM3->TCR&= ~(1 << 1); 			// Out Reset contador (si no se mantiene reseteado)
	
	/* Mandar valores por UART
		 Mandar valores a LCD
		 Iniciar conversión ADC (Hay que pensar si vamos a usar modo burst o lo vamos a hacer manual)
	*/
	
	LPC_TIM0->IR|=(1<<1);		// Borrar flag interrupción		
}


/* ---------------------------------------------------- Funciones de atención a la interrupción Externas ----------------------------------------------------*/
void EINT1_IRQHandler(){rec();}
void EINT2_IRQHandler(){play();}

/* ---------------------------------------------------- Funciones de atención a la interrupción ADC ----------------------------------------------------*/
void ADC_IRQHandler(void)
{
	
	LPC_ADC->ADCR&=~(1<<16); // BURST=0     // Deshabilitamos el modo Ráfaga (ojo continua la conversión del siguiente canal) 
  
	//Almacenamos las muestras
	temp_LM35 = (((LPC_ADC->ADDR0 >>4)&0xFFF)*3.3/4095)*100;	//Temperatura LM35 en ºC
	humedad = ((((LPC_ADC->ADDR2 >>4)&0xFFF)*3.3/4095)-0.772)/0.03;	//%Humedad relativa
}

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
	char *temp_integer = NULL, *temp_decimal = NULL, temp[10];
	I2CSendAddr(0x48,0);													//Dir.Slave 0x48(A2=A1=A0=0) + escritura
	I2CSendByte(0xAA);														//Leemos temperatura
	I2CSendAddr(0x48,1);													//Dir.Slave 0x48(A2=A1=A0=0) + lectura
	*temp_integer = I2CGetByte('0');							//Leemos parte entera de la temperatura (8b de mayor peso)	
	*temp_decimal = (I2CGetByte('1') >> 7);				//Leemos parte decimal de la temperatura (8ºbit de los ocho bits de menor peso)
	strcpy(temp,temp_integer);										//Copiamos cadena (parte entera)
	strcat(temp, ".");														//Punto decimal
	strcat(temp, temp_decimal);										//Concatenamos la parte decimal con la entera y el punto
	return (atof(temp));													//Convertimos a float
}



/* --------------------------------------------------------------- Programa Principal ---------------------------------------------------------------*/
int main(void)
{
	float alt;
	NVIC_SetPriority(EINT1_IRQn, 0);		
	NVIC_SetPriority(EINT2_IRQn, 0);
	NVIC_SetPriority(DMA_IRQn, 0);
	NVIC_EnableIRQ(EINT1_IRQn);	
	NVIC_EnableIRQ(EINT2_IRQn);
	init_GPIO();
	init_PWM();
	init_ADC_sensores();
	config_DS1621();
	check_communication();
	read_calibration_data ();
	read_uncompensated_temp();
	read_uncompensated_press ();
	bmp_t = calculate_temp();
	bmp_p = calculate_press();
	alt = calculate_altitude();
	/*
		1. Inicializar pines
		2. Inicializar ADC
		3. Inicializar FSM(?) - Vamos a usar una máquina de estados para crear un menú en el LCD??
		4. Inicializar LCD con LCD_Initialization()
	*/
	
	while (1)
	{
		if ( temp_LM35 > (float)umbral_temp)				//Cuando esté implementado el DS1621 usar su temperatura!!!
			set_ciclo_trabajo_PWM (temp_LM35);
	}
}

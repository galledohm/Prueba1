#include <LPC17xx.H>
#include "init_timers_pins.h"
#include <i2c_lpc17xx.h>

uint32_t canal_LM35,canal_HIH;
volatile uint32_t frec_anemometro;


void init_ADC_sensores(void)
{	
	LPC_SC->PCONP|= (1<<12);					// POwer ON
	LPC_SC->PCLKSEL0&=~(3<<24); 			// CLK ADC = CCLK/4 (Fpclk después del reset) (100 Mhz/4 = 25Mhz)
	//LPC_ADC->ADCR= 0;								//No podemos escribir todo el registro, hay bits reservados
	LPC_ADC->ADCR= (0x01<<2)|					// Canal 2
								 (0x01<<4)|	  	  	// Canal 4
							   (0x01<<8)|		     	// CLKDIV=1   (Fclk_ADC= 25Mhz /(1+1)= 12.5 Mhz)
								 (4<<24)|				    // Inicio de conversión con el Match 1 del Timer 0
								 (0x01<<21);			 	// PDN=1

	LPC_ADC->ADINTEN=(1<<2);					// Hab. interrupción fin de conversión del PENÚLTIMO canal(canal 2)
	NVIC_EnableIRQ(ADC_IRQn);					 
	NVIC_SetPriority(ADC_IRQn,2);			    
}

//He movido el ADC_grabar al archivo del DMA

void TIMER0_IRQHandler (void)				// Interrumpe cada segundo
{
	
	LPC_ADC->ADCR|=(1<<16); // BURST=1 --> Cada 65TclkADC se toma una muestra de cada canal comenzando desde el más bajo (bit LSB de CR[0..7])
	LPC_TIM0->IR|=(1<<1);		// Borrar flag interrupción		
	
	frec_anemometro = LPC_TIM3->TC; //Quedaría obtener la velocidad del viento
	
	/* Mandar valores por UART
		 Mandar valores a LCD
		 Leer frecuencia anemómetro
		 Iniciar conversión ADC (Hay que pensar si vamos a usar modo burst o lo vamos a hacer manual)
		 RESET contador CAP
		 Quitamos RESET contador CAP
		 Quitamos flag de interrupción de este timer
	*/
}

void TIMER1_IRQHandler(void)
{	


	LPC_TIM1->IR|=(1<<1);		// Borrar flag interrupción								
}


void TIMER2_IRQHandler(void)
{	


	LPC_TIM2->IR|=(1<<1);		// Borrar flag interrupción								
}


void TIMER3_IRQHandler(void)
{	


	LPC_TIM3->IR|=(1<<1);		// Borrar flag interrupción								
}

void ADC_IRQHandler(void)
{
	
	LPC_ADC->ADCR&=~(1<<16); // BURST=0     // Deshabilitamos el modo Ráfaga (ojo continua la conversión del siguiente canal) 
  
	//Almacenamos las muestras
	canal_LM35= ((LPC_ADC->ADDR2 >>4)&0xFFF);	// flag DONE se borra automat. al leer ADDR2
	canal_HIH= ((LPC_ADC->ADDR4 >>4)&0xFFF);	// flag DONE se borra automat. al leer ADDR0
}

void init_DAC(void)
{
	LPC_SC->PCLKSEL0|= (0x00<<22); 	 	// CCLK/4 (Fpclk después del reset) (100 Mhz/4 = 25Mhz)	
	LPC_DAC->DACCTRL=0;								// ? 
}

void config_DS1621(void)
{
	I2CSendAddr(0x48,0);
	I2CSendByte(0xAC);
	I2CSendByte(0x02);
	I2CSendStop();
	I2Cdelay();
	I2CSendAddr(0x48,0);
	I2CSendByte(0xEE);
	I2CSendStop();
}

unsigned char leer_DS1621(unsigned char ACK)
{
	I2CSendAddr(0x48,0);
	I2CSendByte(0xAA);
	I2CSendAddr(0x48,1);
	return I2CGetByte(ACK);
}

int main(void)
{
	init_GPIO();
	init_ADC_sensores();
	/*
		1. Inicializar pines
		2. Inicializar ADC
		3. Inicializar FSM(?) - Vamos a usar una máquina de estados para crear un menú en el LCD??
		4. Inicializar LCD con LCD_Initialization()
		5. Calibrar TocuhPanel
	*/
	
	//Bucle espera activa
	
}

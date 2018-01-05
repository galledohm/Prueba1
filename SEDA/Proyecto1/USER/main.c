#include <LPC17xx.H>
#include <Math.h>
#define F_cpu 100e6		// Defecto Keil (xtal=12Mhz)
#define F_pclk F_cpu/4 // Defecto despues del reset
#define F_muestreo 1 // Fs=Hz -- T=1seg

uint32_t canal_LM32,canal_HIH;



	// Inicializaci�n de los pines para los sensores, pulsadores, micr�fono, altavoces, ventilador
static void init_GPIO()
{
	// Sensores anal�gicos y micr�fono	
 	LPC_PINCON->PINSEL1|= (1<<18);  					// AD0.2 PIN0.25 (LM35)
	LPC_PINCON->PINSEL3|= (3<<28)|(3<<30);  	// AD0.4 PIN1.30 (HIH) | AD0.5 PIN1.31 (Micr�fono)
	
	LPC_PINCON->PINMODE1|= (2<<18);						// Se deshabilita el pullup/down
	LPC_PINCON->PINMODE3|= (2<<28)|(2<<30);
	
	// Pulsadores
	LPC_PINCON->PINSEL4|= (1<<22)|(1<<24);		// EINT1 Key1 (Pulsador-grabar) PIN2.11 | EINT2 Key2 (Pulsador-reporoducir) PIN2.12
	LPC_PINCON->PINMODE4|= (2<<22)|(2<<24);		// ���Se deshabilita el pullup/down???
	
	// Anem�metro
	LPC_PINCON->PINSEL1|= (3<<16);						// CAP3.1 PIN0.24 (Anem�metro)
	LPC_PINCON->PINMODE1|= (2<<18);						// Se deshabilita el pullup/down
	
	// Ventilador
	LPC_PINCON->PINSEL3|= (2<<10);						// PWM1.3 PIN1.21 (Ventilador)
	LPC_PINCON->PINMODE3|= (2<<10);						// Se deshabilita el pullup/down

	// Altavoz
	LPC_PINCON->PINSEL1|= (2<<20); 	 					// AOUT PIN0.26 (DAC output - Altavoz)
	LPC_PINCON->PINMODE1|= (2<<20); 					// Deshabilita pullup/pulldown

	// MMAT0.1 del TIMER0 para la conversi�n del ADC de los datos de los sensores anal�gicos.
	LPC_PINCON->PINSEL3|= (3<<26); 						// MAT0.1 PIN1.29

}
// Configuraci�n del timer que controla la conversi�n del ADC para los sensoles anal�gicos
void init_TIMER0(void)
{
	  LPC_SC->PCONP|=(1<<1);						// 
    LPC_TIM0->PR = 0x01;							// Para    
    LPC_TIM0->MCR = 0x10;   					//   
    LPC_TIM0->MR1 = (F_pclk/F_muestreo/2)-1; // DOS Match para iniciar la conversi�n!!!!   
    LPC_TIM0->EMR = 0x00C2;   				// 
    LPC_TIM0->TCR = 0x01;     				//  
}
// Configuraci�n del timer que controla la conversi�n del DAC para reproducir
/*Lo que hab�a pensado era poner este TIMER interrumpiendo cada segundo, para poder usarlo para el anem�metro y para mandar cosas por la UART y al LCD*/
void init_TIMER1(void)
{
	  LPC_SC->PCONP|=(1<<2);						// 
    LPC_TIM1->PR = 0x00;							//    
    LPC_TIM1->MCR = 0x10;   					//   
    LPC_TIM1->MR1 = (F_pclk/F_muestreo/2)-1; // DOS Match para iniciar la conversi�n!!!!   1s
    LPC_TIM1->EMR = 0x00C2;   				// 
    LPC_TIM1->TCR = 0x01;     				//  
}



//TIMER en modo CAP para el anem�metro (ver ejemplo BlackBoard)
void init_TIMER2(void)
{
	  LPC_SC->PCONP|=(1<<22);						// 
    LPC_TIM2->PR = 0x00;							//    
    LPC_TIM2->MCR = 0x10;   					//   
    LPC_TIM2->MR1 = (F_pclk/F_muestreo/2)-1; // DOS Match para iniciar la conversi�n!!!!   
    LPC_TIM2->EMR = 0x00C2;   				// 
    LPC_TIM2->TCR = 0x01;     				//  
}



//Este podr�amos usarle para el DMA
void init_TIMER3(void)
{
	  LPC_SC->PCONP|=(1<<23);						// 
    LPC_TIM3->PR = 0x00;							//    
    LPC_TIM3->MCR = 0x10;   					//   
    LPC_TIM3->MR1 = (F_pclk/F_muestreo/2)-1; // DOS Match para iniciar la conversi�n!!!!   
    LPC_TIM3->EMR = 0x00C2;   				// 
    LPC_TIM3->TCR = 0x01;     				//  
}




//A�adir INIT_PWM

void init_ADC_sensores(void)
{	
	LPC_SC->PCONP|= (1<<12);					// POwer ON
	LPC_SC->PCLKSEL0&=~(3<<24); 			// CLK ADC = CCLK/4 (Fpclk despu�s del reset) (100 Mhz/4 = 25Mhz)
	LPC_ADC->ADCR= 0;
	LPC_ADC->ADCR= (0x01<<2)|					// Canal 2
								 (0x01<<4)|	  	  	// Canal 4
							   (0x01<<8)|		     	// CLKDIV=1   (Fclk_ADC= 25Mhz /(1+1)= 12.5 Mhz)
								 (4<<24)|				    // Inicio de conversi�n con el Match 1 del Timer 0
								 (0x01<<21);			 	// PDN=1

	LPC_ADC->ADINTEN=(1<<2);					// Hab. interrupci�n fin de conversi�n del PEN�LTIMO canal(canal 2)
	NVIC_EnableIRQ(ADC_IRQn);					// 
	NVIC_SetPriority(ADC_IRQn,2);			//     

}

//He movido el ADC_grabar al archivo del DMA

void TIMER0_IRQHandler (void)				// Interrumpe cada segundo
{
	
	LPC_ADC->ADCR|=(1<<16); // BURST=1 --> Cada 65TclkADC se toma una muestra de cada canal comenzando desde el m�s bajo (bit LSB de CR[0..7])
	LPC_TIM0->IR|=(1<<1);		// Borrar flag interrupci�n		
	
	/* Mandar valores por UART
		 Mandar valores a LCD
		 Leer frecuencia anem�metro
		 Iniciar conversi�n ADC (Hay que pensar si vamos a usar modo burst o lo vamos a hacer manual)
		 RESET contador CAP
		 Quitamos RESET contador CAP
		 Quitamos flag de interrupci�n de este timer
	*/
}

void TIMER1_IRQHandler(void)
{	


	LPC_TIM1->IR|=(1<<1);		// Borrar flag interrupci�n								
}


void TIMER2_IRQHandler(void)
{	


	LPC_TIM2->IR|=(1<<1);		// Borrar flag interrupci�n								
}


void TIMER3_IRQHandler(void)
{	


	LPC_TIM3->IR|=(1<<1);		// Borrar flag interrupci�n								
}

void ADC_IRQHandler(void)
{
	
	LPC_ADC->ADCR&=~(1<<16); // BURST=0     // Deshabilitamos el modo R�faga (ojo continua la conversi�n del siguiente canal) 
  
	//Almacenamos las muestras
	canal_LM32= ((LPC_ADC->ADDR2 >>4)&0xFFF);	// flag DONE se borra automat. al leer ADDR2
	canal_HIH= ((LPC_ADC->ADDR4 >>4)&0xFFF);	// flag DONE se borra automat. al leer ADDR0
}

void init_DAC(void)
{
	LPC_SC->PCLKSEL0|= (0x00<<22); 	 	// CCLK/4 (Fpclk despu�s del reset) (100 Mhz/4 = 25Mhz)	
	LPC_DAC->DACCTRL=0;								// ? 
}

int main(void)
{
	init_GPIO();
	/*
		1. Inicializar pines
		2. Inicializar ADC
		3. Inicializar FSM(?) - Vamos a usar una m�quina de estados para crear un men� en el LCD??
		4. Inicializar LCD con LCD_Initialization()
		5. Calibrar TocuhPanel
	*/
	
	//Bucle espera activa
	
}

#include "init_timers_pins.h"

// Inicializaci�n de los pines para los sensores, pulsadores, micr�fono, altavoces, ventilador
void init_GPIO(void)
{
	// Sensores anal�gicos y micr�fono	
 	LPC_PINCON->PINSEL1 |= (1<<18);  					// AD0.2 PIN0.25 (LM35)
	LPC_PINCON->PINSEL3 |= (3<<28)|(3<<30);  	// AD0.4 PIN1.30 (HIH) | AD0.5 PIN1.31 (Micr�fono)
	
	LPC_PINCON->PINMODE1 |= (2<<18);						// Se deshabilita el pullup/down
	LPC_PINCON->PINMODE3 |= (2<<28)|(2<<30);
	
	// Pulsadores
	LPC_PINCON->PINSEL4 |= (1<<22)|(1<<24);		// EINT1 Key1 (Pulsador-grabar) PIN2.11 | EINT2 Key2 (Pulsador-reporoducir) PIN2.12
	LPC_PINCON->PINMODE4 |= (2<<22)|(2<<24);		// ���Se deshabilita el pullup/down???
	
	// Anem�metro
	LPC_PINCON->PINSEL1 |= (3<<16);						// CAP3.1 PIN0.24 (Anem�metro)
	LPC_PINCON->PINMODE1 |= (2<<18);						// Se deshabilita el pullup/down
	
	// Ventilador
	LPC_PINCON->PINSEL3 |= (2<<10);						// PWM1.3 PIN1.21 (Ventilador)
	LPC_PINCON->PINMODE3 |= (2<<10);						// Se deshabilita el pullup/down

	// Altavoz
	LPC_PINCON->PINSEL1 |= (2<<20); 	 					// AOUT PIN0.26 (DAC output - Altavoz)
	LPC_PINCON->PINMODE1 |= (2<<20); 					// Deshabilita pullup/pulldown

	// MMAT0.1 del TIMER0 para la conversi�n del ADC de los datos de los sensores anal�gicos.
	LPC_PINCON->PINSEL3 |= (3<<26); 						// MAT0.1 PIN1.29 �Para qu� queremos tener la se�al que genera ese TIMER en el pin este?

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


//Este podr�amos usarle para el DMA
void init_TIMER2(void)
{
	  LPC_SC->PCONP|=(1<<22);						// 
    LPC_TIM2->PR = 0x00;							//    
    LPC_TIM2->MCR = 0x10;   					//   
    LPC_TIM2->MR1 = (F_pclk/F_muestreo/2)-1; // DOS Match para iniciar la conversi�n!!!!   
    LPC_TIM2->EMR = 0x00C2;   				// 
    LPC_TIM2->TCR = 0x01;     				//  
}



//TIMER en modo CAP para el anem�metro (ver ejemplo BlackBoard)
void init_TIMER3(void)
{
		LPC_TIM3->CTCR|= (1<<1)|(1<<2);	 	// Modo contador, Cuenta en falling edges de CAP3.1 (P0.24)
		LPC_TIM3->TCR |= 1 << 0; 					// Start contador
}

//TIMER para el ventilador
void init_PWM(void)
{}

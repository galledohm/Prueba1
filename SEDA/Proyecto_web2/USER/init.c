#include "init.h"

/*-------------------------------- Inicializaci�n de los pines para los sensores, pulsadores, micr�fono, altavoces, ventilador ----------------------------------*/

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
	LPC_PINCON->PINSEL1 |= (2<<20); 	 				// AOUT PIN0.26 (DAC output - Altavoz)
	LPC_PINCON->PINMODE1 |= (2<<20); 					// Deshabilita pullup/pulldown

	//PWM
	LPC_PINCON->PINSEL3 |= (2<<4);						 // PWM1.1 P1.18
	LPC_PINCON->PINMODE3 |= (2<<4);						 // Deshabilita pullup/pulldown

}


/* ---------------------------------------------------------------------- Configuraci�n TIMERS --------------------------------------------------------------------*/

//Configuraci�n del timer que controla la conversi�n del ADC para los sensoles anal�gicos
void init_TIMER0(void)
{
	  LPC_SC->PCONP |=(1<<1);													// Power-ON TIMER 0
    LPC_TIM0->PR = 0x01;														// Prescaler igual a 1 para tener 1s de freq
    LPC_TIM0->MCR = 0x18;   												// ��Interrumpe y resetea TC cuando se alcance el valor de MR1
    LPC_TIM0->MR1 = (F_pclk/F_muestreo_std/2)-1; 		// DOS Match para iniciar la conversi�n!!!!   
    LPC_TIM0->TCR = 0x01;  
		NVIC_EnableIRQ (TIMER0_IRQn);	// TC y PC habilitados para contar
}

// TIMER1 para grabaci�n (ADC-DMA)
void init_TIMER1(void)
{
	  LPC_SC->PCONP|=(1<<2);													// Power-ON TIMER 1
    LPC_TIM1->PR = 0x00;														// Prescaler igual a 0 
    LPC_TIM1->MCR = 0x18;   												// Reset del TC cuando se alcance el valor de MR1
    LPC_TIM1->MR1 = (F_pclk/F_muestreo_rec/2)-1; 		// DOS Match para iniciar la conversi�n!!!!  
    LPC_TIM1->TCR = 0x03;     											// TC y PC habilitados para contar + reset del timer
		LPC_TIM1->TCR &= ~(1<<1);												// Des-reset
		NVIC_EnableIRQ(TIMER1_IRQn);										//Habilitamos interrupci�n del Timer1
}

//TIMER3 en modo CAP para el anem�metro (ver ejemplo BlackBoard)
void init_TIMER3(void)
{
	  LPC_SC->PCONP|=(1<<23);													// Power-ON TIMER 2
		LPC_TIM3->CTCR|= (1<<1)|(1<<2);	 	// Modo contador, Cuenta en falling edges de CAP3.1 (P0.24)
		LPC_TIM3->TCR |= 1 << 0; 					// Start contador
}


/* ---------------------------------------------------- Configuraci�n ADC ----------------------------------------------------*/

void init_ADC_sensores(void)
{	
	LPC_SC->PCONP |= (1<<12);					// POwer ON
	LPC_SC->PCLKSEL0 &= ~(3<<24); 			// CLK ADC = CCLK/4 (Fpclk despu�s del reset) (100 Mhz/4 = 25Mhz)
	LPC_ADC->ADCR= (0x01<<2)|					// Canal 2
								 (0x01<<4)|	  	  	// Canal 4
							   (0xFF<<8)|		     	// CLKDIV=255   (Fclk_ADC= 25Mhz /(255+1)= 97.65625 Khz)
								 (4<<24)|				    // Inicio de conversi�n con el Match 1 del Timer 0
								 (0x01<<21);			 	// PDN=1

	LPC_ADC->ADINTEN =(1<<2);					// Hab. interrupci�n fin de conversi�n del PEN�LTIMO canal(canal 2)
	NVIC_EnableIRQ (ADC_IRQn);					 
	NVIC_SetPriority (ADC_IRQn,4);			    
}

void init_ADC_grabar(void)
{	
	LPC_SC->PCONP |= (1<<12);						// POwer ON
	LPC_SC->PCLKSEL0 &= ~(3<<24); 			// CLK ADC = CCLK/4 (Fpclk despu�s del reset) (100 Mhz/4 = 25Mhz)
	LPC_ADC->ADCR= 0;
	LPC_ADC->ADCR= (1<<5)|		  	 		// Canal 5
							   (1<<8)|		     		// CLKDIV=1   (Fclk_ADC= 25Mhz /(1+1)= 12.5 Mhz)
								 (7<<24)|				    // Inicio de conversi�n con el Match 1 del Timer 1
								 (1<<21);			 			// PDN=1
  LPC_ADC->ADINTEN= (1<<5);					// Hab. interrupci�n fin de conversi�n canal 5, necesario para DMA
}

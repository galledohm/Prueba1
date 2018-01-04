#include <LPC17xx.H>
#include <Math.h>
#define F_cpu 100e6		// Defecto Keil (xtal=12Mhz)
#define F_pclk F_cpu/4 // Defecto despues del reset
#define F_muestreo 100 // Fs=100Hz 


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

}
// Configuraci�n del timer que controla la conversi�n del ADC para los sensoles anal�gicos
void init_TIMER0(void)
{
	  LPC_SC->PCONP|=(1<<1);						// 
    LPC_TIM0->PR = 0x00;							//    
    LPC_TIM0->MCR = 0x10;   					//   
    LPC_TIM0->MR1 = (F_pclk/F_muestreo/2)-1; // DOS Match para iniciar la conversi�n!!!!   
    LPC_TIM0->EMR = 0x00C2;   				// 
    LPC_TIM0->TCR = 0x01;     				//  
}
// Configuraci�n del timer que controla la conversi�n del DAC para reproducir
/*Lo que hab�a pensado era poner este TIMER interrumpiendo cada segundo, para poder usarlo para el anem�metro y para mandar cosas por la UART y al LCD*/
void init_TIMER1(void)
{
	  LPC_SC->PCONP|=(1<<1);						// 
    LPC_TIM1->PR = 0x00;							//    
    LPC_TIM1->MCR = 0x10;   					//   
    LPC_TIM1->MR1 = (F_pclk/F_muestreo/2)-1; // DOS Match para iniciar la conversi�n!!!!   1s
    LPC_TIM1->EMR = 0x00C2;   				// 
    LPC_TIM1->TCR = 0x01;     				//  
}

//TIMER en modo CAP para el anem�metro (ver ejemplo BlackBoard)
void init_TIMER2(void)
{
	  LPC_SC->PCONP|=(1<<1);						// 
    LPC_TIM2->PR = 0x00;							//    
    LPC_TIM2->MCR = 0x10;   					//   
    LPC_TIM2->MR1 = (F_pclk/F_muestreo/2)-1; // DOS Match para iniciar la conversi�n!!!!   
    LPC_TIM2->EMR = 0x00C2;   				// 
    LPC_TIM2->TCR = 0x01;     				//  
}

//Este podr�amos usarle para el DMA
void init_TIMER3(void)
{
	  LPC_SC->PCONP|=(1<<1);						// 
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
	//LPC_SC->PCLKSEL0|=(1<<24); 			// CLK ADC = CCLK = 100 MHz (Para muestrear a 500kHz)
	LPC_ADC->ADCR= 0;
	LPC_ADC->ADCR= (0x01<<0)|		  	  // Canal 0
	//						 (0x02<<8)|		     	// CLKDIV=2   (Fclk_ADC= 100Mhz /(2+1)= 33.3 Mhz) para muestrear a 500Khz!!!
							   (0x01<<8)|		     	// CLKDIV=1   (Fclk_ADC= 25Mhz /(1+1)= 12.5 Mhz)
								 (4<<24)|				    // Inicio de conversi�n con el Match 1 del Timer 0
								 (0x01<<21);			 	// PDN=1
  LPC_ADC->ADINTEN= (1<<0);					// Hab. interrupci�n fin de conversi�n canal 0
}

//He movido el ADC_grabar al archivo del DMA

void TIMER0_IRQHandler (void)				// Interrumpe cada segundo
{
	/* Mandar valores por UART
		 Mandar valores a LCD
		 Leer frecuencia anem�metro
		 Iniciar conversi�n ADC (Hay que pensar si vamos a usar modo burst o lo vamos a hacer manual)
		 RESET contador CAP
		 Quitamos RESET contador CAP
		 Quitamos flag de interrupci�n de este timer
	*/
}


void init_DAC(void)
{
	LPC_SC->PCLKSEL0|= (0x00<<22); 	 	// CCLK/4 (Fpclk despu�s del reset) (100 Mhz/4 = 25Mhz)	
	LPC_DAC->DACCTRL=0;								// ? 
}

int main(void)
{
	/*
		1. Inicializar pines
		2. Inicializar ADC
		3. Inicializar FSM(?) - Vamos a usar una m�quina de estados para crear un men� en el LCD??
		4. Inicializar LCD con LCD_Initialization()
		5. Calibrar TocuhPanel
	*/
	
	//Bucle espera activa
}
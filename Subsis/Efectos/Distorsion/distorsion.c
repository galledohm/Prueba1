#include <LPC17xx.H>
#define F_cpu 100e6		// Defecto Keil (xtal=12Mhz)
#define F_pclk F_cpu/4 // Defecto despues del reset
#define F_muestreo 50000 // Fs=50kHz (Cada 20us se toma una muestra del canal 0)
//NOTA:Frec.Muestro CDs = 44KHz

/*Parámetros para el DISTORSION*/
uint16_t dist_value=100; 
uint16_t input;


void ADC_IRQHandler(void)
{
	//FUZZ: Se trata de un efecto que, cuando la señal sobrepasa un cierto umbral (la mitad de las muestras +- el fuzz_value en caso de que la señal estuviera optimizada para la entrada del
	//ADC, es decir, 2047+-100), la "clippea" a un valor fijo. En este caso lo llevamos a los valores máximo (4095) y mínimo (0) pero hay que reducir el umbral porque no se ha tratado la señal
	//antes
	
	input = ((LPC_ADC->ADGDR >>4)&0xFFF);		//Tomamos el valor de la muestra
  if (input > 250 + dist_value) input= 2047 + dist_value;		//Clippeamos/Recortamos al máximo
	if (input < 250 - dist_value) input= 2047 - dist_value;			//""									"" al mínimo
	LPC_DAC->DACR = (input >> 2) << 6;		
	 
}

void init_DAC(void)
{
	LPC_PINCON->PINSEL1|= (2<<20); 	 	// DAC output = P0.26 (AOUT)
	LPC_PINCON->PINMODE1|= (2<<20); 	// Deshabilita pullup/pulldown
	LPC_SC->PCLKSEL0|= (0x00<<22); 	 	// CCLK/4 (Fpclk después del reset) (100 Mhz/4 = 25Mhz)	
	LPC_DAC->DACCTRL=0;								// ? 
}

void init_ADC(void)
{	
	LPC_SC->PCONP|= (1<<12);					// POwer ON
	LPC_PINCON->PINSEL1|= (1<<14);  	// ADC input= P0.23 (AD0.0)
	LPC_PINCON->PINMODE1|= (2<<14); 	// Deshabilita pullup/pulldown
	LPC_SC->PCLKSEL0|= (0x00<<8); 		// CCLK/4 (Fpclk después del reset) (100 Mhz/4 = 25Mhz)
	LPC_ADC->ADCR= (0x01<<0)|		  	  // Canal 0
								 (0x01<<8)|		  	  // CLKDIV=1   (Fclk_ADC=25Mhz /(1+1)= 12.5Mhz)
								 (0x01<<21)|			 	// PDN=1
								 (4<<24);				    // Inicio de conversión con el Match 1 del Timer 0
	
	LPC_ADC->ADINTEN= (1<<0)|(1<<8);	// Hab. interrupción fin de conversión canal 0
	NVIC_EnableIRQ(ADC_IRQn);					// ? 
	NVIC_SetPriority(ADC_IRQn,1);			// ?        
}	

/*  Timer 0 en modo Output Compare (reset T0TC on Match 1)
	Counter clk: 25 MHz 	MAT0.1 : On match, Toggle pin/output (P1.29)
  Cada 2 Match se provoca el INICIO DE CONVERSIÓN DEL ADC 
	Habilitamos la salida (MAT0.1) para observar la frecuencia de muestreo del ADC */
						
void init_TIMER0(void)
{
	  LPC_SC->PCONP|=(1<<1);						
	  LPC_PINCON->PINSEL3|= 0x0C000000; 
    LPC_TIM0->PR = 0x00;							   
    LPC_TIM0->MCR = 0x10;   					  
    LPC_TIM0->MR1 = (F_pclk/F_muestreo/2)-1; // Se han de producir DOS Match para iniciar la conversión!!!!   
    LPC_TIM0->EMR = 0x00C2;   				
    LPC_TIM0->TCR = 0x01;     				 
}	

int main(void)
{
	NVIC_SetPriorityGrouping(2);
	init_ADC();
	init_DAC();
	init_TIMER0();
	while(1);
}

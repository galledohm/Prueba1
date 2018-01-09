#include <LPC17xx.H>
#define F_cpu 100e6		// Defecto Keil (xtal=12Mhz)
#define F_pclk F_cpu/4 // Defecto despues del reset
#define F_muestreo 50000 // Fs=50kHz (Cada 20us se toma una muestra del canal 0)
//NOTA:Frec.Muestro CDs = 44KHz

/*Parámetros para el delay*/
#define DELAY_MAX 5340						//valor máximo posible para la memoria con tres arrays, límita bastante el efecto deseado

uint16_t Delay_Buffer1[DELAY_MAX];
uint16_t Delay_Buffer2[DELAY_MAX];
uint16_t Delay_Buffer3[DELAY_MAX];
uint16_t DelayCounter1 = 0;
uint16_t DelayCounter2 = 0;
uint16_t DelayCounter3 = 0;
uint16_t Delay_Depth1 = 5340; 
uint16_t Delay_Depth2 = 2600; 
uint16_t Delay_Depth3 = 1000; 
uint32_t input_signal=0;

void ADC_IRQHandler(void)
{
	//Reverb: Se trata de un efecto muy similar al delay o al echo, pero con la diferencia de que recibes varios desde distintos focos, es decir,
	//recibes varios echos pero con un retardo entre ellos porque al estar en diferentes focos el recorrido sería dintito, mayor para unos y menor para otros 
	
	input_signal = ((LPC_ADC->ADGDR >>4)&0x3FF);		//Valor de la muestra de entrada actual
	
	Delay_Buffer1[DelayCounter1]  = (input_signal + Delay_Buffer1[DelayCounter1])>>1;
	Delay_Buffer2[DelayCounter2]  = (input_signal + Delay_Buffer2[DelayCounter2])>>1;
	Delay_Buffer3[DelayCounter3]  = (input_signal + Delay_Buffer3[DelayCounter3])>>1;
	
  DelayCounter1++;	
	if(DelayCounter1 >= Delay_Depth1) DelayCounter1 = 0; 
	
	DelayCounter2++;	
	if(DelayCounter2 >= Delay_Depth2) DelayCounter2 = 0; 
	
	DelayCounter3++;	
	if(DelayCounter3 >= Delay_Depth3) DelayCounter3 = 0;
	
	LPC_DAC->DACR = ((input_signal + (Delay_Buffer1[DelayCounter1]) + (Delay_Buffer2[DelayCounter2]) + (Delay_Buffer3[DelayCounter3])) >> 2) << 6;	//Empleo un ">>2" para atenuar más la señal que en delay o echo
	 
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
	  LPC_SC->PCONP|=(1<<1);						// ? 
	  LPC_PINCON->PINSEL3|= 0x0C000000; // ?
    LPC_TIM0->PR = 0x00;							// ?   
    LPC_TIM0->MCR = 0x10;   					// ?   
    LPC_TIM0->MR1 = (F_pclk/F_muestreo/2)-1; // Se han de producir DOS Match para iniciar la conversión!!!!   
    LPC_TIM0->EMR = 0x00C2;   				// ? 
    LPC_TIM0->TCR = 0x01;     				// ? 
}	

int main(void)
{
	NVIC_SetPriorityGrouping(2);
	init_ADC();
	init_DAC();
	init_TIMER0();
	while(1);
}


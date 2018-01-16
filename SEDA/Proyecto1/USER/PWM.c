#include "PWM.h"

/* ---------------------------------------------------- Configuración PWM ----------------------------------------------------*/
void init_PWM(void)
{
		LPC_SC->PCONP |=(1<<6);								//PWM1 ON
		LPC_PWM1->MR0 = F_pclk*Tpwm-1;
		LPC_PWM1->MCR |= (1<<1);							//Reset del TC cuando se alcanza el valor de MR0
		LPC_PWM1->PCR |= (1<<11); 						//configurado el ENA3 (PWM1.3 salida por pin P1.21)
		LPC_PWM1->TCR |= (1<<0)|(1<<3); 			//PWMTC y PWMPC habilitados para contar, modo PWM habilitado
}


/* ---------------------------------------------------- Cambio ciclo PWM ----------------------------------------------------*/
void set_ciclo_trabajo_PWM ( float temperatura)
{
	LPC_PWM1->MR1= LPC_PWM1->MR0*temperatura/100; 	//Ciclo de trabajo 
	LPC_PWM1->LER|= (1 << 1)|(1<<0); 								
}


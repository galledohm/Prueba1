#include "PWM.h"

/* ---------------------------------------------------- Configuración PWM ----------------------------------------------------*/
void init_PWM(void)
{
		//Se ha seleccionado PWM1.1 en el P1.18 en el archivo init.c
		LPC_SC->PCONP |=(1<<6);								//PWM1 ON
		LPC_PWM1->MR0 =F_pclk*Tpwm;						//25e6*20e-3 
		LPC_PWM1->MCR |= (1<<1);							//Reset del TC cuando se alcanza el valor de MR0
		LPC_PWM1->PCR |= (1<<9); 							//configurado el ENA1 (PWM1.1 salida por pin P1.18 ó P2.0)
		LPC_PWM1->TCR |= (1<<0)|(1<<3); 			//PWMTC y PWMPC habilitados para contar, modo PWM habilitado
}


/* ---------------------------------------------------- Cambio ciclo PWM ----------------------------------------------------*/
void set_ciclo_trabajo_PWM ( float temperatura)
{
	LPC_PWM1->MR1 = (LPC_PWM1->MR0*temperatura/100); 	//Ciclo de trabajo (necesario usar MR1 ya que es el match que rst el canal PWM1)
	LPC_PWM1->LER |= (1 << 1)|(1<<0); 								
}


#if !defined (_PWM_H_)
#define _PWM_H_

#include <LPC17xx.H>

#define F_cpu 100e6			// Defecto Keil (xtal=12Mhz)
#define F_pclk F_cpu/4 		// Defecto despues del reset
#define Tpwm 20e-3			// Periodo PWM1 (20ms)

void init_PWM(void);
void set_ciclo_trabajo_PWM ( float temperatura);

#endif //_PWM_H_


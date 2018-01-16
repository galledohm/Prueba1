#if !defined (_INIT_H_)
#define _INIT_H_

#include <LPC17xx.H>

#define F_cpu 100e6				// Defecto Keil (xtal=12Mhz)
#define F_pclk F_cpu/4 		// Defecto despues del reset
#define F_muestreo 1 			// Fs=Hz -> T=1seg
#define Tpwm 20e-3				// Periodo PWM1 (20ms)

void init_GPIO(void);
void init_TIMER0(void);
void init_TIMER1(void);
void init_TIMER2(void);
void init_TIMER3(void);
void init_PWM(void);
void init_ADC_sensores(void);
void init_ADC_grabar(void);
void init_DAC(void);

#endif //_INIT_H_


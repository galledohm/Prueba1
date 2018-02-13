/*----------------------------------------------------------------------------
 *      RL-ARM - TCPnet
 *----------------------------------------------------------------------------
 *      Name:    HTTP_DEMO.C
 *      Purpose: HTTP Server demo example
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2011 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

#include <LPC17xx.H>
#include "init.h"
#include "PWM.h"
#include "GLCD.h"
#include <RTL.h>
#include "uart.h"
#include <string.h>
#include <stdlib.h>
#include <Net_Config.h>
#include <i2c_lpc17xx.h>

#define pi 3.141516
#define radio_spinner 0.008 // (8mm) En metros , para obtener la velocidad en m/s

float temp_LM35,temp_DS1621, humedad, presion;
int contador_uart=0,contador_disp=0;
volatile uint32_t vel_anemometro = 0;
uint16_t umbral_temp = 25; 									//Límite a partir del cual se activa el ventilador interno (PWM)
char buff_env[30];
uint8_t text_disp[4];

BOOL LEDrun;
BOOL LCDupdate;
BOOL tick;
U32  dhcp_tout;
U8   lcd_text[2][16+1] = {" ",                /* Buffer for LCD text         */
                          "Waiting for DHCP"};

extern LOCALM localm[];                       /* Local Machine Settings      */
#define MY_IP localm[NETIF_ETH].IpAdr
#define DHCP_TOUT   50                        /* DHCP timeout 5 seconds      */




/*--------------------------- init ------------------------------------------*/


static void init_TCP_NET () {
  /* Add System initialisation code here */ 

  init_TcpNet ();
  /* Setup and enable the SysTick timer for 100ms. */
  SysTick->LOAD = (SystemCoreClock / 10) - 1;
  SysTick->CTRL = 0x05;
}


/*--------------------------- timer_poll ------------------------------------*/


static void timer_poll () {
  /* System tick timer running in poll mode */

  if (SysTick->CTRL & 0x10000) {
    /* Timer tick every 100 ms */
    timer_tick ();
    tick = __TRUE;
  }
}

/*--------------------------- dhcp_check ------------------------------------*/

static void dhcp_check () {
  /* Monitor DHCP IP address assignment. */

  if (tick == __FALSE || dhcp_tout == 0) {
    return;
  }

  if (mem_test (&MY_IP, 0, IP_ADRLEN) == __FALSE && !(dhcp_tout & 0x80000000)) {
    /* Success, DHCP has already got the IP address. */
    dhcp_tout = 0;
    sprintf((char *)lcd_text[0]," IP address:");
    sprintf((char *)lcd_text[1]," %d.%d.%d.%d", MY_IP[0], MY_IP[1],
                                                MY_IP[2], MY_IP[3]);
    LCDupdate = __TRUE;
    return;
  }
  if (--dhcp_tout == 0) {
    /* A timeout, disable DHCP and use static IP address. */
    dhcp_disable ();
    sprintf((char *)lcd_text[1]," DHCP failed    " );
    LCDupdate = __TRUE;
    dhcp_tout = 30 | 0x80000000;
    return;
  }
  if (dhcp_tout == 0x80000000) {
    dhcp_tout = 0;
    sprintf((char *)lcd_text[0]," IP address:");
    sprintf((char *)lcd_text[1]," %d.%d.%d.%d", MY_IP[0], MY_IP[1],
                                                MY_IP[2], MY_IP[3]);
    LCDupdate = __TRUE;
  }
}






/*Funciones propias!!!*/


/* ---------------------------------------------------- Funciones de atención a la interrupción ----------------------------------------------------*/

void TIMER0_IRQHandler (void)				// Interrumpe cada segundo
{
	
	LPC_ADC->ADCR|=(1<<16); // BURST=1 --> Cada 65TclkADC se toma una muestra de cada canal comenzando desde el más bajo (bit LSB de CR[0..7])
		
	vel_anemometro = LPC_TIM3->TC * 2 * pi * radio_spinner; // Medida en m/s
	LPC_TIM3->TCR |= 1 << 1; 				// Reset contador (Timer3)
	LPC_TIM3->TCR&= ~(1 << 1); 			// Out Reset contador (si no se mantiene reseteado)
	if(tx_completa){
				if(contador_uart==0)
				{
					sprintf((char*)buff_env,"Temperatura\n LM35: %.1f C\n DS1621: %.1f C\n\r",temp_LM35,temp_DS1621);
					tx_cadena_UART0(buff_env);
					contador_uart++;	
				}
				else	
				{	
					sprintf((char*)buff_env,"Presion: %.2f hPa\n Humedad: %.2f %%\n\r",presion,humedad);
					tx_cadena_UART0(buff_env);
					contador_uart=0;
				}
		}
	contador_disp++;
	LPC_TIM0->IR|=(1<<1);		// Borrar flag interrupción		
}


void TIMER1_IRQHandler(void)
{	


	LPC_TIM1->IR|=(1<<1);		// Borrar flag interrupción								
}


void TIMER2_IRQHandler(void)
{	


	LPC_TIM2->IR|=(1<<1);		// Borrar flag interrupción								
}


void TIMER3_IRQHandler(void)
{	


	LPC_TIM3->IR|=(1<<1);		// Borrar flag interrupción								
}

void ADC_IRQHandler(void)
{
	
	LPC_ADC->ADCR&=~(1<<16); // BURST=0     // Deshabilitamos el modo Ráfaga (ojo continua la conversión del siguiente canal) 
  
	//Almacenamos las muestras
	temp_LM35 = (((LPC_ADC->ADDR0 >>4)&0xFFF)*3.3/4095)*100;	//Temperatura LM35 en ºC
	humedad = ((((LPC_ADC->ADDR2 >>4)&0xFFF)*3.3/4095)-0.772)/0.03;	//%Humedad relativa
}

void config_DS1621(void)
{
	I2CSendAddr(0x48,0);			//Dir.Slave 0x48(A2=A1=A0=0) + escritura
	I2CSendByte(0xAC);				//Acceso al reg.onfiguración
	I2CSendByte(0x02);				//Modo conversión continua --> según el reg.conf datasheet esto debería ser 0x01(?)
	I2CSendStop();
	I2Cdelay();
	I2CSendAddr(0x48,0);			//Dir.Slave 0x48(A2=A1=A0=0) + lectura
	I2CSendByte(0xEE);				//Inicio conversión, continua, de la temperatura
	I2CSendStop();
}

float leer_DS1621()
{
	char *temp_integer = NULL, *temp_decimal = NULL, temp[10];
	I2CSendAddr(0x48,0);													//Dir.Slave 0x48(A2=A1=A0=0) + escritura
	I2CSendByte(0xAA);														//Leemos temperatura
	I2CSendAddr(0x48,1);													//Dir.Slave 0x48(A2=A1=A0=0) + lectura
	*temp_integer = I2CGetByte('0');							//Leemos parte entera de la temperatura (8b de mayor peso)	
	*temp_decimal = (I2CGetByte('1') >> 7);				//Leemos parte decimal de la temperatura (8ºbit de los ocho bits de menor peso)
	strcpy(temp,temp_integer);										//Copiamos cadena (parte entera)
	strcat(temp, ".");														//Punto decimal
	strcat(temp, temp_decimal);										//Concatenamos la parte decimal con la entera y el punto
	return (atof(temp));													//Convertimos a float
}
/*----------------------DISPLAY----------------------------------------*/
static void init_display ()
{
  /* LCD Module init */
	LCD_Initializtion();
  LCD_Clear(Black);
  GUI_Text(10,10, "ESTACION METEOROLOGICA",Green,Black);
  GUI_Text(10,40, "Temperatura LM35: ",Red,Black);
	GUI_Text(10,60, "Temperatura DS1621: ",Red,Black);
	GUI_Text(10,80, "Humedad: ",Red,Black);
	GUI_Text(10,100, "Presion: ",Red,Black);
}
static void upd_display () {
  /* Update GLCD Module display text. */

	 sprintf((char*)text_disp,"%.1f C",temp_LM35);
   GUI_Text(150,40,text_disp,White,Black);
	 sprintf((char*)text_disp,"%.1f C",temp_DS1621);
   GUI_Text(170,60,text_disp,White,Black);
	 sprintf((char*)text_disp,"%.1f %%",humedad);
	 GUI_Text(80,80,text_disp,White,Black);
	 sprintf((char*)text_disp,"%.2f hPa",presion);
	 GUI_Text(80,100,text_disp,White,Black);
}
int main (void) {
  /* Main Thread of the TcpNet */
	NVIC_SetPriorityGrouping(3);
	NVIC_SetPriority(UART0_IRQn, 0x0);
	NVIC_SetPriority(TIMER0_IRQn, 0x1);
	init_display();
	
	init_TCP_NET(); //Inicia TCP_NET
	//init_GPIO();
	//init_PWM();
	
	//init_ADC_sensores();
	//config_DS1621();
	init_display();
	
	uart0_init(9600);
	tx_cadena_UART0("Estacion meteo\n\r");
	init_TIMER0();
  dhcp_tout = DHCP_TOUT;
  while (1) {
    timer_poll ();
    main_TcpNet ();
    dhcp_check ();
		if(contador_disp > 10){
			upd_display();
			contador_disp=0;
		}
	
	}
}
/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

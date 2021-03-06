#include <stdio.h>
#include <stdlib.h>
#include <RTL.h>
#include <Net_Config.h>
#include <LPC17xx.h>                    /* LPC17xx definitions               */
#include "init.h"
#include "DS1621.h"
#include "PWM.h"
#include "DMA.h"
#include "GLCD.h"
#include "i2c_lpc17xx.h"
#include "uart.h" 
#include "BMP180.h"

/*------------------------- Declaraciones METEO --------------------*/
#define pi 3.141516
#define radio_spinner 0.008 // (8mm) En metros , para obtener la velocidad en m/s

float temp_LM35 = 22.5, temp_DS1621 = 26.0, temp_BMP180 = 30.1, humedad = 22.5, presion = 120.0, altitud = 500.4;
float vel_anemometro = 0;
char buff_env[30];
uint8_t text_disp[4];

/*--------------------------- Declaraciones WEB --------------------------------------------*/

BOOL LCDupdate;
BOOL tick;
U32  dhcp_tout;
U8   lcd_text[2][16+1] = {" ",                /* Buffer for LCD text         */
                          "Waiting for DHCP"};

extern LOCALM localm[];                       /* Local Machine Settings      */
#define MY_IP localm[NETIF_ETH].IpAdr
#define DHCP_TOUT   50                        /* DHCP timeout 5 seconds      */

static void init_display (void);


/*-------------------------- init ------------------------------------------*/

static void init ()
{
	//Funciones de atención a la interrupción
	NVIC_SetPriorityGrouping(3);
	NVIC_SetPriority(UART0_IRQn, 0x0);
	NVIC_SetPriority(TIMER0_IRQn, 0x1);
	NVIC_SetPriority(EINT1_IRQn, 0);		
	NVIC_SetPriority(EINT2_IRQn, 0);
	NVIC_EnableIRQ(EINT1_IRQn);	
	NVIC_EnableIRQ(EINT2_IRQn);
	
	//Inicialización GPIO
	init_GPIO();
	
	//Inicialización PWM
	init_PWM();
	
	//Inicialización ADC para los sensores analógicos
	init_ADC_sensores();
	
	//Configuración DS1621
	config_DS1621();
	
	//Inicialización LCD y TCP
  init_display ();
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


/*--------------------------- upd_display -----------------------------------*/

static void upd_display () {
  /* Update GLCD Module display text. */

   sprintf((char*)text_disp,"%.1f C",temp_LM35);
   GUI_Text(150,40,text_disp,White,Black);
	 sprintf((char*)text_disp,"%.1f C",temp_DS1621);
   GUI_Text(170,60,text_disp,White,Black);
	 sprintf((char*)text_disp,"%.1f C",temp_BMP180);
   GUI_Text(170,80,text_disp,White,Black);
	 sprintf((char*)text_disp,"%d %%",umbral_temp);
	 GUI_Text(80,100,text_disp,White,Black);
	 sprintf((char*)text_disp,"%.2f hPa",presion);
	 GUI_Text(80,120,text_disp,White,Black);
	 sprintf((char*)text_disp,"%.1f C",humedad);
	 GUI_Text(80,140,text_disp,White,Black);
	 sprintf((char*)text_disp,"%.2f m/s",vel_anemometro);
	 GUI_Text(80,160,text_disp,White,Black);
		sprintf((char*)text_disp,"%.2f m",altitud);
	 GUI_Text(80,180,text_disp,White,Black);
	 GUI_Text(60,240,lcd_text[0],White,Red);
   GUI_Text(52,260,lcd_text[1],White,Red);

   //LCDupdate =__FALSE;
}


/*--------------------------- init_display ----------------------------------*/

static void init_display () {
  /* LCD Module init */

	LCD_Initializtion();
  LCD_Clear(Black);
  GUI_Text(10,10, "ESTACION METEOROLOGICA",Green,Black);
  GUI_Text(10,40, "Temperatura LM35: ",Red,Black);
	GUI_Text(10,60, "Temperatura DS1621: ",Red,Black);
	GUI_Text(10,80, "Temperatura BMP180: ",Red,Black);
	GUI_Text(10,100, "Humedad: ",Red,Black);
	GUI_Text(10,120, "Presion: ",Red,Black);
	GUI_Text(10,140, "Umbral : ",Red,Black);
	GUI_Text(10,160, "Viento : ",Red,Black);
	GUI_Text(10,180, "Altitud: ",Red,Black);
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

/* ---------------------------------------------------- Funciones de atención a la interrupción Externas ----------------------------------------------------*/
void EINT1_IRQHandler(){rec_ADC();}
void EINT2_IRQHandler(){play();}

/* ---------------------------------------------------- Funciones de atención a la interrupción ADC ----------------------------------------------------*/
void ADC_IRQHandler(void)
{
	
	LPC_ADC->ADCR&=~(1<<16); // BURST=0     // Deshabilitamos el modo Ráfaga (ojo continua la conversión del siguiente canal) 
  
	//Almacenamos las muestras
	temp_LM35 = (((LPC_ADC->ADDR2 >>4)&0xFFF)*3.3/4095)*10;	//Temperatura LM35 en ºC
	humedad = ((((LPC_ADC->ADDR4 >>4)&0xFFF)*3.3/4095)-0.772)/0.03;	//%Humedad relativa
	temp_DS1621=leer_DS1621();
}

/*------------------------------ TIMER0 ---------------------------------------------*/

void TIMER0_IRQHandler (void)				// Interrumpe cada segundo
{
	//Contadores para enviar datos por UART y LCD
	static U32 contador_uart,contador_disp=0;
	
	//Activación del modo BURST - ADC sensores analógicos
	LPC_ADC->ADCR|=(1<<16); // BURST=1 --> Cada 65TclkADC se toma una muestra de cada canal comenzando desde el más bajo (bit LSB de CR[0..7])
	
	//Lectura velocidad anemómetro
	vel_anemometro = LPC_TIM3->TC * 2 * pi * radio_spinner; // Medida en m/s
	LPC_TIM3->TCR |= 1 << 1; 				// Reset contador (Timer3)
	LPC_TIM3->TCR&= ~(1 << 1); 			// Out Reset contador (si no se mantiene reseteado)
	
	//Lectura datos BMP
	read_uncompensated_temp();
	read_uncompensated_press();
	temp_BMP180 = calculate_temp();
	presion = calculate_press();
	altitud = calculate_altitude();
	
	//TX datos UART y LCD
	if(tx_completa)
	{
		switch(contador_uart){
			case 0:
			{
					sprintf((char*)buff_env,"TEMPERATURAS\n LM35: %.1f C\nDS1621: %.1f C\nBMP018: %.1f C\nUmbral: %d C\n\r",temp_LM35,temp_DS1621,temp_BMP180,umbral_temp);
					tx_cadena_UART0(buff_env);
					contador_uart++;	
				break;
			}
			case 1:
			{	
				sprintf((char*)buff_env,"PRESION: %.2f hPa\n\nHUMEDAD: %.2f %%\n\r",presion,humedad);
				tx_cadena_UART0(buff_env);
				contador_uart++;
				break;
			}
			case 2:
			{	
				sprintf((char*)buff_env,"VIENTO: %.2f m/s\n\nALTITUD: %.2f m\n\r",vel_anemometro,altitud);
				tx_cadena_UART0(buff_env);
				contador_uart=0;
				break;
			}
	}
	if(contador_disp>5)			//Actualizamos display cada 2.5s
	{
		contador_disp=0;
     upd_display ();    
	}
	contador_disp++;
	LPC_TIM0->IR|=(1<<1);		// Borrar flag interrupción		
}
}

/*------------------------------------ MAIN ---------------------------------------------*/

int main (void) {
  //Inicialización general
  init();
	//Baudios UART
	uart0_init(9600);
	tx_cadena_UART0("----ESTACION METEOROLOGICA---\n\r");
	
	//Inicialización TIMER principales T0-adq y tx datos, T3-Anemómetro
	init_TIMER3();
	init_TIMER0();
	
	//Lectura datos calibración BMP180
	check_communication();
	read_calibration_data();
	
  dhcp_tout = DHCP_TOUT;
	
  while (1)
	{
    timer_poll ();
    main_TcpNet ();
    dhcp_check ();
		if ( temp_DS1621 > (float)umbral_temp)									//Se comprueba el umbral de temperatura para
		{
			set_ciclo_trabajo_PWM (temp_DS1621);									//configurar la velocidad del ventilador.
			if (temp_DS1621 > ((float)umbral_temp + 50))						//En caso de una temperatura muy alta, lanzar alarma			
				play();
		}		
  }
}


/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 *      RL-ARM - TCPnet
 *----------------------------------------------------------------------------
 *      Name:    HTTP_DEMO.C
 *      Purpose: HTTP Server demo example
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2011 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <RTL.h>
#include <Net_Config.h>
#include <LPC17xx.h>                    /* LPC17xx definitions               */
#include "init.h"
#include <i2c_lpc17xx.h>
#include "PWM.h"
#include "DMA.h"
#include "GLCD.h"
#include "uart.h" 
//#include "debug_frmwrk.h"

/*-------------------------Declaraciones METEO--------------------*/
#define pi 3.141516
#define radio_spinner 0.008 // (8mm) En metros , para obtener la velocidad en m/s

float temp_LM35=22.5,temp_DS1621=26.0, humedad=22.5, presion=120;
//int contador_uart=0,contador_disp=0;
volatile uint32_t vel_anemometro = 0;
char buff_env[30];
uint8_t text_disp[4];
/*-----------------------------------------------------------------------*/


BOOL LEDrun;
BOOL LCDupdate;
BOOL tick;
U32  dhcp_tout;
U8   lcd_text[2][16+1] = {" ",                /* Buffer for LCD text         */
                          "Waiting for DHCP"};

extern LOCALM localm[];                       /* Local Machine Settings      */
#define MY_IP localm[NETIF_ETH].IpAdr
#define DHCP_TOUT   50                        /* DHCP timeout 5 seconds      */

//static void init_io (void);
static void init_display (void);


/*--------------------------- init ------------------------------------------*/


static void init () {
  /* Add System initialisation code here */ 

//  init_io ();
	
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


/*--------------------------- init_io ---------------------------------------*/

//static void init_io () {

//  /* Configure the GPIO for Push Buttons */
//  LPC_PINCON->PINSEL4 &= 0xFFCFFFFF;
//  LPC_GPIO2->FIODIR   &= 0xFFFFFBFF;

// 
//  /* Configure the GPIO for LED1 and LED2. */
//  LPC_GPIO3->FIODIR   |= (1<<25)|(1<<26);


//  /* Configure UART1 for 115200 baud. */
//  LPC_PINCON->PINSEL0 &= ~((3 << 4)|(3 << 6));
//  LPC_PINCON->PINSEL0 |= (1 << 4)|(1 << 6);
////  LPC_PINCON->PINSEL4 |= 0x0000000A;
//  LPC_UART0->LCR = 0x83;
//  LPC_UART0->DLL = 9;                              /* 115200 Baud Rate @ 25.0 MHZ PCLK */
//  LPC_UART0->FDR = 0x21;                           /* FR 1,507, DIVADDVAL = 1, MULVAL = 2 */
//  LPC_UART0->DLM = 0;
//  LPC_UART0->LCR = 0x03;

//  /* Configure AD0.2 input. */
//  LPC_PINCON->PINSEL1 &= 0xFFF3FFFF;
//  LPC_PINCON->PINSEL1 |= 0x00040000;
//  LPC_SC->PCONP       |= 0x00001000;
//  LPC_ADC->ADCR        = 0x00200404;               /* ADC enable, ADC clock 25MHz/5, select AD0.2 pin */
//}


/*--------------------------- fputc -----------------------------------------*/

int fputc(int ch, FILE *f)  {
  /* Debug output to serial port. */

  if (ch == '\n')  {
    while (!(LPC_UART0->LSR & 0x20));
    LPC_UART0->THR = 0x0D;
  }
  while (!(LPC_UART0->LSR & 0x20));
  LPC_UART0->THR = (ch & 0xFF);
  return (ch);
}


/*--------------------------- LED_out ---------------------------------------*/

void LED_out (U32 val) {
  const U8 led_pos[2] = { 25, 26 }; // LED1 and LED2 (P3.25 y P3.26)
  U32 i,mask;

  for (i = 0; i < 2; i++) {
    mask = 1 << led_pos[i];
    if (val & (1<<i)) {
    LPC_GPIO3->FIOCLR = mask;  // LED ON
    }
    else {
    LPC_GPIO3->FIOSET = mask;  // LED OFF
    }
  }

}




/*--------------------------- get_button ------------------------------------*/

U8 get_button (void) {
  /* Read Mini-DK2 Digital Input */
  U32 val = 0;
    /* ISP button ?*/
  if ((LPC_GPIO2->FIOPIN & (1 << 10)) == 0) val |= 0x01; // P2.10 ?
	
    /* KEY1 button ?*/
  if ((LPC_GPIO2->FIOPIN & (1 << 11)) == 0) val |= 0x02; // P2.11 ?
	
    /* KEY2 button ? */
  if ((LPC_GPIO2->FIOPIN & (1 << 12)) == 0) val |= 0x04; // P2.12 ?
  
  return (val);
}


/*--------------------------- upd_display -----------------------------------*/

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
	 sprintf((char*)text_disp,"%d C",umbral_temp);
	 GUI_Text(80,120,text_disp,White,Black);
	   GUI_Text(60,144,lcd_text[0],White,Red);
   GUI_Text(52,160,lcd_text[1],White,Red);

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
	GUI_Text(10,80, "Humedad: ",Red,Black);
	GUI_Text(10,100, "Presion: ",Red,Black);
	GUI_Text(10,120, "Umbral : ",Red,Black);
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


/*--------------------------- blink_led -------------------------------------*/


static void blink_led () {
  /* Blink the LED1 and LED2 on Mini-DK2 board */
  const U8 led_val[2] = {0x01,0x02};
  static U32 cnt;

  if (tick == __TRUE) {
    /* Every 100 ms */
    tick = __FALSE;
    if (LEDrun == __TRUE) {
      LED_out (led_val[cnt]);
      if (++cnt >= sizeof(led_val)) {
        cnt = 0;
      }
    }
    if (LCDupdate == __TRUE) {
     // upd_display ();
    }
  }
}



/*---------------------------------------------------------------------------*/

void TIMER0_IRQHandler (void)				// Interrumpe cada segundo
{
	static U32 contador_uart,contador_disp=0;
	//LPC_ADC->ADCR|=(1<<16); // BURST=1 --> Cada 65TclkADC se toma una muestra de cada canal comenzando desde el más bajo (bit LSB de CR[0..7])
		
//	vel_anemometro = LPC_TIM3->TC * 2 * pi * radio_spinner; // Medida en m/s
//	LPC_TIM3->TCR |= 1 << 1; 				// Reset contador (Timer3)
//	LPC_TIM3->TCR&= ~(1 << 1); 			// Out Reset contador (si no se mantiene reseteado)
		if(tx_completa){
				if(contador_uart==0)
				{
					sprintf((char*)buff_env,"Temperatura\n LM35: %.1f C\n DS1621: %.1f C\n\r",temp_LM35,temp_DS1621);
					tx_cadena_UART0(buff_env);
					contador_uart++;	
				}
				else if(contador_uart==1)
				{	
					sprintf((char*)buff_env,"Presion: %.2f hPa\n Humedad: %.2f %%\n\r",presion,humedad);
					tx_cadena_UART0(buff_env);
					contador_uart++;
				}
				else if(contador_uart==2)
				{	
					sprintf((char*)buff_env,"Umbral: %d C\n\r",umbral_temp);
					tx_cadena_UART0(buff_env);
					contador_uart=0;
				}
			}
		if(contador_disp>5)
			{
				contador_disp=0;
				// if (LCDupdate == __TRUE) {
      upd_display ();
    
			}

contador_disp++;
LPC_TIM0->IR|=(1<<1);		// Borrar flag interrupción		
}

int main (void) {
  /* Main Thread of the TcpNet */
	NVIC_SetPriorityGrouping(3);
	NVIC_SetPriority(UART0_IRQn, 0x0);
	NVIC_SetPriority(TIMER0_IRQn, 0x1);
  init();
	
	uart0_init(9600);
	tx_cadena_UART0("Estacion meteo\n\r");
	init_TIMER0();
  LEDrun = __TRUE;
  dhcp_tout = DHCP_TOUT;
  while (1) {
    timer_poll ();
    main_TcpNet ();
    dhcp_check ();
    blink_led ();
  }
}



/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

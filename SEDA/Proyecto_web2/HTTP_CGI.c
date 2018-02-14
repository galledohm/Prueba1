/*----------------------------------------------------------------------------
 *      RL-ARM - TCPnet
 *----------------------------------------------------------------------------
 *      Name:    HTTP_CGI.C
 *      Purpose: HTTP Server CGI Module
 *      Rev.:    V4.22
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2011 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

#include <Net_Config.h>
#include <stdio.h>
#include "init.h"
#include <string.h>

int umbral_temp = 25; 									//Límite a partir del cual se activa el ventilador interno (PWM)

/* ---------------------------------------------------------------------------
 * The HTTP server provides a small scripting language.
 *
 * The script language is simple and works as follows. Each script line starts
 * with a command character, either "i", "t", "c", "#" or ".".
 *   "i" - command tells the script interpreter to "include" a file from the
 *         virtual file system and output it to the web browser.
 *   "t" - command should be followed by a line of text that is to be output
 *         to the browser.
 *   "c" - command is used to call one of the C functions from the this file.
 *         It may be followed by the line of text. This text is passed to
 *         'cgi_func()' as a pointer to environment variable.
 *   "#' - command is a comment line and is ignored (the "#" denotes a comment)
 *   "." - denotes the last script line.
 *
 * --------------------------------------------------------------------------*/

/* at_System.c */
extern  LOCALM localm[];
#define LocM   localm[NETIF_ETH]

/* Net_Config.c */
extern struct tcp_cfg   tcp_config;
extern struct http_cfg  http_config;
#define tcp_NumSocks    tcp_config.NumSocks
#define tcp_socket      tcp_config.Scb
#define http_EnAuth     http_config.EnAuth
#define http_auth_passw http_config.Passw

extern BOOL LCDupdate;
extern U8   lcd_text[2][16+1];

/*----------------------------------------------------------------------------
 * HTTP Server Common Gateway Interface Functions
 *---------------------------------------------------------------------------*/

/*--------------------------- cgi_process_var -------------------------------*/

void cgi_process_var (U8 *qs) {
  /* This function is called by HTTP server to process the Querry_String   */
  /* for the CGI Form GET method. It is called on SUBMIT from the browser. */
  /*.The Querry_String.is SPACE terminated.                                */
  U8 *var;
  var = (U8 *)alloc_mem (40);
  do
		{
    /* Loop through all the parameters. */
    qs = http_get_var (qs, var, 40);
    /* Check the returned string, 'qs' now points to the next. */
    if (var[0] != 0)
		{
         if (str_scomp (var, "umbral_t=") == __TRUE) 
            sscanf ((const char *)&var[9], "%d", &umbral_temp);
		}
	} while (qs);
   
  free_mem ((OS_FRAME *)var);
}


/*--------------------------- cgi_process_data ------------------------------*/

void cgi_process_data (U8 code, U8 *dat, U16 len) {
 
}


/*--------------------------- cgi_func --------------------------------------*/

U16 cgi_func (U8 *env, U8 *buf, U16 buflen, U32 *pcgi) {
  
  U32 len = 0;
  switch (env[0])
	{
    case 't' :  //Se actualizan los campos correspondientes a las temperaturas	
		{
			switch (env[2])
			{
				case '1': //Temp sensor analógico LM35
					len= sprintf((char*)buf,(const char*)&env[4],temp_LM35);
				break;	
				
				case '2': //Temp sensor DS1621
				 len= sprintf((char*)buf,(const char*)&env[4],temp_DS1621);
				break;
				
				case '3': //Temp sensor BMP180
				 len= sprintf((char*)buf,(const char*)&env[4],temp_BMP180);
				break;
				
				case '4': //Umbral
				 len= sprintf((char*)buf,(const char*)&env[4],umbral_temp);
				break;
			}	 	 
		 }
		break;
		
		case 'h': //Se actualiza la humedad
					len= sprintf((char*)buf,(const char*)&env[2],humedad);
		break;
		 
		case 'p': //Se actualiza la presión
				len= sprintf((char*)buf,(const char*)&env[2],presion);
		break;
		
		case 'v': //Se actualiza la velocidad del anemómetro
				len= sprintf((char*)buf,(const char*)&env[2],vel_anemometro);
		break;
		
				case 'a': //Se actualiza la altura
				len= sprintf((char*)buf,(const char*)&env[2],altitud);
		break;	
  }
  return ((U16)len);
}


/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

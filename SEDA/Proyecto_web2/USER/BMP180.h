#if !defined (_BMP180_H_)
#define _BMP180_H_

#include "i2c_lpc17xx.h"
#include <math.h>

#define SEA_LVL_PRESS 101325							//Presión a nivel de mar en Pa

static short AC1,AC2,AC3,B1,B2,MB,MC,MD;
static unsigned short AC4,AC5,AC6;
static int UT,UP,X1,X2,X3,B3,B5,B6;
static unsigned int B4,B7;
static float t_bmp,p_bmp;
static char ID_BMP;

void read_calibration_data (void);
void read_uncompensated_temp (void);
void read_uncompensated_press (void);
float calculate_temp (void);
float calculate_press (void);
float calculate_altitude (void);
void I2CRestart (void);
void check_communication(void);


#endif //_BMP180_H_
#include "ad_reader.h"
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define SLOTS_DIR "/sys/devices/bone_capemgr.9/slots"
#define DEVICE "BB-ADC"

/* P9 GPIO USAGE:
 * VDD_3V3 (3) = POWER 2.7 - 5.5
 * AIN0 (39)   = ANALOG OUTPUT, READ FROM IN_VOLTAGE0_RAW
 * GROUND (34) = GNDA_ADC
*/

//Reads from AIN0
#define A2D_FILE_VOLTAGE0  "/sys/bus/iio/devices/iio:device0/in_voltage1_raw"
#define A2D_VOLTAGE_REF_V  1.8
#define A2D_MAX_READING    4095


static FILE *ADReader;	/* File desc for reading the the distance reader */
static char buf[7][5]; 
/* Initializes the AD reader functions */
void AD_init(void) {
	//config the GPIO
	ADReader = fopen(SLOTS_DIR, "w");
	fprintf(ADReader,DEVICE);
	fclose(ADReader);	
}

/* read the distance value */
int AD_read(int channel) {
	if(channel < 0 || channel > 6) {
		return -1;
	}
	char newStr[80];
	sprintf(newStr, "/sys/bus/iio/devices/iio:device0/in_voltage%d", channel);
	strcat(newStr, "_raw");
	ADReader = fopen(newStr, "r");
	fread(buf[channel],sizeof(char),4,ADReader);
	fclose(ADReader);
	return atoi(buf[channel]);
}

int AD_getVoltage0Reading()
{

   FILE *f = fopen(A2D_FILE_VOLTAGE0, "r");
   if (!f) {
      printf("ERROR: Unable to open voltage input file. Cape loaded?\n");
      printf("try:   echo BB_ADC > /sys/devices/bone_capemgr.9/slots\n");
      exit(-1);
   }

   int a2dReading = 0;
   int itemsRead = fscanf(f, "%d", &a2dReading);
   if (itemsRead <= 0) {
      printf("ERROR: Unable to read values from voltage input file.\n");
      exit(-1);
   }
   fclose(f);

   return a2dReading;
}

double AD_getVoltage(int reading) {
   double voltage = ((double)reading / A2D_MAX_READING) * A2D_VOLTAGE_REF_V;
   return voltage;
}

double AD_getTemperature(double voltage) {
   double tempInCelsius = ((voltage*1000)-500) / 10;
   return tempInCelsius;
}

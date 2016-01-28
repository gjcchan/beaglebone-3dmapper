#include "lib.h"
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
/*
	Math library used to calculate the IR distance sensor values and properties due to it's non-linear nature


*/
#define IN_VOLT 1.786
#define DIV_RATIO 0.5714285
#define AD_SCALE 4095
// curve fit formula for sensor 41.543 * (Voltage + 0.30221) ^ -1.5281
#define F_COEFF 41.543
#define F_OFFSET 0.30221
#define F_POW -1.5281

double lib_get_volts(int value) {
	double ratio = (double) value / AD_SCALE;
	fprintf(stderr,"%d %f\n", value, ratio);
	double trueVolt = (IN_VOLT * ratio) / DIV_RATIO;
	return trueVolt;
}
double lib_convert_to_cm(double voltage) {
	double cm = voltage + F_OFFSET;
	cm = pow(cm, F_POW);
	cm = cm * F_COEFF;
	return cm;
}
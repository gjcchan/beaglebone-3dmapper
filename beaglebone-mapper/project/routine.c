#include "lib.h"
#include "ad_reader.h"
#include "pwm_servo.h"
#include "webcam.h"
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/time.h>

#define MAX_NS = 2500000
#define MIN_NS = 470000

#define M_PI 3.1415926535

#define DEFAULT_INT_SIZE 10
#define MAX_INTERVAL_DEG 15
#define X_END 180
#define X_START 0
#define Y_END 110
#define Y_START 210
#define IR_DIST_MAX 80.0
#define IR_DIST_MIN 10.0
#define JSON_SIZE 1000000
#define ARRAY_SIZE 100000
struct point {
	double x;
	double y;
	double z;
};

/*
	Contains all  the routine as well as mathematical conversions to process and control the hardware to parse back the appropriate and converted inputs
*/

static struct point array[ARRAY_SIZE];
static int array_ptr = 0;
static char JSON[JSON_SIZE];
static int running;
static int deg_interval;
static double returnValues[3];

static struct timespec readDelay = {1,0};

static double deg_to_rad(double deg);


void routine_init(void) {
	pwm_set_run(1, 'X'); 
	pwm_set_run(1, 'Y'); 
	deg_interval = DEFAULT_INT_SIZE;
}

void routine_change_deg(int deg) {
	if(deg < MAX_INTERVAL_DEG && deg > 0) {
		deg_interval = deg;
	}
}

void routine_run(void) {
	if(running == 1) {
		return;
	}
	running = 1;	
	double radians_x = 0;
	double radians_y = 0;
	//double radians_y = 0;
	int distance = 0;
	double cm = 0;
	double x_cm = 0;
	double y_cm = 0;
	double z_cm = 0;
	double y_ratio = 1.0;
	int temp_interval = deg_interval;

	for(int j = Y_START; j >= (Y_END+60); j-=temp_interval) {
		pwm_change_deg(j, 'Y');
		y_ratio = ((float)j - (float)Y_END)/((float)Y_START - (float)Y_END);
		nanosleep(&readDelay, (struct timespec *) NULL);
		for (int i = X_START; i <= X_END; i+=temp_interval) {
			pwm_change_deg(i, 'X');
			nanosleep(&readDelay, (struct timespec *) NULL);
			distance = AD_read(0);
			//take pictures of the scene
			if(j == Y_START) {
				if(i == 30) {
					webcam_snap("/root/30.png");
				} else if(i == 90) {
					webcam_snap("/root/90.png");
				} else if(i == 150) {
					webcam_snap("/root/150.png");
				}
			}

			cm = lib_get_volts(distance);
			cm = lib_convert_to_cm(cm);
			//set limits
			if(cm > IR_DIST_MAX) {
				cm = IR_DIST_MAX;
			} else if(cm < IR_DIST_MIN) {
				cm = IR_DIST_MIN;
			}
			//convert the data here
			radians_x = deg_to_rad(i);	
			radians_y = deg_to_rad(fabs(j-Y_START));
			x_cm = cm * cos(radians_x) * y_ratio;
			z_cm = cm * sin(radians_x) * cos(radians_y);
			y_cm = IR_DIST_MAX * sin(radians_y);
			array[array_ptr].x = x_cm;
			array[array_ptr].y = y_cm;
			array[array_ptr].z = z_cm;
			array_ptr++;		
		}			
	}
	webcam_stitch("/root/150.png /root/90.png /root/30.png", "/root/scene.png");
	webcam_send_image();
	running = 0;

}

void routine_halt(void) {
	running = 0;
}

char* routine_return(void) {
	return JSON;
}

static double deg_to_rad(double deg) {
	return deg / (180.0 / M_PI);
}
void routine_print(void) {
	for(int x = 0; x < array_ptr; x++) {
		fprintf(stderr, "y: %f x: %f z: %f\n", array[x].y, array[x].x, array[x].z);
	}
}
//converts to JSON for client-use
char* routine_json(void) {
	char temp[1000];
	sprintf(JSON,"{\"points\":[");
	for(int x = 0; x < array_ptr-1; x++) {
		sprintf(temp, "{\"y\": %f, \"x\": %f, \"z\": %f},", array[x].y, array[x].x, array[x].z);
		strcat(JSON,temp);
	}
	sprintf(temp, "{\"y\": %f, \"x\": %f, \"z\": %f}", array[array_ptr-1].y, array[array_ptr-1].x, array[array_ptr-1].z);
	strcat(JSON,temp);
	sprintf(temp,"]}");
	strcat(JSON,temp);
	array_ptr = 0;
	return JSON;
}

double* routine_getDimensionValues(void){
	// Add first value and second value to calculate dimension
	int distance = 0;
	double cm = 0;
	pwm_change_deg(Y_END, 'Y');
	nanosleep(&readDelay, (struct timespec *) NULL);
	distance = AD_read(0);
	cm = lib_get_volts(distance);
	cm = lib_convert_to_cm(cm);
	//set limits Y
	if(cm > IR_DIST_MAX) {
		cm = IR_DIST_MAX;
	} else if(cm < IR_DIST_MIN) {
		cm = IR_DIST_MIN;
	}
	returnValues[1] = cm;

	pwm_change_deg(Y_START, 'Y');
	nanosleep(&readDelay, (struct timespec *) NULL);

	pwm_change_deg(X_START, 'X');
	nanosleep(&readDelay, (struct timespec *) NULL);
	distance = AD_read(0);
	cm = lib_get_volts(distance);
	cm = lib_convert_to_cm(cm);
	//set limits X
	if(cm > IR_DIST_MAX) {
		cm = IR_DIST_MAX;
	} else if(cm < IR_DIST_MIN) {
		cm = IR_DIST_MIN;
	}
	returnValues[0] = cm;
	pwm_change_deg(X_END, 'X');
	nanosleep(&readDelay, (struct timespec *) NULL);
	distance = AD_read(0);
	cm = lib_get_volts(distance);
	cm = lib_convert_to_cm(cm);
	//set limits otherside of X
	if(cm > IR_DIST_MAX) {
		cm = IR_DIST_MAX;
	} else if(cm < IR_DIST_MIN) {
		cm = IR_DIST_MIN;
	}
	returnValues[0] += cm;

	pwm_change_deg(90, 'X');
	nanosleep(&readDelay, (struct timespec *) NULL);
	distance = AD_read(0);
	cm = lib_get_volts(distance);
	cm = lib_convert_to_cm(cm);
	//set limits Z
	if(cm > IR_DIST_MAX) {
		cm = IR_DIST_MAX;
	} else if(cm < IR_DIST_MIN) {
		cm = IR_DIST_MIN;
	}
	returnValues[2] = cm;
	return returnValues;
}

double routine_getTemperatureValue(void){
	int reading = AD_getVoltage0Reading();
    double voltage = AD_getVoltage(reading);
    double tempInCelsius = AD_getTemperature(voltage);
    return tempInCelsius;
}
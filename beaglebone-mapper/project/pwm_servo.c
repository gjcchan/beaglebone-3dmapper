#include "pwm_servo.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
/* For reference
EXPORT NUMBER	PIN NAME	PINS

	0			EHRPWM0A	P9.22,P9.31
	1			EHRPWM0B	P9.21,P9.29
	2			ECAPPWM0	P9.42
	3			EHRPWM1A	P9.14,P8.36
	4			EHRPWM1B	P9.16,P8.34
	5			EHRPWM2A	P8.19,P8.45
	6			EHRPWM2B	P8.13,P8.46
	7			ECAPPWM2	P9.28
*/
//just pin header 8, pin 13
#define PIN_USE_X "P8.13"
#define PWM_CHANNEL_X "6"

#define PIN_USE_Y "P8.19"
#define PWM_CHANNEL_Y "5"	

#define EXPORT_DIR "/sys/class/pwm/export"
#define SLOT_DIR "/sys/devices/bone_capemgr.9/slots"
#define PIN_CONFIG_DIR "config-pin"

#define CAPE_UNN "cape-universaln"

#define PWM_DIR "/sys/class/pwm/pwm"
#define DUTY_CYCLE_DIR "/duty_ns"
#define PERIOD_CYCLE_DIR "/period_ns"
#define RUN_DIR "/run" 
//20ms period cycle
//servos are not period cycle dependent, so any period cycle will do
#define PERIOD_CYCLE_NS 20000000
//the maximum movement range of servo 
#define DUTY_MAX_NS 2500000
#define DUTY_MIN_NS 470000
#define MAX_DEG 210
#define MIN_DEG 0
/*
	Code for initializing and driving the PWM servers, the functions are calibrated to work in degrees for more precise and easier control
*/

#define DEG_NS (DUTY_MAX_NS - DUTY_MIN_NS) / MAX_DEG

static FILE *PWMsetup;
static FILE *PWM_run;
static FILE *PWM_duty;

void pwm_init(void)
{
	//load cape driver
	PWMsetup = fopen(SLOT_DIR, "w");
	fprintf(PWMsetup,"%s", CAPE_UNN);
	fclose(PWMsetup);

	system(PIN_CONFIG_DIR " " PIN_USE_X " pwm");

	//set pwm channel and pin to open
	PWMsetup = fopen(EXPORT_DIR, "w");
	fprintf(PWMsetup,"%s", PWM_CHANNEL_X);
	fclose(PWMsetup);

	//set default arbitrary period cycle
	PWMsetup = fopen(PWM_DIR PWM_CHANNEL_X PERIOD_CYCLE_DIR, "w");
	fprintf(PWMsetup,"%d", PERIOD_CYCLE_NS);
	fclose(PWMsetup);

	system(PIN_CONFIG_DIR " " PIN_USE_Y " pwm");
	//set pwm channel and pin to open
	PWMsetup = fopen(EXPORT_DIR, "w");
	fprintf(PWMsetup,"%s", PWM_CHANNEL_Y);
	fclose(PWMsetup);

	//set default arbitrary period cycle
	PWMsetup = fopen(PWM_DIR PWM_CHANNEL_Y PERIOD_CYCLE_DIR, "w");
	fprintf(PWMsetup,"%d", PERIOD_CYCLE_NS);
	fclose(PWMsetup);


}
void pwm_change_deg(int deg, char axis) {
	if(deg > MAX_DEG) {
		deg = MAX_DEG;
	} else if(deg < MIN_DEG) {
		deg = MIN_DEG;
	}
	fprintf(stderr, "%d\n", (deg * DEG_NS) + DUTY_MIN_NS);
	pwm_change_ns((deg * DEG_NS) + DUTY_MIN_NS, axis);
}
void pwm_change_ns(int ns, char axis) {
	if(ns > DUTY_MAX_NS) {
		ns = DUTY_MAX_NS;
	 } else if(ns < DUTY_MIN_NS) {
	 	ns = DUTY_MIN_NS;
	 }
	if(axis == 'X') {
		PWM_duty = fopen(PWM_DIR PWM_CHANNEL_X DUTY_CYCLE_DIR, "w");		
	} else if(axis == 'Y') {
		PWM_duty = fopen(PWM_DIR PWM_CHANNEL_Y DUTY_CYCLE_DIR, "w");	
	}

	fprintf(PWM_duty,"%d", ns);
	fclose(PWM_duty);	
}
int pwm_set_run(int status, char axis) 
{
	if(status == 1 || status == 0) {

		if(axis == 'X') {
			PWM_run = fopen(PWM_DIR PWM_CHANNEL_X RUN_DIR, "w");	
		} else if(axis == 'Y') {
			PWM_run = fopen(PWM_DIR PWM_CHANNEL_Y RUN_DIR, "w");	
		}		
		fprintf(PWM_run,"%d", status);
		fclose(PWM_run);
		return status;			
	}
	return -1;
}
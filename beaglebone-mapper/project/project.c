#include "pwm_servo.h"
#include "ad_reader.h"
#include "routine.h"
#include "UDPListener.h"

#include "lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <unistd.h>

#include <sys/wait.h>
#include <time.h>
#include <sys/time.h>

#include <math.h>

#define MAX_NS = 2500000
#define MIN_NS = 470000
/*
*	Code starts and intializes the whole program, it is the point of entry for the program
*/
struct timespec readDelay = {1,0};

int main(void) {
	//init the pwm
	pwm_init();
	//init the sensor
	AD_init();
	routine_init();
	UDP_ListenerLaunchThread();
	// lets it run in a loop while other thread does the actual runtime executions
	while(1) {
		nanosleep(&readDelay, (struct timespec *) NULL);
	}
	UDP_close();
	return 0;
}
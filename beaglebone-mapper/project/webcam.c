#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/time.h>
/*
 MODULE FOR CAPTURING PHOTOS
 IMAGEMAGICK AND FSWEBCAM REQUIRED ON BBB, PLEASE INSTALL BEFORE RUNNING



*/

#define WEBCAM_OUT_DIR "/root/"
#define WEBCAM_RES "-r 1280x720 "
#define WEBCAM_CMD "fswebcam --png 3 --no-banner " 

#define STITCH_CMD "convert "
#define STITCH_ARG " +append "

static char command[150];

FILE *fd;
//image size
size_t size;
unsigned char *buffer;
//Modules requires fswebcam to be installed

int webcam_snap(char* filename) {
	strcpy(command, WEBCAM_CMD WEBCAM_RES);
	strcat(command, filename);
	return system(command);
}

int webcam_stitch(char* inputs, char* output) {
	strcpy(command, STITCH_CMD);
	strcat(command, inputs);
	strcat(command, STITCH_ARG);
	strcat(command, output);
	return system(command);
}

void webcam_send_image(void) {
	system("cp scene.png /root/server/3dpublic");
}

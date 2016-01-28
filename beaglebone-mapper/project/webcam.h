//webcam.h
#ifndef _WEBCAM_H_
#define _WEBCAM_H_
int webcam_snap(char* filename);
int webcam_stitch(char* inputs, char* output);
void webcam_send_image(void);
#endif

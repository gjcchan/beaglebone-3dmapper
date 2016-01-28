// pwm_servo.h
#ifndef _PWM_LISTENER_H_
#define _PWM_LISTENER_H_
// Init AC
void pwm_init(void);
//
void pwm_change_deg(int deg, char axis);
// 
void pwm_change_ns(int ns, char axis);
// Start polling thread
int pwm_set_run(int status, char axis);
#endif


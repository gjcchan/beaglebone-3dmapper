// routine.h
#ifndef _ROUTINE_H_
#define _ROUTINE_H_
// Init Routine objects
void routine_init(void);
//
void routine_change_deg(int deg);
// 
void routine_run(void);
//
void routine_halt(void);
//
char* routine_return(void);

void routine_print(void);
char* routine_json(void);
double* routine_getDimensionValues(void); 
double routine_getTemperatureValue(void);
#endif


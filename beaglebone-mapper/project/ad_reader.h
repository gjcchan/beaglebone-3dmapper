//ad_reader.h used to read temperature and IR dietance
#ifndef _AD_H_
#define _AD_H_
//configures and setup the initial condition for reading the sensor
void AD_init(void);
//read from distance sensor channel is between 0-6, errors would return -1
int AD_read(int channel);

int AD_getVoltage0Reading();
double AD_getVoltage(int reading);
double AD_getTemperature(double voltage);
#endif
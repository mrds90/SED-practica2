/** \file io.h
 *  Mar 2022
 *  Maestría en SIstemas Embebidos
 */

#ifndef IO_H_
#define IO_H_

/* Prototipos */
void IO_gpioInit();
int IO_setLed(int);
void IO_toggleLed(void);

void IO_adcInit();
uint16_t IO_readAdc();
uint32_t IO_voltAdc();

void IO_monitorStem(int);
void IO_monitorPause(const char *);


void IO_pwmInit(void);
void IO_pwmSet(float);

#endif /* IO_H_ */

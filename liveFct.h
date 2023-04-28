#ifndef LIVEFCT_H
#define	LIVEFCT_H

#include <xc.h>
#include "carInterface.h"

void rtManageMotor(uint8_t brake, uint8_t accel);
void rtManageWheel();
void rtOdometer();
void rtClock();
static bool stop  = false;
static bool start  = false;
static bool tick = false;

#endif	/* LIVEFCT_H */


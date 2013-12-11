
#ifndef _PWM_H_
#define _PWM_H_

#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>

#define RANGE_DEFAULT 100

class PWM {
	public:
		PWM();
		int initialize ( int pin, int initialValue, int pwmRange );
		int writePulse ( int pin, int nomValue ) ;
		int range, value;
};

#endif

#include "PWM.h"

PWM::PWM() {
	range = RANGE_DEFAULT;
}

int PWM::initialize( int pin, int initialValue, int pwmRange ) {
	range = pwmRange;
	int n = softPwmCreate ( pin, initialValue, range) ;
	if( n < 0 ) {
		printf( "Attention: PWM can not initialize! \n" );
	}
	else {
		printf( "PWM initialize successfully!\n" );
	}
}

int PWM::writePulse( int pin, int nomValue ) {
	value = nomValue/100 + 10;
	softPwmWrite (pin, value) ;
	return 0;
}
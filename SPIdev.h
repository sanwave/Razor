
#ifndef _SPIDEV_H_
#define _SPIDEV_H_

#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>
#include <errno.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include "Type.h"

class SPIdev {
	public:
		SPIdev();
		static int setup (int channel, int speed) ;
		static int dataRW (int channel, unsigned char *data, int len) ;		
		
};

#endif
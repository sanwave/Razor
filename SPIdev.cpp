
#include "SPIdev.h"

SPIdev::SPIdev() {
}

int SPIdev::setup(int channel, int speed) {
	return  wiringPiSPISetup (channel, speed) ;
}

int SPIdev::dataRW(int channel, unsigned char *data, int len) {
	return wiringPiSPIDataRW (channel, data, len) ;
}
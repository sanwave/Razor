
#include "MS561101BA.h"
#define CONVERSION_TIME 10000l // conversion time in microseconds


MS561101BA::MS561101BA() {
	devAddr = MS561101BA_DEFAULT_ADDRESS;
}

MS561101BA::MS561101BA(uint8_t addr) {
	devAddr = addr;
}

int MS561101BA::initialize() { 
	printf ("\n*****MS5611 Driver*****\n");
	msAddr =  I2Cdev::setup (devAddr);
	if (msAddr < 0){
		printf ("Attention: Unable to initialise MS561101BA.\n");
		return 1;
	}
	reset(); // reset the device to populate its internal PROM registers
	delay(1000); // some safety time
	readPROM(); // reads the PROM into object variables for later use
	printf ("MS561101BA Initialize Successfully!\n");
	return 0;
}

float MS561101BA::getPressure(uint8_t OSR) {
	// see datasheet page 7 for formulas
  
	uint32_t rawPress = rawPressure(OSR);
	if(rawPress == 0) {
		return 0;
	}

	int32_t dT = getDeltaTemp(OSR);
	if(dT == 0) {
		return 0.0;
	}
  
	int64_t off  = ((uint32_t)_C[1] <<16) + (((int64_t)dT * _C[3]) >> 7);
	int64_t sens = ((uint32_t)_C[0] <<15) + (((int64_t)dT * _C[2]) >> 8);
	return ((( (rawPress * sens ) >> 21) - off) >> 15) / 100.0;
}

float MS561101BA::getTemperature(uint8_t OSR) {
	// see datasheet page 7 for formulas
	int64_t dT = getDeltaTemp(OSR);

	if(dT != 0) {
		return (2000 + ((dT * _C[5]) >> 23)) / 100.0;
	}
	else {
		return 0.0;
	}
}

int32_t MS561101BA::getDeltaTemp(uint8_t OSR) {
	uint32_t rawTemp = rawTemperature(OSR);
	if(rawTemp != 0) {
		return (int32_t)(rawTemp - ((uint32_t)_C[4] << 8));
	}
	else {
		return 0;
	}
}

//TODO: avoid duplicated code between rawPressure and rawTemperature methods
//TODO: possible race condition between readings.. serious headache doing this.. help appreciated!

uint32_t MS561101BA::rawPressure(uint8_t OSR) {
/*
	startConversion(MS561101BA_D2 + OSR);
	delay(300);
	pressCache = getConversion(MS561101BA_D1 + OSR);
	return pressCache;
/**/
	unsigned long now = micros();
	if(lastPresConv != 0 && (now - lastPresConv) >= CONVERSION_TIME) {
		lastPresConv = 0;
		pressCache = getConversion(MS561101BA_D1 + OSR);
	}
	else {
		if(lastPresConv == 0 && lastTempConv == 0) {
			startConversion(MS561101BA_D1 + OSR);
			lastPresConv = now;
		}		
	}
	return pressCache;
}

uint32_t MS561101BA::rawTemperature(uint8_t OSR) {
/*
	startConversion(0x58);
	delay(110);
	tempCache = getConversion(MS561101BA_D2 + OSR);

*/
	unsigned long now = micros();
	if(lastTempConv != 0 && (now - lastTempConv) >= CONVERSION_TIME) {
		lastTempConv = 0;
		tempCache = getConversion(MS561101BA_D2 + OSR);
	}
	else {
		if(lastTempConv == 0 && lastPresConv == 0) { // no conversions in progress
			startConversion(MS561101BA_D2 + OSR);
			lastTempConv = now;
		}
	}
	
	return tempCache;
}


// see page 11 of the datasheet
void MS561101BA::startConversion(uint8_t command) {
	I2Cdev::writeByte(msAddr, command, 0x00);  
}

uint32_t MS561101BA::getConversion(uint8_t command) {
	uint32_t raw[4];
	I2Cdev::writeByte(msAddr, 0x00);  
	raw[2] = I2Cdev::readByte(msAddr);
	raw[1] = I2Cdev::readByte(msAddr);
	raw[0] = I2Cdev::readByte(msAddr); 
	uint32_t val = raw[2]<<16 | raw[1]<<8 | raw[0];
	printf("#val is : 0x%x\n", val);
	return val;
}

int MS561101BA::readPROM() {
	uint8_t buffer[14];
	I2Cdev::readBytes(msAddr, (uint8_t)MS561101BA_PROM_BASE_ADDR, (uint8_t)14, buffer);
	for (int i=0;i<=5;++i) {		
		_C[i] = (buffer[2*i] << 8) | buffer[2*i+1];		
	}
	printf("C0=0x%x, C1=0x%x, C2=0x%x, C3=0x%x, C4=0x%x, C5=0x%x\n", _C[0], _C[1], _C[2], _C[3], _C[4], _C[5]);
	return 0;
}

void MS561101BA::reset() {
	I2Cdev::writeByte(msAddr, MS561101BA_RESET);
}
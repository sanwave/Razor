
#include "I2Cdev.h"

I2Cdev::I2Cdev() {
}

int I2Cdev::setup(int devId) {
	return wiringPiI2CSetup (devId) ;
}

uint8_t I2Cdev::readByte(int devAddr) {
    return (uint8_t)wiringPiI2CRead(devAddr);	
}

uint8_t I2Cdev::readByte(int devAddr, uint8_t regAddr) {
    return (uint8_t)wiringPiI2CReadReg8(devAddr, regAddr);	
}

void I2Cdev::readByte(int devAddr, uint8_t regAddr, uint8_t *data) {
    *data= (uint8_t)wiringPiI2CReadReg8(devAddr, regAddr);
}

void I2Cdev::readBytes(int devAddr, uint8_t regAddr, uint8_t length, uint8_t *data) {
	for (uint8_t i = 0; i < length; ++i) {
		data[i]=(uint8_t)wiringPiI2CReadReg8(devAddr, regAddr+i);
	}
}

//未完
void I2Cdev::readBytes(int devAddr, uint8_t length, uint8_t *data) {
	char flag=0x40;
	pinMode(PINSDA0, OUTPUT);
	for(uint8_t i=0; i<7; ++i) {
		if(devAddr & flag) {
			digitalWrite (PINSDA0, HIGH);
		}
		else {
			digitalWrite (PINSDA0, LOW);
		}
		flag >>= 1;
	}
	digitalWrite (PINSDA0, HIGH);	//Read
	delayMicroseconds(4);
	
}

void I2Cdev::writeByte(int devAddr, uint8_t data){
	wiringPiI2CWrite(devAddr, data);
}

void I2Cdev::writeByte(int devAddr, uint8_t regAddr, uint8_t data) {
    wiringPiI2CWriteReg8(devAddr,regAddr,  data);
}

void I2Cdev::writeBytes(int devAddr, uint8_t regAddr, uint8_t length, uint8_t* data) {
    for (uint8_t i = 0; i < length; ++i) {
		wiringPiI2CWriteReg8(devAddr, regAddr+i, data[i]);
	}
}
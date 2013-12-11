
#ifndef _I2CDEV_H_
#define _I2CDEV_H_

#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>
#include <errno.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include "Type.h"

#define PINSDA0 8
#define PINSCL0 9

class I2Cdev {
    public:
        I2Cdev(); 
		static int setup(int devId);
		static uint8_t readByte(int devAddr);
		static uint8_t readByte(int devAddr, uint8_t regAddr);
        static void readByte(int devAddr, uint8_t regAddr, uint8_t *data);
        static void readBytes(int devAddr, uint8_t regAddr, uint8_t length, uint8_t *data);
		static void readBytes(int devAddr, uint8_t length, uint8_t *data);
        
		static void writeByte(int devAddr, uint8_t data);
        static void writeByte(int devAddr, uint8_t regAddr, uint8_t data);
        static void writeBytes(int devAddr, uint8_t regAddr, uint8_t length, uint8_t *data);
};

#endif

#ifndef _UARTDEV_H_
#define _UARTDEV_H_

#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>
#include <errno.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include "Type.h"

class Uartdev {
	public:
		Uartdev();
		static int setup(char *device, int baud);
		static void write (int fd, char* s);
		static int getChar (int fd);
		static int read (int fd, char *s);
		//TxBuf[BUF_SIZE];
		//RxBuf[BUF_SIZE];
};

#endif
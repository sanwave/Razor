
#include "Uartdev.h"

Uartdev::Uartdev() {
}

int Uartdev::setup (char *device, int baud) {
	return serialOpen (device, baud) ;
}

void Uartdev::write (int fd, char* s) {
	serialPuts(fd, s);
}

int Uartdev::getChar (int fd) {
	return serialGetchar(fd);
}

int Uartdev::read (int fd, char *s) {
	int i;
	int n = serialDataAvail(fd);	
	if( n <= 0 )
		return n-1;
	for(i=0; i<n ; ++i) {
		if(i == BUF_SIZE-1) {
			s[i] = '\0';
			return 1;
		}
		s[i] = serialGetchar (fd);
	}
	s[i] = '\0';
	return 0;
}
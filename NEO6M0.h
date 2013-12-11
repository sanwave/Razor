
#ifndef _NEO6M0_H_
#define _NEO6M0_H_

#include <string.h>
#include "Uartdev.h"
#include "Stringdev.h"

class NEO6M0 {
	public:
		NEO6M0();
		int initialize();
		int updateInfos (char *readBuf);
		int printInfos();
		int close();
		int decode(char *statement);
		int decodeGGA (char *statement) ;
		int decodeGLL (char *statement) ;
		int decodeGSA (char *statement) ;
		int decodeGSV (char *statement) ;
		int decodeRMC (char *statement) ;
		int decodeTXT (char *statement) ;
		int decodeVTG (char *statement) ;
		double getDouble(char *str);
				
		int fixStatus;							//1定位 or 0未定位
		double latitude,longitude,altitude;		//
		double speed, direction;				//
		int year,month,day,hour,minute;			//
		float second;							//
		int satellitesCount;					//
		float pdop, hdop, vdop;					//
		char checksum[2];						//
		int neoAddr;
		
	private:		
		char buffer[BUF_SIZE];
};

#endif
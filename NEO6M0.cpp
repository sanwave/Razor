
#include "NEO6M0.h"

NEO6M0::NEO6M0() {
	fixStatus = 0;
	latitude = 0.001;
	longitude = 0.0;
	altitude = 0.0;
	speed = 0.0;
	direction = 0.0;
	year = month = day = 0;
	hour = minute = 0;
	second = 0.0;
	satellitesCount = 0;
	pdop = hdop = vdop = 0.0;	//0.5~99.9	
}

int NEO6M0::initialize() {
	char *device = (char *)"/dev/ttyAMA0";
	neoAddr = Uartdev::setup(device, 9600);
	if(neoAddr == -1) {
		return -1;
	}
	else {
		return 1;
	}
}

int NEO6M0::updateInfos(char *readBuf) {		
	char *statement, *block;
	while (Uartdev::read( neoAddr, readBuf ) >= 0) {
		if (*readBuf == 0x00) {
			return 0;
		}
		block = readBuf;
		while (1) {
			statement = Stringdev::spiltHalf (&block, '\n');
			if( *buffer != 0x00 && *block != 0x00) {
				strcat ( buffer, statement );
				decode ( buffer );
				*buffer = 0x00;
			}
			else if ( *buffer != 0x00 && *block == 0x00) {
				strcat ( buffer, statement );
				decode ( buffer );
				*buffer = 0x00;
				break;
			}
			else if (*buffer == 0x00 && *block != 0x00) {
				decode ( statement );
			}
			else if(*buffer == 0x00 && *block == 0x00) {
				strcpy ( buffer, statement );
				break;
			}
		}
	}	
	return 1;
}

int NEO6M0::printInfos() {
	printf("******GPS Driber Info******\n");
	printf("\tGPS Working Status:%d (1=>working 0=>not work)\n", fixStatus);
	printf("\tTime: %2d:%2d:%5.2f\n", hour, minute, second);
	printf("\tDate: %4d/%2d/%2d\n", year, month, day);
	printf("\tLatitude: %-16.12f\tLongitude:%-16.12f\tAltitude:%-7.2f\n", latitude, longitude, altitude);
	printf("\tThe speed is: %d\n", speed);
	printf("\tSetellite Numbers: %d\n", satellitesCount);
	printf("\tHDOP is: %f\n", hdop);
}

int NEO6M0::decode( char *statement ) {
	char head[7];
	strncpy(head, statement, 6);
	if(strcmp(head, "$GPGGA") == 0) {
		decodeGGA(statement);
	}
	else if(strcmp(head, "$GPGLL") == 0) {
		decodeGLL(statement);
	}
	else if(strcmp(head, "$GPGSA") == 0) {
		decodeGSA(statement);
	}
	else if(strcmp(head, "$GPGSV") == 0) {
		decodeGSV(statement);
	}
	else if(strcmp(head, "$GPRMC") == 0) {
		decodeRMC(statement);
	}
	else if(strcmp(head, "$GPTXT") == 0) {
		decodeTXT(statement);
	}
	else if(strcmp(head, "$GPVTG") == 0) {
		decodeVTG(statement);
	}
	return 0;
}

int NEO6M0::decodeGGA (char *statement) {	
	//printf("\tdecodeGGA: %s\n", statement);
	char *field, *rest = statement;
	field = Stringdev::spiltHalf (&rest, ',');
	char *flag = (char *)"$GPGGA";
	if( strcmp(field,flag) != 0 ) {
		printf("Error: decodeGGA: strcmp(field,flag) != 0\n");
		return -1;
	}
	
	//UTC time
	field = Stringdev::spiltHalf (&rest, ',');
	if( strlen(field) != 0 ) {
		hour = (field[0]-0x30) * 10 + (field[1]-0x30);
		minute = (field[2]-0x30) * 10 + (field[3]-0x30);
		second = (field[4]-0x30) * 10 + (field[5]-0x30) + (field[7]-0x30) / 10 + (field[8]-0x30) / 100 ;
	}
	
	//latitude
	field = Stringdev::spiltHalf (&rest, ',');
	if( strlen(field) != 0 ) {
		latitude = (field[2]-0x30) * 10 + (field[3]-0x30) + (field[5]-0x30) / 10 + (field [6]-0x30) / 100;
		latitude = (field[0]-0x30) * 10 + (field[1]-0x30) + latitude / 60;
	}		
	field = Stringdev::spiltHalf (&rest, ',');
	if(*field == 'S') {
		latitude *= -1;
	}
	
	//longitude
	field = Stringdev::spiltHalf (&rest, ',');
	if( strlen(field) != 0 ) {
		longitude = (field[3]-0x30) * 10 + (field[4]-0x30) + (field[6]-0x30) / 10 + (field [7]-0x30) / 100;
		longitude = (field[0]-0x30) * 100 + (field[1]-0x30) * 10 + (field[2]-0x30) + longitude / 60;
	}
	field = Stringdev::spiltHalf (&rest, ',');
	if(*field == 'W') {
		longitude *= -1;
	}
	
	//FS 0为无效，1为非差分定位，2为查分定位，3为无效PPS，6为正在定位
	field = Stringdev::spiltHalf (&rest, ',');	
	if( strlen(field) != 0) {
		if( *field == '1' || *field == '2' || *field == '3' ) {
			fixStatus = 1;
		}
		else if(*field == '0') {
			fixStatus = 0;
		}
	}
	
	//NoSv
	field = Stringdev::spiltHalf (&rest, ',');
	if( strlen(field) != 0 ) {
		satellitesCount = (field[0]-0x30) * 10 + (field[1]-0x30);
	}
	
	//HDOP
	field = Stringdev::spiltHalf (&rest, ',');
	if( strlen(field) != 0 ) {
		hdop = (float)getDouble(field);
	}
	
	//msl
	field = Stringdev::spiltHalf (&rest, ',');
	if( strlen(field) != 0 ) {
		altitude = getDouble(field);
	}
	
	//uMsl
	field = Stringdev::spiltHalf (&rest, ',');
	if( strlen(field) != 0 ) {
		if( *field != 'M' ) {
			altitude *= -1.0;
		}		
	}
	
	//Altret
	field = Stringdev::spiltHalf (&rest, ',');
	if( strlen(field) != 0 ) {
		
	}
	
	//uSep
	field = Stringdev::spiltHalf (&rest, ',');
	if( strlen(field) != 0 ) {
		
	}
	
	//uDiffAge
	field = Stringdev::spiltHalf (&rest, ',');
	if( strlen(field) != 0 ) {
		
	}
	
	//DiffStation
	field = Stringdev::spiltHalf (&rest, '*');
	if( strlen(field) != 0 ) {
		
	}
	
	//Checksum
	field = rest;
	if( strlen(field) != 0 ) {
		
	}
	return 1;
}

int NEO6M0::decodeGLL (char *statement) {
	//printf("\tdecodeGLL: %s\n", statement);
	char *field, *rest = statement;
	field = Stringdev::spiltHalf (&rest, ',');
	char *flag = (char *)"$GPGLL";
	if( strcmp(field,flag) != 0 ) {
		printf("Error: decodeGLL: strcmp(field,flag) != 0\n");
		return -1;
	}
	
	//latitude
	field = Stringdev::spiltHalf (&rest, ',');
	if( strlen(field) != 0 ) {
		latitude = (field[2]-0x30) * 10 + (field[3]-0x30) + (field[5]-0x30) / 10 + (field [6]-0x30) / 100;
		latitude = (field[0]-0x30) * 10 + (field[1]-0x30) + latitude / 60;
	}		
	field = Stringdev::spiltHalf (&rest, ',');
	if(*field == 'S') {
		latitude *= -1;
	}
	
	//longitude
	field = Stringdev::spiltHalf (&rest, ',');
	if( strlen(field) != 0 ) {
		longitude = (field[3]-0x30) * 10 + (field[4]-0x30) + (field[6]-0x30) / 10 + (field [7]-0x30) / 100;
		longitude = (field[0]-0x30) * 100 + (field[1]-0x30) * 10 + (field[2]-0x30) + longitude / 60;
	}
	field = Stringdev::spiltHalf (&rest, ',');
	if(*field == 'W') {
		longitude *= -1;
	}
	
	//UTC time
	field = Stringdev::spiltHalf (&rest, ',');
	if( strlen(field) != 0 ) {
		hour = (field[0]-0x30) * 10 + (field[1]-0x30);
		minute = (field[2]-0x30) * 10 + (field[3]-0x30);
		second = (field[4]-0x30) * 10 + (field[5]-0x30) + (field[7]-0x30) / 10 + (field[8]-0x30) / 100 ;
	}
	
	//Valid 0为无效，1为有效
	field = Stringdev::spiltHalf (&rest, ',');	
	if( strlen(field) == 0  || *field == 'V') {
		fixStatus = 0;
	}
	else
		fixStatus = 1;
	
	//Mode
	field = Stringdev::spiltHalf (&rest, '*');
	if( strlen(field) != 0 ) {
		
	}
	
	//Checksum
	field = rest;
	if( strlen(field) != 0 ) {
		
	}
	return 1;
}

int NEO6M0::decodeGSA (char *statement) {
	//printf("\tdecodeGSA: %s\n", statement);
	char *field, *rest = statement;
	field = Stringdev::spiltHalf (&rest, ',');
	char *flag = (char *)"$GPGSA";
	if( strcmp(field,flag) != 0 ) {
		printf("Error: decodeGSA: strcmp(field,flag) != 0");
		return -1;
	}
	
	//Smode
	field = Stringdev::spiltHalf (&rest, ',');
	if( strlen(field) != 0 ) {
		
	}
	
	//FS 1为未定位，2为2D定位，3为3D定位
	field = Stringdev::spiltHalf (&rest, ',');	
	if( strlen(field) != 0) {
		if( *field == '2' || *field == '3' ) {
			fixStatus = 1;
		}
		else if(*field == '1') {
			fixStatus = 0;
		}
	}
	
	//sv
	for(int i=0; i<12; ++i) {
		field = Stringdev::spiltHalf (&rest, ',');
	}
	
	//PDOP
	field = Stringdev::spiltHalf (&rest, ',');
	if( strlen(field) != 0 ) {
		pdop = (float)getDouble(field);
	}
	
	//HDOP
	field = Stringdev::spiltHalf (&rest, ',');
	if( strlen(field) != 0 ) {
		hdop = (float)getDouble(field);
	}
	
	//VDOP
	field = Stringdev::spiltHalf (&rest, '*');
	if( strlen(field) != 0 ) {
		vdop = (float)getDouble(field);
	}
	
	//Checksum
	field = rest;
	if( strlen(field) != 0 ) {
		strncpy(checksum, field, 2);
	}
	
	return 1;
}

int NEO6M0::decodeGSV (char *statement) {
	//printf("\tdecodeGSV: %s\n", statement);
	char *field, *rest = statement;
	field = Stringdev::spiltHalf (&rest, ',');
	char *flag = (char *)"$GPGSV";
	if( strcmp(field,flag) != 0 ) {
		printf("Error: decodeGSV: strcmp(field,flag) != 0");
		return -1;
	}
	
	//NoMsg
	field = Stringdev::spiltHalf (&rest, ',');
	if( strlen(field) != 0 ) {
		
	}
	
	//MsgNo
	field = Stringdev::spiltHalf (&rest, ',');
	if( strlen(field) != 0 ) {
		
	}
	
	//NoSv
	field = Stringdev::spiltHalf (&rest, ',');
	if( strlen(field) != 0 ) {
		//satellitesCount = (field[0]-0x30) * 10 + (field[1]-0x30);
	}
	
	return 1;
}

int NEO6M0::decodeRMC (char *statement) {
	//printf("\tdecodeRMC: %s\n", statement);
	char *field, *rest = statement;
	field = Stringdev::spiltHalf (&rest, ',');
	char *flag = (char *)"$GPRMC";
	if( strcmp(field,flag) != 0 ) {
		printf("Error: decodeRMC: strcmp(field,flag) != 0");
		return -1;
	}
	//UTC time
	field = Stringdev::spiltHalf (&rest, ',');
	if( strlen(field) != 0 ) {
		hour = (field[0]-0x30) * 10 + (field[1]-0x30);
		minute = (field[2]-0x30) * 10 + (field[3]-0x30);
		second = (field[4]-0x30) * 10 + (field[5]-0x30) + (field[7]-0x30) / 10 + (field[8]-0x30) / 100 ;
	}
	
	//Status 0为未定位，1为有效
	field = Stringdev::spiltHalf (&rest, ',');	
	if( strlen(field) == 0  || *field == 'V') {
		fixStatus = 0;
	}
	else
		fixStatus = 1;
	
	//latitude
	field = Stringdev::spiltHalf (&rest, ',');
	if(*field != 0x00) {
		latitude = (field[2]-0x30) * 10 + (field[3]-0x30) + (field[5]-0x30) / 10 + (field [6]-0x30) / 100;
		latitude = (field[0]-0x30) * 10 + (field[1]-0x30) + latitude / 60;
	}		
	field = Stringdev::spiltHalf (&rest, ',');
	if(*field == 'S') {
		latitude *= -1;
	}
	
	//longitude
	field = Stringdev::spiltHalf (&rest, ',');
	if( strlen(field) != 0 ) {
		longitude = (field[3]-0x30) * 10 + (field[4]-0x30) + (field[6]-0x30) / 10 + (field [7]-0x30) / 100;
		longitude = (field[0]-0x30) * 100 + (field[1]-0x30) * 10 + (field[2]-0x30) + longitude / 60;
	}
	field = Stringdev::spiltHalf (&rest, ',');
	if(*field == 'W') {
		longitude *= -1;
	}
	
	//Spd
	field = Stringdev::spiltHalf (&rest, ',');
	if( strlen(field) != 0 ) {
		
	}
		
	//Cog
	field = Stringdev::spiltHalf (&rest, ',');
	if( strlen(field) != 0 ) {
		
	}
	
	//Date ddmmyy
	field = Stringdev::spiltHalf (&rest, ',');
	if( strlen(field) != 0 ) {
		year = (field [4]-0x30) * 10 + (field[5]-0x30) + 2000;
		month = (field [2]-0x30) * 10 + (field[3]-0x30);
		day = (field [0]-0x30) * 10 + (field[1]-0x30);
	}
	
	//mv
	field = Stringdev::spiltHalf (&rest, ',');
	if( strlen(field) != 0 ) {
		
	}
	
	//mvE
	field = Stringdev::spiltHalf (&rest, ',');
	if( strlen(field) != 0 ) {
		
	}
	
	//mode
	field = Stringdev::spiltHalf (&rest, '*');
	if( strlen(field) != 0 ) {
		if(*field == 'A' || *field == 'D' ||*field == 'E') {
			fixStatus = 1;
		}
		else {
			fixStatus = 0;
		}
	}
	
	//Checksum
	field = rest;
	if( strlen(field) != 0 ) {
		
	}
	
	return 1;
}

int NEO6M0::decodeTXT (char *statement) {
	//printf("\tdecodeTXT: %s\n", statement);
	char *field, *rest = statement;
	field = Stringdev::spiltHalf (&rest, ',');
	char *flag = (char *)"$GPTXT";
	if( strcmp(field,flag) != 0 ) {
		printf("Error: decodeTXT: strcmp(field,flag) != 0");
		return -1;
	}
	return 1;
}

int NEO6M0::decodeVTG (char *statement) {
	//printf("\tdecodeVTG: %s\n", statement);
	char *field, *rest = statement;
	field = Stringdev::spiltHalf (&rest, ',');
	char *flag = (char *)"$GPVTG";
	if( strcmp(field,flag) != 0 ) {
		printf("Error: decodeVTG: strcmp(field,flag) != 0");
		return -1;
	}
	
	//Cogt
	field = Stringdev::spiltHalf (&rest, ',');
	if( strlen(field) != 0 ) {
		direction = getDouble ( field );
	}
	
	//T
	field = Stringdev::spiltHalf (&rest, ',');
	if( strlen(field) != 0 && strlen(field) != 'T') {
		direction = getDouble ( field );
	}
	
	//Cogm
	field = Stringdev::spiltHalf (&rest, ',');
	if( strlen(field) != 0 ) {
		
	}
	
	//M
	field = Stringdev::spiltHalf (&rest, ',');
	if( strlen(field) != 0 ) {
		
	}
	
	//Sog
	field = Stringdev::spiltHalf (&rest, ',');
	if( strlen(field) != 0 ) {
		
	}
	
	//N
	field = Stringdev::spiltHalf (&rest, ',');
	if( strlen(field) != 0 ) {
		
	}
	
	//Kph
	field = Stringdev::spiltHalf (&rest, ',');
	if( strlen(field) != 0 ) {
		speed = getDouble ( field );
	}
	
	//K
	field = Stringdev::spiltHalf (&rest, ',');
	if( strlen(field) != 0 && strlen(field) != 'K') {
		speed = -1.0;
	}
	
	//mode
	field = Stringdev::spiltHalf (&rest, '*');
	if( strlen(field) != 0 ) {
		if(*field == 'A' || *field == 'D' ||*field == 'E') {
			fixStatus = 1;
		}
		else {
			fixStatus = 0;
		}
	}
	
	//Checksum
	field = Stringdev::spiltHalf (&rest, ',');
	if( strlen(field) != 0 ) {
		
	}
	
	return 1;
}

double NEO6M0::getDouble(char *str) {
	double data = 0.0;
	int i,j,integer = 0, decimal = 0;
	for ( i=0; str[i]!=0x00 && str[i]!='.'; ++i) {
		integer = integer * 10 + str[i] - 0x30;
	}
	if(str[i++] == '.') {
		for ( j=0; str[i]!=0x00; ++i, ++j) {
			decimal = decimal * 10 + str[i] - 0x30;
		}
		data = decimal;
		while (j--) {
			data /= 10;
		}		
	}
	data += integer;
	return data;
}

int NEO6M0::close() {
	serialClose (neoAddr);
	return 1;
}
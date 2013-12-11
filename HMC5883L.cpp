
#include "HMC5883L.h"

HMC5883L::HMC5883L() {
    devAddr = HMC5883L_DEFAULT_ADDRESS;
}

HMC5883L::HMC5883L(uint8_t address) {
    devAddr = address;
}

int HMC5883L::initialize() {	
	printf ("\n*****RasPi HMC5883L Driver*****\n");	
	hmcAddr = I2Cdev::setup (devAddr);
	if (hmcAddr < 0){
		printf ("Attention: Unable to initialise HMC5883L.\n");
		return 1;
	}
	if(!testConnection()) {
		printf ("Attention: Unable to connect HMC5883L.\n");
		return 1;
	}
	I2Cdev::writeByte(hmcAddr,HMC5883L_RA_CONFIG_A,  0x70);			//采样速率、模式及求平均次数
	I2Cdev::writeByte(hmcAddr,HMC5883L_RA_CONFIG_B,  0x20);			//采样精度和范围
	I2Cdev::writeByte(hmcAddr,HMC5883L_RA_MODE,  0x00);				//连续采样模式
	printf ("HMC5883L Initialize Successfully!\n");
	//1. 0x70 => 0x00 Config A
	//2. 0x20 => 0x01 Config B
	//3. 0x00/0x01 => 0x02 Mode Single  每次获取数据前都要执行
			
	return 0;
}

bool HMC5883L::testConnection() {
    I2Cdev::readBytes(hmcAddr, HMC5883L_RA_ID_A, 3, buffer);
	//printf("HMC devid is : %c%c%c",buffer[0],buffer[1],buffer[2]);
    if (buffer[0] == 'H' && buffer[1] == '4' && buffer[2] == '3') {
		return true;
    }
    return false;
}

void HMC5883L::getHeading(int16_t *x, int16_t *y, int16_t *z) {
    I2Cdev::readBytes(hmcAddr, HMC5883L_RA_DATAX_H, 6, buffer);
    if (mode == HMC5883L_MODE_SINGLE) 
		I2Cdev::writeByte(hmcAddr, HMC5883L_RA_MODE, HMC5883L_MODE_SINGLE << (HMC5883L_MODEREG_BIT - HMC5883L_MODEREG_LENGTH + 1));
    *x = (((int16_t)buffer[0]) << 8) | buffer[1];
    *y = (((int16_t)buffer[4]) << 8) | buffer[5];
    *z = (((int16_t)buffer[2]) << 8) | buffer[3];
}
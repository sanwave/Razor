
#include "MPU6050.h"

MPU6050::MPU6050() {
    devAddr = MPU6050_DEFAULT_ADDRESS;
}

MPU6050::MPU6050(uint8_t address) {
    devAddr = address;
}

int MPU6050::initialize() {	
	printf ("\n*****RasPi MPU6050 Driver******\n");		
	mpuAddr = I2Cdev::setup (devAddr);	
	if (mpuAddr < 0){
		printf ("Attention: Unable to initialise MPU6050.\n");
		return 1;
	}
    if(!testConnection()) {
		printf ("Attention: Unable to connect MPU6050.\n");
		return 1;
	}
    I2Cdev::writeByte(mpuAddr,MPU6050_RA_PWR_MGMT_1,  0x41);			//����ΪX��ʱ��
    I2Cdev::writeByte(mpuAddr,MPU6050_RA_GYRO_CONFIG, MPU6050_GYRO_FS_250<<4);	//��250��
    I2Cdev::writeByte(mpuAddr,MPU6050_RA_ACCEL_CONFIG, MPU6050_ACCEL_FS_2<<4 );	//��2g
	I2Cdev::writeByte(mpuAddr,MPU6050_RA_PWR_MGMT_1,  0x01);			//���Ѵ�����
	I2Cdev::writeByte(mpuAddr,MPU6050_RA_USER_CTRL,  0x00);			//�ر�I2C Master ģʽ
	I2Cdev::writeByte(mpuAddr,MPU6050_RA_INT_PIN_CFG,  0x02);		//����bypassģʽ
		
	//1. 0x41 => 0x6B �趨ʱ��, X��ʱ��
	//2. 0x00 => 0x1B ���������̣�250��/s
	//3. 0x00 => 0x1C ���ٶȼ����̣�2g
	//4. 0x01 => 0x6B ����
	//5. 0x3B~0x48 �۲�������������У������ݾ�Ϊ2�ֽڳ��� 0x3B ���ٶ�X, 0x3D ���ٶ�Y, 0x3F ���ٶ�Z, 0x41 �¶�, 0x43 ������X, 0x45 ������Y, 0x47 ������Z
	
	//�ڴ�Ҫ�ȳ�ʼ�� HMC5883,����Ҫʹ������ֱ�Ӷ������
	//MPU6050 0x00 => 0x6A �ر�I2C Master ģʽ
	//MPU6050 0x02 => 0x37 ��bypassģʽ
	//HMC5883 0x70 => 0x00 Config A
	//HMC5883 0x20 => 0x01 Config B
	//HMC5883 0x00 => 0x02 �������
	
	//����Ϊ������������MPU6050����HMC5883����������Ҫ��MPU6050����һ�����á�����Ϊ��
	//6. 0x20 => 0x6A ��I2C Masterģʽ
	//7. 0x00 => 0x37 �ر�bypassģʽ
	//8. 0x0D => 0x24 ���ò�����������ʱ�Ӽ�����
	//9. 0x9e => 0x25 ���ô������Ĳ���ģʽ�����ַ
	//10. 0x03 => 0x26 ���ö�ȡ����������ʼ�Ĵ�����ַ
	//11. 0x86 => 0x27 ���ö�ȡ�������Ĵ����ĳ��ȼ����ص�
	//���ϲ����MPU6050���ܻ�ȡ����HMC5883��ֵ������һ����I2C SLV0����
	printf ("MPU6050 Initialize Successfully!\n");
	return 0;
}

void MPU6050::EnabledHMC5883L() {
	I2Cdev::writeByte(mpuAddr,MPU6050_RA_USER_CTRL,  0x20);			//����I2C Masterģʽ
	I2Cdev::writeByte(mpuAddr,MPU6050_RA_INT_PIN_CFG,  0x00);		//�ر�bypassģʽ
	I2Cdev::writeByte(mpuAddr,MPU6050_RA_I2C_MST_CTRL,  0x0D);		//���ò�����������ʱ�Ӽ�����
	I2Cdev::writeByte(mpuAddr,MPU6050_RA_I2C_SLV0_ADDR,  0x9e);		//���ô������Ĳ���ģʽ�����ַ
	I2Cdev::writeByte(mpuAddr,MPU6050_RA_I2C_SLV0_REG,  0x03);		//���ö�ȡ����������ʼ�Ĵ�����ַ
	I2Cdev::writeByte(mpuAddr,MPU6050_RA_I2C_SLV0_CTRL,  0x86);		//���ö�ȡ�������Ĵ����ĳ��ȼ����ص�
	printf ("MPU6050 Enabled I2C Masster Mode.\n");
}

bool MPU6050::testConnection() {
	//printf("mpu6050 device id is : %d\n", getDeviceID());
    return getDeviceID() == 0x34 ;
}

uint8_t MPU6050::getDeviceID() {
    return (I2Cdev::readByte(mpuAddr, MPU6050_RA_WHO_AM_I)>>1)&0x3f;
}

uint8_t MPU6050::getFullScaleGyroRange() {
    return I2Cdev::readByte(mpuAddr, MPU6050_RA_GYRO_CONFIG);    
}

void MPU6050::setFullScaleGyroRange(uint8_t range) {
    I2Cdev::writeByte(mpuAddr, MPU6050_RA_GYRO_CONFIG, range<<MPU6050_GCONFIG_FS_SEL_BIT);
}

uint8_t MPU6050::getFullScaleAccelRange() {
    return I2Cdev::readByte(mpuAddr, MPU6050_RA_ACCEL_CONFIG);
}

void MPU6050::setFullScaleAccelRange(uint8_t range) {
    I2Cdev::writeByte(mpuAddr, MPU6050_RA_ACCEL_CONFIG, range<<MPU6050_ACONFIG_AFS_SEL_BIT);
}

void MPU6050::getMotion6(int16_t* ax, int16_t* ay, int16_t* az, int16_t* gx, int16_t* gy, int16_t* gz) {	
	I2Cdev::readBytes(mpuAddr, MPU6050_RA_ACCEL_XOUT_H, 14, buffer);
    *ax = (((int16_t)buffer[0]) << 8) | buffer[1];
    *ay = (((int16_t)buffer[2]) << 8) | buffer[3];
    *az = (((int16_t)buffer[4]) << 8) | buffer[5];
    *gx = (((int16_t)buffer[8]) << 8) | buffer[9];
    *gy = (((int16_t)buffer[10]) << 8) | buffer[11];
    *gz = (((int16_t)buffer[12]) << 8) | buffer[13];
}

void MPU6050::getMotion9(int16_t* ax, int16_t* ay, int16_t* az, int16_t* gx, int16_t* gy, int16_t* gz, int16_t* mx, int16_t* my, int16_t* mz) {
	I2Cdev::readBytes(mpuAddr, MPU6050_RA_ACCEL_XOUT_H, 20, buffer);
    *ax = (((int16_t)buffer[0]) << 8) | buffer[1];
    *ay = (((int16_t)buffer[2]) << 8) | buffer[3];
    *az = (((int16_t)buffer[4]) << 8) | buffer[5];
    *gx = (((int16_t)buffer[8]) << 8) | buffer[9];
    *gy = (((int16_t)buffer[10]) << 8) | buffer[11];
    *gz = (((int16_t)buffer[12]) << 8) | buffer[13];
	*mx = (((int16_t)buffer[14]) << 8) | buffer[15];
	*my = (((int16_t)buffer[18]) << 8) | buffer[19];
	*mz = (((int16_t)buffer[16]) << 8) | buffer[17];
}
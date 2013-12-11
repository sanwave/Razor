
#ifndef _RAZOR_H_
#define _RAZOR_H_

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include "MPU6050.h"
#include "HMC5883L.h"
#include "MS561101BA.h"
#include "nRF24L01.h"
#include "NEO6M0.h"
#include "PWM.h"
#include "WaveIMU.h"
#include "Controller.h"

#define STEER_ADDRESS 0
#define MOTOR1_ADDRESS 1
#define MOTOR2_ADDRESS 4
#define MOTOR3_ADDRESS 5
#define MOTOR4_ADDRESS 6
#define MOTOR_DEFAULT_VALUE 0
#define MOTOR_PULSE_RANGE 25

class Razor {
	public:
		Razor ();
		int initialize();
		int receiveMsg();
		int executeMsg();
		int sendInfo();
		int sendDateTime();
		int sendLocation();
		int readyToFly();
		int testHardware();
		int writeLog(char * logMsg);
		int updateSensors( bool force );
		int updateGesture();
		int updateControl();
		int updatePID(float p, float i, float d);
		int setPower(int value);
		int closePower();
		int startRecordVideo();
		int stopRecordVideo();
		int takePicture(int istyle, int iawb);
		int updateSysDateTime();
		int getCurrentDateTime(char * datetime);
		int printStatus();
		float getBaroAlt(float seaPress);
		float getBaroAlt();
		int debugTest();
		
		int flyingStatus, controlStatus, rcStatus;
		short int ax, ay, az, gx, gy, gz, mx, my, mz;
		float axf, ayf, azf, gxf, gyf, gzf, mxf, myf, mzf;
		float yawPitchRoll[3], yawPitchRollTarget[3];
		float temperature, pressure;
		int hour, minute, second;
		int year, month, day;
		double latitude, longitude;
		int mpus, hmcs, baros, neos, nrfs;
		int videoPid;
		int throttle;								//油门
		int timer, lastTimer, pictureTimer;
		char TxBuf[BUF_SIZE];
		char RxBuf[BUF_SIZE];
		char Buffer[BUF_SIZE];
		char cmd[BUF_SIZE];
		char time[150];
		char logText[256];
		
	private:
		MPU6050 mpu;
		HMC5883L hmc;
		MS561101BA baro;
		nRF24L01 nRF;
		NEO6M0 neo;
		PWM s1, x1, x2, x3, x4;
		WaveIMU imu;
		Controller con;
		float defSeaPress;
};

#endif
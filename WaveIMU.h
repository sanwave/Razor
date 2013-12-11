
#ifndef _WAVEIMU_H_
#define _WAVEIMU_H_

#include <stdio.h>
#include <cmath>

#define Kp 2.0f
#define Ki 0.005f
#define halfT 0.5f
#define PI 3.1415926f

class WaveIMU {
	public:
		WaveIMU();
		int initialize();
		int updateAHRS(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz);
		int getQ(float *q);		
		int getEulerRad(float *angles);
		int getEuler(float *angles);
		int getYawPitchRollRad(float *yrp);
		int getYawPitchRoll(float *yrp);
		int arr3RadToDeg(float *arr);
		float invSqrt(float number);
		
		float q0, q1, q2, q3;
		float exInt, eyInt, ezInt;
		float defSeaPress;
};

#endif
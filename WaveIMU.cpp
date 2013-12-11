
#include "WaveIMU.h"

WaveIMU::WaveIMU() {
	q0 = 1.0f;
	q1 = 0.0f;
	q2 = 0.0f;
	q3 = 0.0f;
	
	exInt = 0;
	eyInt = 0;
	ezInt = 0;
}

int WaveIMU::initialize() {
	return 1;
}

int WaveIMU::updateAHRS(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz) {
	float norm;
	float hx, hy, hz, bx, bz;
	float vx, vy, vz, wx, wy, wz;
	float ex, ey, ez;

	// auxiliary variables to reduce number of repeated operations
	float q0q0 = q0*q0;
	float q0q1 = q0*q1;
	float q0q2 = q0*q2;
	float q0q3 = q0*q3;
	float q1q1 = q1*q1;
	float q1q2 = q1*q2;
	float q1q3 = q1*q3;
	float q2q2 = q2*q2;   
	float q2q3 = q2*q3;
	float q3q3 = q3*q3;
	
	// normalise the measurements
	norm = invSqrt(ax*ax + ay*ay + az*az); 
	ax = ax * norm;
	ay = ay * norm;
	az = az * norm;
	norm = invSqrt(mx*mx + my*my + mz*mz);
	mx = mx * norm;
	my = my * norm;
	mz = mz * norm; 
	
	// compute reference direction of flux
	hx = 2*mx*(0.5 - q2q2 - q3q3) + 2*my*(q1q2 - q0q3) + 2*mz*(q1q3 + q0q2);
	hy = 2*mx*(q1q2 + q0q3) + 2*my*(0.5 - q1q1 - q3q3) + 2*mz*(q2q3 - q0q1);
	hz = 2*mx*(q1q3 - q0q2) + 2*my*(q2q3 + q0q1) + 2*mz*(0.5 - q1q1 - q2q2);         
	bx = sqrt((hx*hx) + (hy*hy));
	bz = hz;        
	
	// estimated direction of gravity and flux (v and w)
	vx = 2*(q1q3 - q0q2);
	vy = 2*(q0q1 + q2q3);
	vz = q0q0 - q1q1 - q2q2 + q3q3;
	wx = 2*bx*(0.5 - q2q2 - q3q3) + 2*bz*(q1q3 - q0q2);
	wy = 2*bx*(q1q2 - q0q3) + 2*bz*(q0q1 + q2q3);
	wz = 2*bx*(q0q2 + q1q3) + 2*bz*(0.5 - q1q1 - q2q2);  
	
	// error is sum of cross product between reference direction of fields and direction measured by sensors
	ex = (ay*vz - az*vy) + (my*wz - mz*wy);
	ey = (az*vx - ax*vz) + (mz*wx - mx*wz);
	ez = (ax*vy - ay*vx) + (mx*wy - my*wx);
	
	// integral error scaled integral gain
	exInt = exInt + ex*Ki;
	eyInt = eyInt + ey*Ki;
	ezInt = ezInt + ez*Ki;
	
	// adjusted gyroscope measurements
	gx = gx + Kp*ex + exInt;
	gy = gy + Kp*ey + eyInt;
	gz = gz + Kp*ez + ezInt;
	
	// integrate quaternion rate and normalise
	q0 = q0 + (-q1*gx - q2*gy - q3*gz)*halfT;
	q1 = q1 + (q0*gx + q2*gz - q3*gy)*halfT;
	q2 = q2 + (q0*gy - q1*gz + q3*gx)*halfT;
	q3 = q3 + (q0*gz + q1*gy - q2*gx)*halfT;  
	
	// normalise quaternion
	norm = invSqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
	q0 = q0 * norm;
	q1 = q1 * norm;
	q2 = q2 * norm;
	q3 = q3 * norm;
	
	printf("q[0]=%f, q[1]=%f, q[2]=%f, q[3]=%f\n", q0, q1, q2, q3);
	
	
	return 1;
}

int WaveIMU::getQ(float *q) {
	//updateAHRS();
	q[0] = q0;
	q[1] = q1;
	q[2] = q2;
	q[3] = q3;
	return 1;
}

int WaveIMU::getEulerRad(float *angles) {
	float q[4]; // quaternion
	getQ(q);
	angles[0] = atan2(2 * q[1] * q[2] - 2 * q[0] * q[3], 2 * q[0]*q[0] + 2 * q[1] * q[1] - 1); // psi
	angles[1] = -asin(2 * q[1] * q[3] + 2 * q[0] * q[2]); // theta
	angles[2] = atan2(2 * q[2] * q[3] - 2 * q[0] * q[1], 2 * q[0] * q[0] + 2 * q[3] * q[3] - 1); // phi
	return 1;
}

int WaveIMU::getEuler(float *angles) {
	getEulerRad(angles);
	arr3RadToDeg(angles);
	return 1;
}

int WaveIMU::getYawPitchRollRad(float *ypr) {
	float q[4]; // quaternion
	float gx, gy, gz; // estimated gravity direction
	getQ(q);

	gx = 2 * (q[1]*q[3] - q[0]*q[2]);
	gy = 2 * (q[0]*q[1] + q[2]*q[3]);
	gz = q[0]*q[0] - q[1]*q[1] - q[2]*q[2] + q[3]*q[3];

	ypr[0] = atan2(2 * q[1] * q[2] - 2 * q[0] * q[3], 2 * q[0]*q[0] + 2 * q[1] * q[1] - 1);
	ypr[1] = atan(gx / sqrt(gy*gy + gz*gz));
	ypr[2] = atan(gy / sqrt(gx*gx + gz*gz));
	
	return 1;
}

int WaveIMU::getYawPitchRoll(float *ypr) {
	getYawPitchRollRad(ypr);
	arr3RadToDeg(ypr);
	return 1;
}

int WaveIMU::arr3RadToDeg(float *arr) {
	arr[0] *= 180/PI;
	arr[1] *= 180/PI;
	arr[2] *= 180/PI;
	return 1;
}

float WaveIMU::invSqrt(float number) {
	volatile long i;
	volatile float x, y;
	volatile const float f = 1.5F;

	x = number * 0.5F;
	y = number;
	i = * ( long * ) &y;
	i = 0x5f375a86 - ( i >> 1 );
	y = * ( float * ) &i;
	y = y * ( f - ( x * y * y ) );
	return y;
}
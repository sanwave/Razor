
#include "Controller.h"

Controller::Controller() {
	PID_ROL.P = PID_PITCH.P = PID_YAW.P = 0;
	PID_ROL.I = PID_PITCH.I = PID_YAW.I = 0;
	PID_ROL.D = PID_PITCH.D = PID_YAW.D = 0;
}

int Controller::update(int throttle, float *yprNow, float *yprTar, float *motor, float gx, float gy) {	
	float ypr[3];
	for(int i=0;i<3;++i) {
		ypr[i] = yprTar[0] - yprNow[0];
	}
	
	if(throttle<0) {
		throttle=0;//防止出现负数；
	}
	else if(throttle>THROTTLE_MAX) {
		throttle=THROTTLE_MAX;
	}
	
	PID_ROL.IMAX = throttle/2;
	limit(&PID_ROL.IMAX,1000,0);
	PID_PITCH.IMAX = PID_ROL.IMAX;
	
	PID_ROL.pout = PID_ROL.P * ypr[2];
	PID_PITCH.pout = PID_PITCH.P * ypr[1];
	PID_YAW.pout = PID_YAW.P * ypr[0];	
	
	if(yprTar[2]*yprTar[2]<0.1 && yprTar[1]*yprTar[1]<0.1 && yprNow[2]*yprNow[2]<30 && yprNow[1]*yprNow[1]<30 && throttle>300)
	{
		PID_ROL.iout += PID_ROL.I * ypr[2];
		PID_PITCH.iout += PID_PITCH.I * ypr[1];
		limit(&PID_ROL.iout,PID_ROL.IMAX,-PID_ROL.IMAX);
		limit(&PID_PITCH.iout,PID_PITCH.IMAX,-PID_PITCH.IMAX);
	}//认为在平衡位置（小角度内动态平衡）时，积分进行更新，积分进行限幅，防止在大角度时产生积分超调；
	else if(throttle<200)
	{
		PID_ROL.iout = 0;
		PID_PITCH.iout = 0;
	}
	
	PID_ROL.dout = PID_ROL.D * gx;
	PID_PITCH.dout = PID_PITCH.D * gy;
	
	
	PID_ROL.OUT = PID_ROL.pout + PID_ROL.iout + PID_ROL.dout;
	PID_PITCH.OUT = PID_PITCH.pout + PID_PITCH.iout + PID_PITCH.dout;
	PID_YAW.OUT = PID_YAW.pout + PID_YAW.iout + PID_YAW.dout;
	
	if(throttle>200)
	{
		motor[0] = throttle - PID_ROL.OUT - PID_PITCH.OUT + PID_YAW.OUT;
		motor[1] = throttle + PID_ROL.OUT - PID_PITCH.OUT - PID_YAW.OUT;
		motor[2] = throttle + PID_ROL.OUT + PID_PITCH.OUT + PID_YAW.OUT;
		motor[3] = throttle - PID_ROL.OUT + PID_PITCH.OUT - PID_YAW.OUT;
	}
	else
	{
		motor[0] = 0;
		motor[1] = 0;
		motor[2] = 0;
		motor[3] = 0;
	}
	
	return 1;
}

int Controller::limit(float *value, float max, float min) {
	if(*value<min) {
		*value = min;
	}
	else if (*value>max) {
		*value = max;
	}
	return 1;
}

int Controller::changePID(int p, int i, int d) {
	PID_ROL.P = PID_PITCH.P = PID_YAW.P = p;
	PID_ROL.I = PID_PITCH.I = PID_YAW.I = i;
	PID_ROL.D = PID_PITCH.D = PID_YAW.D = d;
	return 1;
}
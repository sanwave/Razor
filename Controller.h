
#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#define THROTTLE_MAX 1000

typedef struct {
			float P,pout,I,iout,D,dout,IMAX,OUT;
		}PID;

class Controller {
	public:
		Controller();
		int update(int throttle, float *yprNow, float *yprTar, float *motor, float gx, float gy);
		int changePID(int p, int i, int d);
		int limit(float *value, float max, float min);
		PID PID_ROL, PID_PITCH, PID_YAW;
};

#endif
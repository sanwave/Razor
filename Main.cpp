
#include "Razor.h"

Razor raz;
int timer, lastTimer0, lastTimer1, pictureTimer;

int main (int argc,char* argv[]) { 	
	wiringPiSetup();
	int razs = raz.initialize();
	if(razs < 0) {
		printf("Error: Razor can not initialize.\n");
		printf("more info in the log");
		return 0;
	}
	
	timer = lastTimer0 = lastTimer1 = 0;
	
	raz.readyToFly();
	
	while (1) {
		timer = micros();
		printf("\n\n");
		
		while(1) {
			if((timer - lastTimer0) / 9500 ) {
				lastTimer0 = timer;
				break;
			}
			delayMicroseconds(10);
		}
		
		raz.updateSensors(true);
		raz.updateGesture();
		//raz.updateControl();
		
		if( raz.receiveMsg() == 1 ) {			
			raz.executeMsg();			
		}
		else {
			int c=0;
			printf("please input command for action.\n");
			printf("1: take a picture;\n");
			printf("2: start video recording.\n");
			printf("3: stop video recording.\n");
			printf("4: print GPS info.\n");
			printf("5: update System Date and Time.\n");
			printf("6: adding throttle 80.\n");
			printf("7: removing throttle 80.\n");
			printf("8: change PID.\n");
			printf("9: set PWM.\n");
			if(((timer - lastTimer1) / 10000) > 1 ) {
				lastTimer1 = timer;
				scanf("%d", &c);
			}
			else {
				c=4;
			}
			switch(c) {
				case 1:
					raz.takePicture(1, 0);
					break;
					
				case 2:
					if(raz.startRecordVideo() == 0) {
						return 0;
					}
					break;
					
				case 3:
					raz.stopRecordVideo();
					break;
					
				case 4:
					raz.debugTest();
					break;
					
				case 5:
					raz.updateSysDateTime();
					break;
					
				case 6:
					raz.throttle += 80;
					break;
					
				case 7:
					raz.throttle -= 80;
					break;
				
				case 8:
					float p,i,d;
					printf("please input p, i and d:");
					scanf("%f%f%f",&p, &i, &d);
					raz.updatePID(p, i, d);
					break;
					
				case 9:
					int t;
					printf("please input throttle:");
					scanf("%d", &t);
					raz.setPower(t);
					break;
					
				default:
					break;			
			}
			//delay(1000);
		}
	}
	
	return 0;
}
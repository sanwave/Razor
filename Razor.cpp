
#include "Razor.h"

Razor::Razor() {
	defSeaPress = 1013.25;
}

/**************************************************************************/
/*     初始化各个传感器                                                   */
/* 返回值：1=>成功完成， 0=>完成但无效， -1=>表示出错。  下同，不赘述     */
/**************************************************************************/
int Razor::initialize() {
	mpus = mpu.initialize();
	hmcs = hmc.initialize();
	baros = baro.initialize();
	nrfs = nRF.initialize();
	neos = neo.initialize();
	mpu.EnabledHMC5883L();
	imu.initialize();
	
	if(mpus < 0) {
		sprintf(logText, "Error: MPU6050 initialize failed!\n");
		writeLog(logText);
		return -1;
	}
	if(hmcs < 0) {
		sprintf(logText, "Error: HMC5883L initialize failed!\n");
		writeLog(logText);
		return -1;
	}
	if(baros < 0) {
		sprintf(logText, "Error: MS561101BA initialize failed!\n");
		writeLog(logText);
		return -1;
	}
	if(nrfs < 0) {
		sprintf(logText, "Error: nRF24l01 initialize failed!\n");
		writeLog(logText);
		return -1;
	}
	if(neos < 0) {
		sprintf(logText, "Error: GPS initialize failed!\n");
		writeLog(logText);
		return -1;
	}
	
	nRF.RX_Mode();	
	sprintf(logText, "***********IMU Complete Initialization.*****************\n");
	writeLog(logText);
	return 1;
}

/**************************************************************************/
/*     接收遥控消息                                                       */
/**************************************************************************/
int Razor::receiveMsg() {
	char *msg = RxBuf;
	int m = nRF.RxPacket(msg);	
	sprintf(logText, "%s\t Receive Message:%s\n", time, msg);
	writeLog(logText);
	return m;
}

/**************************************************************************/
/*     解析并执行消息内容，未完                                                 */
/**************************************************************************/
int Razor::executeMsg() {
	char *cmd = RxBuf;
	if(*cmd == 0x00) {
		return 0;
	}
	char * statement = cmd;
	char *msg, *field;
	
	while (1) {
		if(*statement == 0x00) {
			break;
		}
		
		msg = Stringdev::spiltHalf(&statement, '&');
		field = Stringdev::spiltHalf(&msg, '@');
		
		if(strcmp(field, "info") == 0) {
			continue;
		}
		
		if(strcmp(field, "gas") == 0) {
			int gas = 0;
			for(int i=0;msg[i]!=0x00;++i) {
				gas = gas * 10 + msg[i] - '0';
			}
			if(gas > 0 && gas <100) {
				throttle = gas;
			}
		}
		
		if(strcmp(field, "cmd") == 0) {
			if(strcmp(field, "UpdateTime") == 0) {
				updateSysDateTime();
			}
			else if(strcmp(field, "StartVideo") == 0) {
				startRecordVideo();
			}
			else if(strcmp(field, "StopVideo") == 0) {
				stopRecordVideo();
			}
			else if(strcmp(field, "TakePicture") == 0) {				
				takePicture(0, 0);
			}			
		}
	}
	return 1;
}

/**************************************************************************/
/*     发送消息                                                 */
/**************************************************************************/
int Razor::sendInfo() {
	*TxBuf = 0x00;
	sprintf(TxBuf, "info@%02d:%02d", minute, second);
	nRF.TxPacket(TxBuf);
}

/**************************************************************************/
/*     发送日期时间信息                                                   */
/**************************************************************************/
int Razor::sendDateTime() {
	*TxBuf = 0x00;
	sprintf(TxBuf, "datetime@%04d/%02d/%02d,%02d:%02d:%02d", year, month, day, hour, minute, second);
	nRF.TxPacket(TxBuf);
}

/**************************************************************************/
/*     发送位置信息                                                       */
/**************************************************************************/
int Razor::sendLocation() {
	*TxBuf = 0x00;
	sprintf(TxBuf, "location@%09.6f,%010.6f", latitude, longitude);
	nRF.TxPacket(TxBuf);
}

/**************************************************************************/
/*     准备起飞，初始化四路电调PWM并舵机PWM                               */
/**************************************************************************/
int Razor::readyToFly() {
	s1.initialize( STEER_ADDRESS, MOTOR_DEFAULT_VALUE, MOTOR_PULSE_RANGE );
	x1.initialize( MOTOR1_ADDRESS, MOTOR_DEFAULT_VALUE, MOTOR_PULSE_RANGE );
	x2.initialize( MOTOR2_ADDRESS, MOTOR_DEFAULT_VALUE, MOTOR_PULSE_RANGE );
	x3.initialize( MOTOR3_ADDRESS, MOTOR_DEFAULT_VALUE, MOTOR_PULSE_RANGE );
	x4.initialize( MOTOR4_ADDRESS, MOTOR_DEFAULT_VALUE, MOTOR_PULSE_RANGE );
}

/**************************************************************************/
/*     测试硬件连接，差气压计并nRF                                                       */
/**************************************************************************/
int Razor::testHardware() {
	if(mpu.testConnection()) {
		mpus = 1;
	}
	else {
		mpus = 0;		
		sprintf(logText, "%s\t Test Hardware: MPU6050 is lost.\n", time);
		writeLog(logText);
	}
	
	if(neo.year != 0) {
		neos = 1;
	}
	else {
		neos = 0;
		sprintf(logText, "%s\t Test Hardware: GPS is lost.\n", time);
		writeLog(logText);
	}
	
	return mpus & neos;
}

/**************************************************************************/
/*     记录日志，未完                                                 */
/**************************************************************************/
int Razor::writeLog(char * logMsg) {
	printf("%s", logMsg);
}

/**************************************************************************/
/*     更新传感器数据                                                     */
/**************************************************************************/
int Razor::updateSensors( bool force ) {
	mpu.getMotion9(&ax, &ay, &az, &gx, &gy, &gz, &mx, &my, &mz);
	
	axf = ax / 16384.0;
	ayf = ay / 16384.0;
	azf = az / 16384.0;
	gxf = gx / 131.0;
	gyf = gy / 131.0;
	gzf = gz / 131.0;
	mxf = mx / 1090.0;
	myf = my / 1090.0;
	mzf = mz / 1090.0;
	
	lastTimer = timer;
	timer = millis();
	
	if( force || ((lastTimer - timer) % 400)) {
		temperature = baro.getTemperature(MS561101BA_OSR_4096);
		pressure = baro.getPressure(MS561101BA_OSR_4096);
	}
	
	if( force || ((lastTimer - timer) % 1000)) {
		neo.updateInfos( Buffer );
		if(neo.hour <= 24 && neo.hour >= 0) {
			hour = neo.hour;
		}
		if(neo.minute <= 60 && neo.minute >= 0) {
			minute = neo.minute;
		}
		if(neo.second <= 60 && neo.second >= 0) {
			second = (int)neo.second;
		}
		if(neo.year >= 2013 && neo.year <= 2100 ) {
			year = neo.year;
		}
		if(neo.month >= 0 && neo.month <= 12 ) {
			month = neo.month;
		}
		if(neo.day >= 0 && neo.day <= 31) {
			day = neo.day;
		}
		latitude = neo.latitude;
		longitude = neo.longitude;
		sprintf(time, "%02d:%02d:%02d", hour, minute, second);
	}
}

/**************************************************************************/
/*     计算控制，更新油门                                                 */
/**************************************************************************/
int Razor::updateGesture() {
	imu.updateAHRS(gxf * PI / 180.0f, gyf * PI / 180.0f, gzf * PI / 180.0f, axf, ayf, azf, mxf, myf, mzf);
	imu.getYawPitchRoll(yawPitchRoll);
	return 1;
}

/**************************************************************************/
/*     计算控制，更新油门                                                 */
/**************************************************************************/
int Razor::updateControl() {
	float motor[4];
	con.update(throttle, yawPitchRoll, yawPitchRollTarget, motor, gx, gy);
	x1.writePulse( MOTOR1_ADDRESS, motor[0] );
	x2.writePulse( MOTOR2_ADDRESS, motor[1] );
	x3.writePulse( MOTOR3_ADDRESS, motor[2] );
	x4.writePulse( MOTOR4_ADDRESS, motor[3] );
	
	return 1;
}

/**************************************************************************/
/*     更新PID参数                                                        */
/**************************************************************************/
int Razor::updatePID(float p, float i, float d) {
	return con.changePID(p, i, d);
}

/**************************************************************************/
/*     设定油门输出值                                                        */
/**************************************************************************/
int Razor::setPower(int value) {
	x1.writePulse( MOTOR1_ADDRESS, value );
	x2.writePulse( MOTOR2_ADDRESS, value );
	x3.writePulse( MOTOR3_ADDRESS, value );
	x4.writePulse( MOTOR4_ADDRESS, value );
}

/**************************************************************************/
/*     关闭动力，慎用                                                     */
/**************************************************************************/
int Razor::closePower() {
	x1.writePulse( MOTOR1_ADDRESS, 0 );
	x2.writePulse( MOTOR2_ADDRESS, 0 );
	x3.writePulse( MOTOR3_ADDRESS, 0 );
	x4.writePulse( MOTOR4_ADDRESS, 0 );
}

/**************************************************************************/
/*     Linux进程信号处理                                                  */
/**************************************************************************/
void sigroutine(int dunno) { /* 信号处理例程，其中dunno将会得到信号的值 */
	switch (dunno) {
		case 1:
			printf("Get a signal -- SIGHUP ");
			exit(0);
			break;
		case 2:
			printf("Get a signal -- SIGINT ");
			exit(0);
			break;
		case 3:
			printf("Get a signal -- SIGQUIT ");
			exit(0);
			break;
	}
	return;
}

/**************************************************************************/
/*     开始录像                                                           */
/**************************************************************************/
int Razor::startRecordVideo() {
	if(videoPid > 0) {
		return 0;
	}
	videoPid = fork();
	if (videoPid == 0) {
		//signal(SIGINT, sigroutine);
		signal(SIGCHLD,SIG_IGN);
		execl("/bin/sh", "sh", "-c", "raspivid -t -0 -awb cloud -w 1280 -h 720 -fps 24 -b 5000000 -o - | ffmpeg -i - -vcodec copy -an -metadata streamName=Razor -f flv tcp://localhost:6666", NULL);
		return 0;
	}
	else if(videoPid == -1){
		sprintf(logText, "%s\tVideo Record Failed.\n", time);
		writeLog(logText);
		return -1;
	}
	else {
		sprintf(logText, "%s\tStart Video Recording. The process ID is %d.\n", time, videoPid);
		writeLog(logText);
		return 1;
	}
}

/**************************************************************************/
/*     停止录像                                                           */
/**************************************************************************/
int Razor::stopRecordVideo() {
	if (videoPid <= 0) {
		return 0;
	}	
	sprintf(cmd, "sudo killall raspivid&");
	system(cmd);
	sprintf(cmd, "sudo kill -9 %d&", videoPid);
	system(cmd);
	videoPid = 0;
	return 1;
}

/**************************************************************************/
/*     照相                                                               */
/**************************************************************************/
int Razor::takePicture(int istyle, int iawb) {
	if( millis() - pictureTimer < 1000 ) {
		return -1;
	}
	else {
		pictureTimer = millis();
	}
	if( videoPid > 0 ) {
		stopRecordVideo();
	}
	char style[16] = {0}, awb[16] = {0}, location[200] = " ",  dateTime[150] = " ";
	if(istyle == 1) {
		sprintf(style, "-ifx sketch");
	}
	if(iawb == 1) {
		sprintf(awb,"-awb cloud");
	}
	if(neo.fixStatus == 1 || latitude > 0.1 ||latitude < -0.1 ) {
		int latDegree, latMinute, latSecondH;
		int lonDegree, lonMinute, lonSecondH;
		char latRef = 'N', lonRef = 'E';
		if(latitude < 0) {
			latitude *= -1;
			latRef = 'S';
		}
		if(longitude < 0) {
			longitude *= -1;
			lonRef = 'N';
		}		
		latDegree = (int)latitude ;
		latMinute = int((latitude - latDegree) * 60);
		latSecondH = int((latitude - latDegree - latMinute / 60) * 360000);
		lonDegree = (int)longitude ;
		lonMinute = int((longitude - lonDegree) * 60);
		lonSecondH = int((longitude - lonDegree - lonMinute / 60) * 360000);		
		sprintf(location, "-x GPS.GPSLatitude=%02d/1,%02d/1,%04d/100 -x GPS.GPSLatitudeRef=%c -x GPS.GPSLongitude=%03d/1,%02d/1,%04d/100 -x GPS.GPSLongitudeRef=%c", latDegree, latMinute, latSecondH, latRef, lonDegree, lonMinute, lonSecondH, lonRef);
	}
	if(year>0) {
		sprintf(dateTime, "-x EXIF.DateTimeOriginal='%4d-%2d-%2d %2d:%2d:%2d' -x EXIF.DateTimeDigitized='%4d-%2d-%2d %2d:%2d:%2d'", 
			year, month, day, hour, minute, second, year, month, day, hour, minute, second);
	}
	char fileName[80];
	sprintf(fileName, "/home/pi/Razor/Image/imageR%04d%02d%02d%02d%02d%02d.jpg", year, month, day, hour, minute, second);
	if((access("/home/pi/Raozr/Image", F_OK))!=-1) {
		int s = mkdir("/home/pi/Raozr/Image",  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		if(s<0) {
			return -1;
		}
	}
	if((access(fileName, F_OK))!=-1)  
	{
		sprintf(fileName, "/home/pi/Razor/Image/imageR%04d%02d%02d%02d%02d%02dF.jpg", year, month, day, hour, minute, second);
	}
					
	sprintf(cmd, "sudo raspistill -t 0 -w 1920 -h 1080 %s %s %s -x IFD0.Model='Wave Ras.Pi' -x IFD0.Software='Raspberry Pi' -x IFD0.Artist=Wave  -o - > %s &", style, awb, location, fileName );
	printf("%s\n",cmd);
	system(cmd);
	sprintf(logText, "%s\ttake a picture. filename=%s\n", time, fileName);
	writeLog(logText);
	return 1;
}

/**************************************************************************/
/*     更新系统时间                                                     */
/**************************************************************************/
int Razor::updateSysDateTime() {
	updateSensors(true);	
	char cmd[64];
	if(hour>16) {
		++day;
	}
	hour = (hour + 8 ) % 24;
	sprintf(cmd, "sudo date -s \"%d:%d:%d %d-%d-%d\"", hour, minute, second, year, month, day );
	system(cmd);
	return 1;
}

/**************************************************************************/
/*     获取当前时间，输出到datetime中                                     */
/**************************************************************************/
int Razor::getCurrentDateTime(char * datetime) {
	sprintf(datetime, "Now is %d:%d:%d, %d-%d-%d", hour, minute, second, year, month, day);
	return 1;
}
/**************************************************************************/
/*     输出当前状态，未完                                                 */
/**************************************************************************/
int Razor::printStatus() {
	if(rcStatus == 0) {
		printf("Status: Remote Control is lost!\n");
	}
	else {
		printf("Status: Remote Control is in connection.\n");
	}
	
	switch (flyingStatus) {
		case 0:
			printf("Status: flying in primary control.\n");
			break;
		
		case 1:
			printf("Status: flying in launching.(take off)\n");
			break;
		
		case 2:
			printf("Status: flying in landing.\n");
			break;
		
		case 3:
			printf("Status: flying in simple control.\n");
			break;
		
		case 4:
			printf("Status: flying in auto navigating.\n");
			break;
		
		case 5:
			printf("Status: flying in heading back.\n");
			break;
		
		case 6:
			printf("Status: flying in tarcking something.\n");
			break;
		
		default:
			printf("Status: flying in unknown mode.\n");
			break;
	}
	return 1;
}

float Razor::getBaroAlt(float seaPress) {
	float temp = baro.getTemperature(MS561101BA_OSR_4096);
	float press = baro.getPressure(MS561101BA_OSR_4096);
	return ((pow((seaPress / press), 1/5.257) - 1.0) * (temp + 273.15)) / 0.0065;
}

float Razor::getBaroAlt() {
	return getBaroAlt(defSeaPress);
}

int Razor::debugTest() {
	printf("******Razor Test Info******\n");	
	printf("\tTime: %02d:%02d:%02d\n", hour, minute, second);
	printf("\tDate: %04d/%02d/%02d\n", year, month, day);
	printf("\tLatitude: %-8.5f\tLongitude:%-9.5f\tAltitude:%-3.1f\n", latitude, longitude, neo.altitude);
	printf("\tAcc: %5.3f %5.3f %5.3f Gyro: %5.3f %5.3f %5.3f Mag %5.3f %5.3f %5.3f\n", axf, ayf, azf, gxf, gyf, gzf, mxf, myf, mzf);
	printf("\tThrottle: %d Motor Value: %d %d %d %d  Steering: %d\n", throttle, x1.value, x2.value, x3.value, x4.value, s1.value );
	printf("\tTemperature: %4.1f Pressure: %6.1f\n", temperature, pressure);
	printf("\tGesture: y %5.3f;\tp %5.3f;\tr %5.3f\n", yawPitchRoll[0], yawPitchRoll[1], yawPitchRoll[2]);
	//printf();
	printStatus();
	return 1;
}

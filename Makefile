#Razor Makefile
Razor: Main.cpp Razor.o  MS561101BA.o MPU6050.o HMC5883L.o I2Cdev.o nRF24L01.o SPIdev.o NEO6M0.o Uartdev.o Stringdev.o PWM.o WaveIMU.o Controller.o
	gcc -o Razor Main.cpp Razor.o MS561101BA.o MPU6050.o HMC5883L.o I2Cdev.o nRF24L01.o SPIdev.o NEO6M0.o Uartdev.o Stringdev.o PWM.o WaveIMU.o Controller.o -lwiringPi -lm
Razor.o: Razor.h
	gcc -c Razor.cpp -lwiringPi
MS561101BA.o:MS561101BA.h
	gcc -c MS561101BA.cpp -lwiringPi
MPU6050.o: MPU6050.h
	gcc -c MPU6050.cpp -lwiringPi
HMC5883L.o:HMC5883L.h
	gcc -c HMC5883L.cpp -lwiringPi
I2Cdev.o:I2Cdev.h Type.h
	gcc -c I2Cdev.cpp -lwiringPi
nRF24L01.o:nRF24L01.h
	gcc -c nRF24L01.cpp -lwiringPi
SPIdev.o:SPIdev.h Type.h
	gcc -c SPIdev.cpp -lwiringPi
NEO6M0.o: NEO6M0.h Stringdev.h
	gcc -c NEO6M0.cpp -lwiringPi
Uartdev.o: Uartdev.h Type.h
	gcc -c Uartdev.cpp -lwiringPi
Stringdev.o:Stringdev.h
	gcc -c Stringdev.cpp -lwiringPi
PWM.o:PWM.h
	gcc -c PWM.cpp -lwiringPi
WaveIMU.o:WaveIMU.h
	gcc -c WaveIMU.cpp -lwiringPi -lm
Controller.o:Controller.h
	gcc -c Controller.cpp -lwiringPi
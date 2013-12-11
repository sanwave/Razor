
#ifndef MS561101BA_h
#define MS561101BA_h

#include "I2Cdev.h"

extern unsigned int micros (void) ;

// addresses of the device
#define MS561101BA_ADDR_CSB_HIGH  0x76   //CBR=1 0x76 I2C address when CSB is connected to HIGH (VCC)
#define MS561101BA_ADDR_CSB_LOW   0x77   //CBR=0 0x77 I2C address when CSB is connected to LOW (GND)
#define MS561101BA_DEFAULT_ADDRESS MS561101BA_ADDR_CSB_LOW

// registers of the device
#define MS561101BA_D1 0x40
#define MS561101BA_D2 0x50
#define MS561101BA_RESET 0x1E

// D1 and D2 result size (bytes)
#define MS561101BA_D1D2_SIZE 3

// OSR (Over Sampling Ratio) constants
#define MS561101BA_OSR_256 0x00
#define MS561101BA_OSR_512 0x02
#define MS561101BA_OSR_1024 0x04
#define MS561101BA_OSR_2048 0x06
#define MS561101BA_OSR_4096 0x08

#define MS561101BA_PROM_BASE_ADDR 0xA2 // by adding ints from 0 to 6 we can read all the prom configuration values. 
// C1 will be at 0xA2 and all the subsequent are multiples of 2
#define MS561101BA_PROM_REG_COUNT 6 // number of registers in the PROM
#define MS561101BA_PROM_REG_SIZE 2 // size in bytes of a prom registry.



class MS561101BA {
  public:
    MS561101BA();
	MS561101BA(uint8_t addr);
    int initialize();
    float getPressure(uint8_t OSR);
    float getTemperature(uint8_t OSR);
    int32_t getDeltaTemp(uint8_t OSR);
    uint32_t rawPressure(uint8_t OSR);
    uint32_t rawTemperature(uint8_t OSR);
    int readPROM();
    void reset();
    uint32_t lastPresConv, lastTempConv;
  private:
    void startConversion(uint8_t command);
    uint32_t getConversion(uint8_t command);	
    uint8_t devAddr;
	int msAddr;
    uint16_t _C[MS561101BA_PROM_REG_COUNT];
    uint32_t pressCache, tempCache;
};

#endif // MS561101BA_h
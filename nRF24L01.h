/*
 * nRF24L01.h
 *
 *  Created on: 2013-9-5
 *      Author: i24
 */

/*   _ _ _ _ _ _ _ _ _ _ _ _
 *  |1|2|  1->GND  2->VCC  |
 *  |-|-|                  |
 *  |3|4|  3->CE   4->CSN  |
 *  |-|-|                  |
 *  |5|6|  5->SCLK  6->MOSI |
 *  |-|-|                  |
 *  |7|8|  7->MISO 8->IRQ  |
 *  |_|_|_ _ _ _ _ _ _ _ _ |
 */

/***************************
* CE->CE1
* CSN->CE0
* SCK->SCK
* MOSI->MOSI
* MISO->MISO
*****************************/

#ifndef NRF24L01_H_
#define NRF24L01_H_

#include "SPIdev.h"

//*********************************************NRF24L01*************************************
#define ADR_WIDTH 		5
#define PLOAD_WIDTH 	32
#define TX_ADR_WIDTH    ADR_WIDTH   	// 5 uints TX address width
#define RX_ADR_WIDTH    ADR_WIDTH   	// 5 uints RX address width
#define TX_PLOAD_WIDTH  PLOAD_WIDTH  	// 20 uints TX payload
#define RX_PLOAD_WIDTH  PLOAD_WIDTH  	// 20 uints TX payload

// SPI(nRF24L01) commands
#define READ_REG        0x00  // Define read command to register
#define WRITE_REG       0x20  // Define write command to register
#define RD_RX_PLOAD     0x61  // Define RX payload register address
#define WR_TX_PLOAD     0xA0  // Define TX payload register address
#define FLUSH_TX        0xE1  // Define flush TX register command
#define FLUSH_RX        0xE2  // Define flush RX register command
#define REUSE_TX_PL     0xE3  // Define reuse TX payload register command
//#define NOP             0xFF  // Define No Operation, might be used to read status register

//***************************************************//
// SPI(nRF24L01) registers(addresses)
#define CONFIG          0x00  // 'Config' register address
#define EN_AA           0x01  // 'Enable Auto Acknowledgment' register address
#define EN_RXADDR       0x02  // 'Enabled RX addresses' register address
#define SETUP_AW        0x03  // 'Setup address width' register address
#define SETUP_RETR      0x04  // 'Setup Auto. Retrans' register address
#define RF_CH           0x05  // 'RF channel' register address
#define RF_SETUP        0x06  // 'RF setup' register address
#define STATUS          0x07  // 'Status' register address
#define OBSERVE_TX      0x08  // 'Observe TX' register address
#define CD              0x09  // 'Carrier Detect' register address
#define RX_ADDR_P0      0x0A  // 'RX address pipe0' register address
#define RX_ADDR_P1      0x0B  // 'RX address pipe1' register address
#define RX_ADDR_P2      0x0C  // 'RX address pipe2' register address
#define RX_ADDR_P3      0x0D  // 'RX address pipe3' register address
#define RX_ADDR_P4      0x0E  // 'RX address pipe4' register address
#define RX_ADDR_P5      0x0F  // 'RX address pipe5' register address
#define TX_ADDR         0x10  // 'TX address' register address
#define RX_PW_P0        0x11  // 'RX payload width, pipe0' register address
#define RX_PW_P1        0x12  // 'RX payload width, pipe1' register address
#define RX_PW_P2        0x13  // 'RX payload width, pipe2' register address
#define RX_PW_P3        0x14  // 'RX payload width, pipe3' register address
#define RX_PW_P4        0x15  // 'RX payload width, pipe4' register address
#define RX_PW_P5        0x16  // 'RX payload width, pipe5' register address
#define FIFO_STATUS     0x17  // 'FIFO Status Register' register address

#define MAX_RT  		0x10  	// Max #of TX retrans interrupt
#define TX_DS   		0x20  	// TX data sent interrupt
#define RX_DR   		0x40  	// RX data received

#define CE_Pin          11

//-----------------------------------------------------------------------------
#define nRF24L01_CE_H digitalWrite(CE_Pin, HIGH)
#define nRF24L01_CE_L digitalWrite(CE_Pin, LOW)


class nRF24L01 {
	public:
		int initialize();
		uint8_t SPI_RW(uint8_t byte);
		uint8_t SPI_RW_Reg(uint8_t reg, uint8_t value);
		uint8_t SPI_Read(uint8_t reg);
		uint8_t SPI_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t bytes);
		uint8_t SPI_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t bytes);
		void RX_Mode(void);
		void TX_Mode(void);
		uint8_t RxPacket(int8_t* rx_buf);
		void TxPacket(int8_t * tx_buf);
		uint8_t TxBuf[32];
		uint8_t RxBuf[32];
		
	private:		
		//uint8_t TX_ADDRESS[TX_ADR_WIDTH];
		//uint8_t RX_ADDRESS[RX_ADR_WIDTH];
		uint8_t sta;
};

#endif /* NRF24L01_H_ */

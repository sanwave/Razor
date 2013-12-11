/*
 * nRF24L01.cpp
 *
 *  Created on: 2013-11-25
 *      Author: Wave
 */

#include "nRF24L01.h"

int nRF24L01::initialize(void)
{
	uint8_t TX_ADDRESS[TX_ADR_WIDTH]= {0x34,0x43,0x10,0x10,0x01};	//本地地址
	uint8_t RX_ADDRESS[RX_ADR_WIDTH]= {0x34,0x43,0x10,0x10,0x01};	//接收地址
	delay(100);
	int nRF = SPIdev::setup(0, 10000000);
	if ( nRF < 0) {
		printf("Attention: Can not initialize nRF24l01.\n");
		return 1;
	}
	pinMode(CE_Pin, OUTPUT);	
	nRF24L01_CE_L;
	SPI_Write_Buf(WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH); 
	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, RX_ADDRESS, RX_ADR_WIDTH); 
	SPI_Write_Buf(WR_TX_PLOAD, TxBuf, TX_PLOAD_WIDTH); 
	
	SPI_RW_Reg(WRITE_REG + EN_AA, 0x01); 
  	SPI_RW_Reg(WRITE_REG + EN_RXADDR, 0x01);  
  	//SPI_RW_Reg(WRITE_REG + SETUP_RETR, 0x1a); 		//自动重发
  	SPI_RW_Reg(WRITE_REG + RF_CH, 40); 
  	SPI_RW_Reg(WRITE_REG + RF_SETUP, 0x07); 	
	SPI_RW_Reg(WRITE_REG + CONFIG, 0x0f);
	delay(2);		//延时1.5ms, 供内部晶体振荡用
	return 0;
}

uint8_t nRF24L01::SPI_RW(uint8_t byte)
{
	SPIdev::dataRW(0, &byte, 1);
    return byte;           		  		// return read byte
}

uint8_t nRF24L01::SPI_RW_Reg(uint8_t reg, uint8_t value)
{
	uint8_t status;		
	status = SPI_RW(reg);      // select register
  	SPI_RW(value);             // ..and write value to it..	
  	return(status);            // return nRF24L01 status byte
}

uint8_t nRF24L01::SPI_Read(uint8_t reg)
{
	uint8_t reg_val;
  	SPI_RW(reg);            // Select register to read from..
  	reg_val = SPI_RW(0);    // ..then read registervalue
  	return(reg_val);        // return register value
}

uint8_t nRF24L01::SPI_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t bytes)
{
	uint8_t status,byte_ctr;
  	status = SPI_RW(reg);       		// Select register to write to and read status byte
	for(byte_ctr=0;byte_ctr<bytes;byte_ctr++)
    	pBuf[byte_ctr] = SPI_RW(0);    // Perform SPI_RW to read byte from nRF24L01
  	return(status);                    // return nRF24L01 status byte
}

uint8_t nRF24L01::SPI_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t bytes)
{
	uint8_t status,byte_ctr;
  	status = SPI_RW(reg);    					// Select register to write to and read status byte
  	for(byte_ctr=0; byte_ctr<bytes; byte_ctr++) // then write all byte in buffer(*pBuf)
    	SPI_RW(*pBuf++);
  	return(status);          					// return nRF24L01 status byte
}

void nRF24L01::RX_Mode(void)
{
	nRF24L01_CE_L;
  	SPI_RW_Reg(WRITE_REG + CONFIG, 0x0f); 
	nRF24L01_CE_H;
	delayMicroseconds(130);		// Delay more than 130us
}

void nRF24L01::TX_Mode(void)
{
	nRF24L01_CE_L;
  	SPI_RW_Reg(WRITE_REG + CONFIG, 0x0e);
	nRF24L01_CE_H;
	delayMicroseconds(130);		// Delay more than 130us
}

uint8_t nRF24L01::RxPacket(int8_t* rx_buf)
{
    uint8_t revale=0;
	uint8_t * rxbuf = (uint8_t *)rx_buf;
	sta=SPI_Read(STATUS);	// 读取状态寄存其来判断数据接收状况
	if(sta & RX_DR)				// 判断是否接收到数据
	{
		nRF24L01_CE_L;
		SPI_Read_Buf(RD_RX_PLOAD,rxbuf,TX_PLOAD_WIDTH);
		revale =1;			//读取数据完成标志
	}
	SPI_RW_Reg(WRITE_REG+STATUS,sta);   //接收到数据后RX_DR,TX_DS,MAX_PT都置高为1，通过写1来清楚中断标志
	return revale;
}

void nRF24L01::TxPacket(int8_t * tx_buf)
{
	uint8_t *txbuf = (uint8_t *)tx_buf;
	nRF24L01_CE_L; 			//StandBy I模式	
	SPI_Write_Buf(WR_TX_PLOAD, txbuf, TX_PLOAD_WIDTH);  // 装载数据
	nRF24L01_CE_H;
	delayMicroseconds(10);			// Delay 10us
}
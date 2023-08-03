#ifndef __NRF24L01_H
#define __NRF24L01_H
#include <stdint.h>
 
 typedef struct {
	void (*init)(void);
	void (*deinit)(void);
	void (*writeReg)(uint8_t reg, uint8_t value);
	uint8_t (*readReg)(uint8_t reg);
	uint8_t (*readBuf)(uint8_t reg, uint8_t *pBuf, uint8_t len);
	uint8_t (*writeBuf)(uint8_t reg, uint8_t *pBuf, uint8_t len);
	uint8_t (*dataReady)(void);
	void (*ceEnable)(uint8_t enable);
}NRF24L01_ops_t;

typedef struct {
	NRF24L01_ops_t *ops;
	uint8_t address[TX_ADR_WIDTH];
}NRF24L01_t;


 /**
  * @brief initialize nrf24l01
  * @param nrf24l01 pointer to nrf24l01 handle
  * @param ops pointer to nrf24l01 ops
  * @return 0 if successed, otherwise failed
  */
int NRF24L01_Init(NRF24L01_t *nrf24l01, NRF24L01_ops_t *ops) ;

/**
 * @brief check if nrf24l01 is connected
 * @param nrf24l01 
 * @return 
 */
int8_t NRF24L01_Check(NRF24L01_t *nrf24l01);


/**
 * @brief send packet to nrf24l01
 * @param nrf24l01 handle
 * @param txbuf pointer to tx buffer
 * @param txLength pointer to tx length
 * @return 0 if failed, otherwise return tx length
 */
uint8_t NRF24L01_TxPacket(NRF24L01_t *nrf24l01, uint8_t *txbuf, size_t txLength);

/**
 * @brief receive packet from nrf24l01
 * @param nrf24l01 pointer to nrf24l01 handle
 * @param rxbuf pointer to rx buffer
 * @param rxLength pointer to rx length
 * @return 0 if failed, otherwise return rx length
 */
uint8_t NRF24L01_RxPacket(NRF24L01_t *nrf24l01, uint8_t *rxbuf, size_t rxLength);

/**
 * @brief set nrf24l01 to rx mode
 * @param nrf24l01 pointer to nrf24l01 handle
 */
void NRF24L01_RX_Mode(NRF24L01_t *nrf24l01);

 
/**
 * @brief set nrf24l01 to rx mode
 * @param nrf24l01 pointer to nrf24l01 handle
 */
void NRF24L01_RX_Mode(NRF24L01_t *nrf24l01);

 
/**
 * @brief set nrf24l01 to tx mode
 * @param nrf24l01 pointer to nrf24l01 handle
 */
void NRF24L01_TX_Mode(NRF24L01_t *nrf24l01);

/**
 * @brief set the lower power of NRF24L01
 * @param nrf24l01 
 */
void NRF24L01_LowPower_Mode(NRF24L01_t *nrf24l01);
#endif











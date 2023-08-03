#include "nRF24L01.h"
#include "stdio.h"


// NRF24L01发送接收数据宽度定义
#define TX_ADR_WIDTH                                  5   	//5字节的地址宽度
#define RX_ADR_WIDTH                                  5   	//5字节的地址宽度
#define TX_PLOAD_WIDTH                                32  	//32字节的用户数据宽度
#define RX_PLOAD_WIDTH                                32  	//32字节的用户数据宽度
 
//NRF24L01寄存器操作命令
#define NRF_READ_REG                                  0x00  //读配置寄存器,低5位为寄存器地址
#define NRF_WRITE_REG                                 0x20  //写配置寄存器,低5位为寄存器地址
#define RD_RX_PLOAD                                   0x61  //读RX有效数据,1~32字节
#define WR_TX_PLOAD                                   0xA0  //写TX有效数据,1~32字节
#define FLUSH_TX                                      0xE1  //清除TX FIFO寄存器.发射模式下用
#define FLUSH_RX                                      0xE2  //清除RX FIFO寄存器.接收模式下用
#define REUSE_TX_PL                                   0xE3  //重新使用上一包数据,CE为高,数据包被不断发送.
#define NOP                                           0xFF  //空操作,可以用来读状态寄存器	 
//SPI(NRF24L01)寄存器地址
#define CONFIG                                        0x00  //配置寄存器地址;bit0:1接收模式,0发射模式;bit1:电选择;bit2:CRC模式;bit3:CRC使能;
                                                            //bit4:中断MAX_RT(达到最大重发次数中断)使能;bit5:中断TX_DS使能;bit6:中断RX_DR使能
#define EN_AA                                         0x01  //使能自动应答功能  bit0~5,对应通道0~5
#define EN_RXADDR                                     0x02  //接收地址允许,bit0~5,对应通道0~5
#define SETUP_AW                                      0x03  //设置地址宽度(所有数据通道):bit1,0:00,3字节;01,4字节;02,5字节;
#define SETUP_RETR                                    0x04  //建立自动重发;bit3:0,自动重发计数器;bit7:4,自动重发延时 250*x+86us
#define RF_CH                                         0x05  //RF通道,bit6:0,工作通道频率;
#define RF_SETUP                                      0x06  //RF寄存器;bit3:传输速率(0:1Mbps,1:2Mbps);bit2:1,发射功率;bit0:低噪声放大器增益
#define STATUS                                        0x07  //状态寄存器;bit0:TX FIFO满标志;bit3:1,接收数据通道号(最大:6);bit4,达到最多次重发
                                                            //bit5:数据发送完成中断;bit6:接收数据中断;
#define MAX_TX  		                                  0x10  //达到最大发送次数中断
#define TX_OK   		                                  0x20  //TX发送完成中断
#define RX_OK   		                                  0x40  //接收到数据中断
 
#define OBSERVE_TX                                    0x08  //发送检测寄存器,bit7:4,数据包丢失计数器;bit3:0,重发计数器
#define CD                                            0x09  //载波检测寄存器,bit0,载波检测;
#define RX_ADDR_P0                                    0x0A  //数据通道0接收地址,最大长度5个字节,低字节在前
#define RX_ADDR_P1                                    0x0B  //数据通道1接收地址,最大长度5个字节,低字节在前
#define RX_ADDR_P2                                    0x0C  //数据通道2接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define RX_ADDR_P3                                    0x0D  //数据通道3接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define RX_ADDR_P4                                    0x0E  //数据通道4接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define RX_ADDR_P5                                    0x0F  //数据通道5接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define TX_ADDR                                       0x10  //发送地址(低字节在前),ShockBurstTM模式下,RX_ADDR_P0与此地址相等
#define RX_PW_P0                                      0x11  //接收数据通道0有效数据宽度(1~32字节),设置为0则非法
#define RX_PW_P1                                      0x12  //接收数据通道1有效数据宽度(1~32字节),设置为0则非法
#define RX_PW_P2                                      0x13  //接收数据通道2有效数据宽度(1~32字节),设置为0则非法
#define RX_PW_P3                                      0x14  //接收数据通道3有效数据宽度(1~32字节),设置为0则非法
#define RX_PW_P4                                      0x15  //接收数据通道4有效数据宽度(1~32字节),设置为0则非法
#define RX_PW_P5                                      0x16  //接收数据通道5有效数据宽度(1~32字节),设置为0则非法
#define NRF_FIFO_STATUS                               0x17  //FIFO状态寄存器;bit0,RX FIFO寄存器空标志;bit1,RX FIFO满标志;bit2,3,保留
                                                            //bit4,TX FIFO空标志;bit5,TX FIFO满标志;bit6,1,循环发送上一数据包.0,不循环;


 int NRF24L01_Init(NRF24L01_t *nrf24l01, NRF24L01_ops_t *ops) {
	 nrf24l01->ops = ops;
	 nrf24l01->ops.init();
	 
	uint8_t defaultAddress[TX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01};
	memcpy(nrf24l01->address, defaultAddress, TX_ADR_WIDTH);

	nrf24l01->ops->ceEnable(1);

	return NRF24L01_Check(nrf24l01);
 }


int8_t NRF24L01_Check(NRF24L01_t *nrf24l01){
	uint8_t buf[TX_ADR_WIDTH]={0};
	
	NRF24L01_ops_t *ops = nrf24l01->ops;
	ops->ceEnable(1);
	ops->writeBuf(NRF_WRITE_REG+TX_ADDR, nrf24l01->address, TX_ADR_WIDTH); //写入5个字节的地址.
	ops->readBuf(TX_ADDR, buf, 5); //读出写入的地址
	if(memcmp(buf, nrf24l01->address, TX_ADR_WIDTH) != 0) {
		return -1;
	}

	return 0;		 	//检测到24L01
}	
 

uint8_t NRF24L01_TxPacket(NRF24L01_t *nrf24l01, uint8_t *txbuf, size_t txLength){
	if (txLength > TX_PLOAD_WIDTH) {
		return 0;
	}

	NRF24L01_ops_t *ops = nrf24l01->ops;
	ops->ceEnable(0);
	ops->writeBuf(WR_TX_PLOAD, txbuf, txLength);//写数据到TX BUF  32个字节
	ops->ceEnable(1);//启动发送

	uint32_t now = HAL_millis();
	while(ops->dataReady() != 0){
		if (HAL_elapesdMillis(now) > 1000) {
			return 0;
		}

		HAL_msleep(1);
	}

	uint8_t sta = ops->readReg(STATUS);  //读取状态寄存器的值
	ops->writeReg(NRF_WRITE_REG+STATUS, sta); //清除TX_DS或MAX_RT中断标志
	if(sta&MAX_TX)//达到最大重发次数
	{
		ops->writeReg(FLUSH_TX,0xff);//清除TX FIFO寄存器 
		return 0; 
	}

	if(sta&TX_OK)//发送完成
	{
		return txLength;
	}

	return 0;
}
 

uint8_t NRF24L01_RxPacket(NRF24L01_t *nrf24l01, uint8_t *rxbuf, size_t rxLength){
	if (rxLength > RX_PLOAD_WIDTH) {
		return 0;
	}

	NRF24L01_ops_t *ops = nrf24l01->ops;

	uint8_t sta = ops->readReg(STATUS);  //读取状态寄存器的值
	ops->writeReg(NRF_WRITE_REG+STATUS, sta); //清除TX_DS或MAX_RT中断标志
	if (sta&RX_OK){
		ops->readBuf(RD_RX_PLOAD, rxbuf, rxLength);//读取数据
		ops->writeReg(FLUSH_RX,0xff);//清除RX FIFO寄存器 
		return rxLength; 
	}


	return 0;
}			
 
void NRF24L01_RX_Mode(NRF24L01_t *nrf24l01){
	NRF24L01_ops_t *ops = nrf24l01->ops;

	ops->ceEnable(0);

	ops->writeReg(NRF_WRITE_REG+CONFIG, 0x0F);  //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC 
	ops->writeReg(NRF_WRITE_REG+EN_AA,0x01);    //使能通道0的自动应答
	ops->writeReg(NRF_WRITE_REG+EN_RXADDR,0x01);//使能通道0的接收地址
	ops->writeReg(NRF_WRITE_REG+RF_CH,40);	     //设置RF通信频率
	ops->writeReg(NRF_WRITE_REG+RF_SETUP,0x0f); //设置TX发射参数,0db增益,2Mbps,低噪声增益开启

	ops->writeReg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//选择通道0的有效数据宽度

	ops->writeBuf(NRF_WRITE_REG+RX_ADDR_P0, nrf24l01->address, RX_ADR_WIDTH);//写RX节点地址

	ops->ceEnable(1); //CE为高,进入接收模式

	HAL_msleep(1);
}	
 
void NRF24L01_TX_Mode(NRF24L01_t *nrf24l01){				
	NRF24L01_ops_t *ops = nrf24l01->ops;

	ops->ceEnable(0);
	
	ops->writeBuf(NRF_WRITE_REG+TX_ADDR, nrf24l01->address, TX_ADR_WIDTH);    //写TX节点地址
	ops->writeBuf(NRF_WRITE_REG+RX_ADDR_P0, nrf24l01->address, RX_ADR_WIDTH); //设置TX节点地址,主要为了使能ACK

	ops->writeReg(NRF_WRITE_REG+EN_AA,0x01);     //使能通道0的自动应答
	ops->writeReg(NRF_WRITE_REG+EN_RXADDR,0x01); //使能通道0的接收地址
	ops->writeReg(NRF_WRITE_REG+SETUP_RETR,0x15);//设置自动重发间隔时间:500us + 86us;最大自动重发次数:5次
	ops->writeReg(NRF_WRITE_REG+RF_CH,40);       //设置RF通道为40
	ops->writeReg(NRF_WRITE_REG+RF_SETUP,0x0f);  //设置TX发射参数,0db增益,2Mbps,低噪声增益开启

	ops->writeReg(NRF_WRITE_REG+CONFIG,0x0e);    //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式,开启所有中断
	ops->ceEnable(1);//CE为高,10us后启动发送

	hal_msleep(1);
}
 
/**
 * @brief set the lower power of NRF24L01
 * @param nrf24l01 
 */
void NRF24L01_LowPower_Mode(NRF24L01_t *nrf24l01)
{
	NRF24L01_ops_t *ops = nrf24l01->ops;

	ops->ceEnable(0);
	ops->writeReg(NRF_WRITE_REG+CONFIG, 0x00);		//配置工作模式:掉电模式
}

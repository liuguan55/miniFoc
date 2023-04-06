//
// Created by 邓志君 on 2023/2/13.
//

#ifndef MINIFOC_F4_DEVICE_NORFLASH_W25QXX_H_
#define MINIFOC_F4_DEVICE_NORFLASH_W25QXX_H_


#define W25Q16
//#define W25Q32
//#define W25Q64
//#define W25Q128
//#define W25Q256
//#define W25Q512

extern SPI_HandleTypeDef hspi2;
#define W25Qxx_SPI_Handle hspi2
#define W25Qxx_CS_GPIOx GPIOC
#define W25Qxx_CS_PIN GPIO_PIN_5


#define W25Qxx_CMD_ID (0x90)             //读ID
#define W25Qxx_CMD_JID (0x9f)             //读jID
#define W25Qxx_CMD_Placeholder (0xff)             //读ID
#define W25Qxx_CMD_Write_Enable (0x06)   //写功能打开
#define W25Qxx_CMD_Write_Disable (0x04)  //写功能关闭
#define W25Qxx_CMD_ReadStatusReg1 (0x05) //读取状态寄存器
#define W25Qxx_CMD_Sector_Erase (0x20) //读取状态寄存器
#define W25Qxx_CMD_Read_Data (0x03)      //读取数据
#define W25Qxx_CMD_Write_Page (0x02)     //写页

int32_t W25Qxx_init(void);
uint32_t W25Qxx_Read_ID(void);
void W25Qxx_Write_Protect(uint8_t Functional);
uint8_t W25Qxx_Read_StatusReg1(void);
void W25Qxx_Wait_Free(void);
void W25Qxx_Sector_Erase(uint32_t Address);
void W25Qxx_Read_Data(uint32_t Address, uint8_t *Buf, uint32_t Len);
void W25Qxx_Write_Page(uint32_t Address, uint8_t *Buf, uint32_t Len);
void W25Qxx_Write_Sector(uint32_t Address, uint8_t *Buf, uint32_t Len);
void W25Qxx_Write(uint32_t Address, uint8_t *Buf, uint32_t Len);


#endif //MINIFOC_F4_DEVICE_NORFLASH_W25QXX_H_

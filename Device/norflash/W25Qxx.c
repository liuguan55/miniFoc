//
// Created by 邓志君 on 2023/2/13.
//
#include <stdio.h>
#include "spi.h"
#include "MiniCommon.h"
#include "W25Qxx.h"

#define LOG_TAG "W25QXX"

#if defined(W25Q16)
uint8_t W25Qxx_Address_Len = 24;
#elif defined(W25Q32)
uint8_t W25Qxx_Address_Len = 24;
#elif defined(W25Q64)
uint8_t W25Qxx_Address_Len = 24;
#elif defined(W25Q128)
uint8_t W25Qxx_Address_Len = 24;
#elif defined(W25Q256)
uint8_t W25Qxx_Address_Len = 32;
#elif defined(W25Q512)
uint8_t W25Qxx_Address_Len = 32;
#else
#error "prese define: W25Q16 or W25Q32 or W25Q64 or W25Q128 or W25Q256 or W25Q512"
#endif

/**
 * @brief 设置片选(CS)为低电平选中
 * @param 无
 * @retval 无
 * @author:HZ12138
 * @date: 2022-07-03 20:49:18
 */
static inline void W25Qxx_CS_Low(void)
{
  HAL_GPIO_WritePin(W25Qxx_CS_GPIOx, W25Qxx_CS_PIN, GPIO_PIN_RESET);
}
/**
 * @brief 设置片选(CS)为高电平未选中
 * @param 无
 * @retval 无
 * @author:HZ12138
 * @date: 2022-07-03 20:49:18
 */
static inline void W25Qxx_CS_Hight(void)
{
  HAL_GPIO_WritePin(W25Qxx_CS_GPIOx, W25Qxx_CS_PIN, GPIO_PIN_SET);
}

/**
 * @brief 通过SPI发送接收数据（阻塞）（SPI是移位发送，接收时要发送数据，发送时也会收到数据）
 * @param TxData:发送的数据
 * @retval 接收的数据
 * @author:HZ12138
 * @date: 2022-07-03 20:49:18
 */
static uint8_t W25Qxx_SPI_RW_Byte(uint8_t TxData)
{
  uint8_t Rxdata;
  HAL_SPI_TransmitReceive(&W25Qxx_SPI_Handle, &TxData, &Rxdata, 1, 1000);
  return Rxdata;
}

/**
 * @brief 读取芯片的ID
 * @param 无
 * @retval 芯片的ID
 * @author:HZ12138
 * @date: 2022-07-03 20:49:18
 */
uint32_t W25Qxx_Read_ID(void)
{
  uint8_t id[4];
  W25Qxx_CS_Low();
  W25Qxx_SPI_RW_Byte(W25Qxx_CMD_JID);                //发送命令
//  W25Qxx_SPI_RW_Byte(W25Qxx_CMD_Placeholder);       //占位符
//  W25Qxx_SPI_RW_Byte(W25Qxx_CMD_Placeholder);       //占位符
//  W25Qxx_SPI_RW_Byte(0x00);                         //必须为0

  for (int i = 0 ; i < sizeof(id); i++){
    id[i] =  W25Qxx_SPI_RW_Byte(W25Qxx_CMD_Placeholder);
  }

  W25Qxx_CS_Hight();


  printf("id %02x-%02x-%02x-%02x\n",id[0],id[1],id[2],id[3]);

  return ((id[0] << 24) | id[1] << 16 | id[2] << 8 | id[3]);
}


int32_t W25Qxx_init(void)
{
  W25Qxx_Read_ID();
}

/**
 * @brief 写保护
 * @param Functional:
 * @arg 1:     允许写入
 * @arg 0:     不允许写入
 * @retval 无
 * @author:HZ12138
 * @date: 2022-07-03 20:49:18
 */
void W25Qxx_Write_Protect(uint8_t Functional)
{
  W25Qxx_CS_Low();
  if (Functional == 0)
	W25Qxx_SPI_RW_Byte(W25Qxx_CMD_Write_Disable);//不允许写入
  else if (Functional == 1)
	W25Qxx_SPI_RW_Byte(W25Qxx_CMD_Write_Enable);//允许写入
  W25Qxx_CS_Hight();
}

/**
 * @brief 读取寄存器1的状态
 * @param 无
 * @retval 状态
 * @author:HZ12138
 * @date: 2022-07-03 20:49:18
 */
uint8_t W25Qxx_Read_StatusReg1(void)
{
  uint8_t zj = 0;
  W25Qxx_CS_Low();
  W25Qxx_SPI_RW_Byte(W25Qxx_CMD_ReadStatusReg1);   //发送命令
  zj = W25Qxx_SPI_RW_Byte(W25Qxx_CMD_Placeholder); //接收数据
  W25Qxx_CS_Hight();
  return zj;
}

/**
 * @brief 等待写入/擦除完成
 * @param 无
 * @retval 无
 * @author:HZ12138
 * @date: 2022-07-03 20:49:18
 */
void W25Qxx_Wait_Free(void)
{
  while (W25Qxx_Read_StatusReg1() & 0x01) //等待写完
  {
    MiniCommon_delayMs(1);
  }
}

/**
 * @brief 扇区擦除
 * @param Address:要擦除的扇区内的任意地址
 * @retval 无
 * @author:HZ12138
 * @date: 2022-07-03 20:49:18
 */
void W25Qxx_Sector_Erase(uint32_t Address)
{
  Address &= 0xFFFFF000;
  W25Qxx_Write_Protect(1); //允许写入

  W25Qxx_CS_Low();
  W25Qxx_SPI_RW_Byte(W25Qxx_CMD_Sector_Erase); //命令

  if (W25Qxx_Address_Len == 32)
	W25Qxx_SPI_RW_Byte((Address & 0xFF000000) >> 24); //如果是32位的地址则发送

  W25Qxx_SPI_RW_Byte((Address & 0x00FF0000) >> 16); //地址
  W25Qxx_SPI_RW_Byte((Address & 0x0000FF00) >> 8);  //地址
  W25Qxx_SPI_RW_Byte((Address & 0x000000FF) >> 0);  //地址
  W25Qxx_CS_Hight();

  W25Qxx_Write_Protect(0); //关闭写入

  W25Qxx_Wait_Free();
}

/**
 * @brief 读取数据
 * @param Address:要读取的地址
 * @param Buf:将数据放入的数组地址
 * @param Len:读取的字节数
 * @retval 无
 * @author:HZ12138
 * @date: 2022-07-03 20:49:18
 */
void W25Qxx_Read_Data(uint32_t Address, uint8_t *Buf, uint32_t Len)
{
  W25Qxx_CS_Low();
  W25Qxx_SPI_RW_Byte(W25Qxx_CMD_Read_Data); //命令

  if (W25Qxx_Address_Len == 32)
	W25Qxx_SPI_RW_Byte((Address & 0xFF000000) >> 24); //如果是32位的地址则发送

  W25Qxx_SPI_RW_Byte((Address & 0x00FF0000) >> 16); //地址
  W25Qxx_SPI_RW_Byte((Address & 0x0000FF00) >> 8);  //地址
  W25Qxx_SPI_RW_Byte((Address & 0x000000FF) >> 0);  //地址
  for (int i = 0; i < Len; i++)
	Buf[i] = W25Qxx_SPI_RW_Byte(W25Qxx_CMD_Placeholder);
  W25Qxx_CS_Hight();
}

/**
 * @brief 按页写入数据
 * @param Address:要写入的地址的首地址（必须首地址）
 * @param Buf:将数据的数组地址
 * @param Len:写入的字节数
 * @retval 无
 * @author:HZ12138
 * @date: 2022-07-03 20:49:18
 */
void W25Qxx_Write_Page(uint32_t Address, uint8_t *Buf, uint32_t Len)
{
  W25Qxx_Write_Protect(1); //允许写入
  W25Qxx_Wait_Free();
  W25Qxx_CS_Low();
  W25Qxx_SPI_RW_Byte(W25Qxx_CMD_Write_Page); //命令

  if (W25Qxx_Address_Len == 32)
	W25Qxx_SPI_RW_Byte((Address & 0xFF000000) >> 24); //如果是32位的地址则发送

  W25Qxx_SPI_RW_Byte((Address & 0x00FF0000) >> 16); //地址
  W25Qxx_SPI_RW_Byte((Address & 0x0000FF00) >> 8);  //地址
  W25Qxx_SPI_RW_Byte((Address & 0x00000000) >> 0);  //地址(确保是首地址)
  for (int i = 0; i < 256; i++)
  {
	if (i < Len)
	  W25Qxx_SPI_RW_Byte(Buf[i]);
	else
	  W25Qxx_SPI_RW_Byte(0xFF);
  }
  W25Qxx_CS_Hight();
  W25Qxx_Write_Protect(0); //关闭写入
}


//uint8_t W25Qxx_Buf[16][256] = {0};
///**
// * @brief 打印出整个扇区的数据
// * @param Address:扇区内的任意地址
// * @return 无
// * @author HZ12138
// * @date 2022-07-04 10:13:03
// */
//void W25Qxx_Print_Sector(uint32_t Address)
//{
//  for (uint32_t i = 0; i < 16; i++)
//	W25Qxx_Read_Data((Address & 0xFFFFF000) | (i << 8), W25Qxx_Buf[i], 256);
//
//  for (int i = 0; i < 16; i++)
//  {
//	printf("%06X:", (Address & 0xFFFFF000) | (i << 8));
//	for (int j = 0; j < 256; j++)
//	{
//	  printf("%02X ", W25Qxx_Buf[i][j]);
//	}
//	printf("\r\n");
//  }
//}
//uint8_t W25Qxx_Buf[16][256] = {0};
///**
// * @brief 向扇区写入数据
// * @param Address:要写入的地址
// * @param Buf:写入数据的数组地址
// * @param Len:长度
// * @return 无
// * @author:HZ12138
// * @date: 2022-07-03 20:49:18
// */
//void W25Qxx_Write_Sector(uint32_t Address, uint8_t *Buf, uint32_t Len)
//{
//  uint32_t Add_Bef = Address;
//  uint32_t Add_Aft = Address + Len;
//  uint32_t Num = 0;
//
//  for (uint32_t i = 0; i < 16; i++) //读取原来数据到缓冲区
//	W25Qxx_Read_Data((Add_Bef & 0xFFFFF000) | (i << 8), W25Qxx_Buf[i], 256);
//
//  for (uint32_t i = Add_Bef; i < Add_Aft; i++) //向缓冲区写入数据
//  {
//	W25Qxx_Buf[(i & 0x00000F00) >> 8][i & 0x000000FF] = Buf[Num];
//	Num++;
//  }
//
//  W25Qxx_Sector_Erase(Add_Bef); //清空这个扇区
//  W25Qxx_Wait_Free();
//
//  for (uint32_t i = 0; i < 16; i++) //向FLASH写入缓冲区内的数据
//  {
//	W25Qxx_Write_Page((Add_Bef & 0xFFFFF000) | (i << 8), W25Qxx_Buf[i], 256);
//	W25Qxx_Wait_Free();
//  }
//}

/**
 * @brief 写入数据
 * @param Address:要写入的地址
 * @param Buf:写入数据的数组地址
 * @param Len:长度
 * @return 无
 * @date 2022-07-04 21:50:38
 */
void W25Qxx_Write(uint32_t Address, uint8_t *Buf, uint32_t Len)
{
  uint32_t Add_Bef = Address;
  uint32_t Add_Aft = Address + Len;
  uint32_t Num = 0;
  W25Qxx_Write_Sector(Add_Bef, Buf, ((Add_Bef & 0xFFFFF000) + 0x00001000 - Add_Bef));
  Num = ((Add_Bef & 0xFFFFF000) + 0x00001000 - Add_Bef);
  for (uint32_t i = (Add_Bef & 0xFFFFF000) + 0x00001000;
	   i < (Add_Aft & 0xFFFFF000); i += 0x00001000)
  {
	W25Qxx_Write_Sector(i, &Buf[Num], 4096);
	Num += 4096;
  }
  W25Qxx_Write_Sector((Add_Aft & 0xFFFFF000), &Buf[Num], Add_Aft - (Add_Aft & 0xFFFFF000));
}


















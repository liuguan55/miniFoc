#include "nRF24L01.h"
#include "spi.h"
#include "stdio.h"

//#include "nrf24L01.h"
//#include "spi.h"
// 
const uint8_t TX_ADDRESS[TX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //���͵�ַ
const uint8_t RX_ADDRESS[RX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //���յ�ַ
 
 
//���NRF24L01�޸�SPI1����
void NRF24L01_SPI_Init(void)
{
    __HAL_SPI_DISABLE(&hspi1);               //�ȹر�SPI1
    hspi1.Init.CLKPolarity=SPI_POLARITY_LOW; //����ͬ��ʱ�ӵĿ���״̬Ϊ�͵�ƽ
    hspi1.Init.CLKPhase=SPI_PHASE_1EDGE;     //����ͬ��ʱ�ӵĵ�1�������أ��������½������ݱ�����
    HAL_SPI_Init(&hspi1);
    __HAL_SPI_ENABLE(&hspi1);                //ʹ��SPI1
}
 
//��ʼ��24L01��IO��
void NRF24L01_Init(void)
{		              			//��ʼ��SPI1
		NRF24L01_SPI_Init();                			//���NRF���ص��޸�SPI������
		NRF24L01_CE_LOW(); 			            			//ʹ��24L01
		NRF24L01_SPI_CS_DISABLE();			    			//SPIƬѡȡ��	 		 	 
}
 
/**
	*SPI�ٶ����ú���
  *SPI�ٶ�=fAPB1/��Ƶϵ��
  *@ref SPI_BaudRate_Prescaler:SPI_BAUDRATEPRESCALER_2~SPI_BAUDRATEPRESCALER_2 256
	*fAPB1ʱ��һ��Ϊ42Mhz��
	*/
static void SPI1_SetSpeed(uint8_t SPI_BaudRatePrescaler)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//�ж���Ч��
    __HAL_SPI_DISABLE(&hspi1);            //�ر�SPI
    hspi1.Instance->CR1&=0XFFC7;          //λ3-5���㣬�������ò�����
    hspi1.Instance->CR1|=SPI_BaudRatePrescaler;//����SPI�ٶ�
    __HAL_SPI_ENABLE(&hspi1);             //ʹ��SPI
}
 
/**
  * ��������: ������Flash��ȡд��һ���ֽ����ݲ�����һ���ֽ�����
  * �������: byte������������
  * �� �� ֵ: uint8_t�����յ�������
  * ˵    ������
  */
uint8_t SPIx_ReadWriteByte(SPI_HandleTypeDef* hspi,uint8_t byte)
{
  uint8_t d_read,d_send=byte;
  if(HAL_SPI_TransmitReceive(hspi,&d_send,&d_read,1,0xFF)!=HAL_OK)
  {
    d_read=0xFF;
  }
  return d_read; 
}
 
/**
  * ��������: ���24L01�Ƿ����
  * �������: ��
  * �� �� ֵ: 0���ɹ�;1��ʧ��
  * ˵    ������          
  */ 
uint8_t NRF24L01_Check(void)
{
	uint8_t buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
	uint8_t i;
  
	SPI1_SetSpeed(SPI_BAUDRATEPRESCALER_4); //spi�ٶ�Ϊ8.0Mhz����24L01�����SPIʱ��Ϊ10Mhz,�����һ��û��ϵ��  
	NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,buf,5);//д��5���ֽڵĵ�ַ.	
	NRF24L01_Read_Buf(TX_ADDR,buf,5); //����д��ĵ�ַ  
	for(i=0;i<5;i++)if(buf[i]!=0XA5)break;	 							   
	if(i!=5)return 1;//���24L01����	
	return 0;		 	//��⵽24L01
}	
 
/**
  * ��������: SPIд�Ĵ���
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ����reg:ָ���Ĵ�����ַ
  *           
  */ 
uint8_t NRF24L01_Write_Reg(uint8_t reg,uint8_t value)
{
	uint8_t status;	
  NRF24L01_SPI_CS_ENABLE();                 //ʹ��SPI����
  status =SPIx_ReadWriteByte(&hspi1,reg);   //���ͼĴ����� 
  SPIx_ReadWriteByte(&hspi1,value);         //д��Ĵ�����ֵ
  NRF24L01_SPI_CS_DISABLE();                //��ֹSPI����	   
  return(status);       			//����״ֵ̬
}
 
/**
  * ��������: ��ȡSPI�Ĵ���ֵ
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ����reg:Ҫ���ļĴ���
  *           
  */ 
uint8_t NRF24L01_Read_Reg(uint8_t reg)
{
	uint8_t reg_val;	    
 	NRF24L01_SPI_CS_ENABLE();          //ʹ��SPI����		
  SPIx_ReadWriteByte(&hspi1,reg);   //���ͼĴ�����
  reg_val=SPIx_ReadWriteByte(&hspi1,0XFF);//��ȡ�Ĵ�������
  NRF24L01_SPI_CS_DISABLE();          //��ֹSPI����		    
  return(reg_val);           //����״ֵ̬
}		
 
/**
  * ��������: ��ָ��λ�ö���ָ�����ȵ�����
  * �������: ��
  * �� �� ֵ: �˴ζ�����״̬�Ĵ���ֵ 
  * ˵    ������
  *           
  */ 
uint8_t NRF24L01_Read_Buf(uint8_t reg,uint8_t *pBuf,uint8_t len)
{
	uint8_t status,uint8_t_ctr;	   
  
  NRF24L01_SPI_CS_ENABLE();           //ʹ��SPI����
  status=SPIx_ReadWriteByte(&hspi1,reg);//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬   	   
 	for(uint8_t_ctr=0;uint8_t_ctr<len;uint8_t_ctr++)
  {
    pBuf[uint8_t_ctr]=SPIx_ReadWriteByte(&hspi1,0XFF);//��������
  }
  NRF24L01_SPI_CS_DISABLE();       //�ر�SPI����
  return status;        //���ض�����״ֵ̬
}
 
/**
  * ��������: ��ָ��λ��дָ�����ȵ�����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ����reg:�Ĵ���(λ��)  *pBuf:����ָ��  len:���ݳ���
  *           
  */ 
uint8_t NRF24L01_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t len)
{
	uint8_t status,uint8_t_ctr;	    
 	NRF24L01_SPI_CS_ENABLE();          //ʹ��SPI����
  status = SPIx_ReadWriteByte(&hspi1,reg);//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬
  for(uint8_t_ctr=0; uint8_t_ctr<len; uint8_t_ctr++)
  {
    SPIx_ReadWriteByte(&hspi1,*pBuf++); //д������	 
  }
  NRF24L01_SPI_CS_DISABLE();       //�ر�SPI����
  return status;          //���ض�����״ֵ̬
}		
 
/**
  * ��������: ����NRF24L01����һ������
  * �������: ��
  * �� �� ֵ: �������״��
  * ˵    ����txbuf:�����������׵�ַ
  *           
  */ 
uint8_t NRF24L01_TxPacket(uint8_t *txbuf)
{
	uint8_t sta;
	SPI1_SetSpeed(SPI_BAUDRATEPRESCALER_8); //spi�ٶ�Ϊ4.0Mhz��24L01�����SPIʱ��Ϊ10Mhz�� 
	NRF24L01_CE_LOW();
  NRF24L01_Write_Buf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);//д���ݵ�TX BUF  32���ֽ�
 	NRF24L01_CE_HIGH();//��������	 
  
	while(NRF24L01_IRQ_PIN_READ()!=0);//�ȴ��������
  
	sta=NRF24L01_Read_Reg(STATUS);  //��ȡ״̬�Ĵ�����ֵ	   
	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //���TX_DS��MAX_RT�жϱ�־
	if(sta&MAX_TX)//�ﵽ����ط�����
	{
		NRF24L01_Write_Reg(FLUSH_TX,0xff);//���TX FIFO�Ĵ��� 
		return MAX_TX; 
	}
	if(sta&TX_OK)//�������
	{
		return TX_OK;
	}
	return 0xff;//����ԭ����ʧ��
}
 
/**
  * ��������:����NRF24L01����һ������
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  *           
  */ 
uint8_t NRF24L01_RxPacket(uint8_t *rxbuf)
{
	uint8_t sta;		
  SPI1_SetSpeed(SPI_BAUDRATEPRESCALER_8); //spi�ٶ�Ϊ4.0Mhz��24L01�����SPIʱ��Ϊ10Mhz�� 
	sta=NRF24L01_Read_Reg(STATUS);  //��ȡ״̬�Ĵ�����ֵ    	 
	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //���TX_DS��MAX_RT�жϱ�־
	if(sta&RX_OK)//���յ�����
	{
		NRF24L01_Read_Buf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//��ȡ����
		NRF24L01_Write_Reg(FLUSH_RX,0xff);//���RX FIFO�Ĵ��� 
		return 0; 
	}	   
	return 1;//û�յ��κ�����
}			
 
/**
  * ��������: �ú�����ʼ��NRF24L01��RXģʽ
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  *           
  */ 
void NRF24L01_RX_Mode(void)
{
	NRF24L01_CE_LOW();	  
  NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG, 0x0F);//���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC 
  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x01);    //ʹ��ͨ��0���Զ�Ӧ��    
  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x01);//ʹ��ͨ��0�Ľ��յ�ַ  	 
  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,40);	     //����RFͨ��Ƶ��		  
  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f);//����TX�������,0db����,2Mbps,���������濪��   
  
  NRF24L01_Write_Reg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//ѡ��ͨ��0����Ч���ݿ��� 	    
    
  NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(uint8_t*)RX_ADDRESS,RX_ADR_WIDTH);//дRX�ڵ��ַ
	
  NRF24L01_CE_HIGH(); //CEΪ��,�������ģʽ 
  HAL_Delay(1);
}	
 
/**
  * ��������: �ú�����ʼ��NRF24L01��TXģʽ
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  *           
  */ 
void NRF24L01_TX_Mode(void)
{														 
	NRF24L01_CE_LOW();	    
  NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,(uint8_t*)TX_ADDRESS,TX_ADR_WIDTH);//дTX�ڵ��ַ 
  NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(uint8_t*)RX_ADDRESS,RX_ADR_WIDTH); //����TX�ڵ��ַ,��ҪΪ��ʹ��ACK	  
 
  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x01);     //ʹ��ͨ��0���Զ�Ӧ��    
  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x01); //ʹ��ͨ��0�Ľ��յ�ַ  
  NRF24L01_Write_Reg(NRF_WRITE_REG+SETUP_RETR,0x15);//�����Զ��ط����ʱ��:500us + 86us;����Զ��ط�����:5��
  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,40);       //����RFͨ��Ϊ40
  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f);  //����TX�������,0db����,2Mbps,���������濪��   
 
  NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG,0x0e);    //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ,���������ж�
	NRF24L01_CE_HIGH();//CEΪ��,10us����������
  HAL_Delay(1);
}
 
/**
  * ��������: �ú���NRF24L01����͹���ģʽ
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  *           
  */
void NRF_LowPower_Mode(void)
{
	NRF24L01_CE_LOW();	 
	NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG, 0x00);		//���ù���ģʽ:����ģʽ
}


//NRF24L01 ��������
#if 0
unsigned char idel_mode_flag = 0;
unsigned char mode_time_counter = 0;

const unsigned char INIT_ADDR0[5]= {0x02,0x3A,0xB1,0xB1,0x01};
const unsigned char INIT_ADDR1[5]= {0x02,0x3A,0x01,0x01,0x01};
const unsigned char INIT_ADDR2[5]= {0x03,0x3A,0x01,0x01,0x01};
const unsigned char INIT_ADDR3[5]= {0x04,0x3A,0x01,0x01,0x01};
const unsigned char INIT_ADDR4[5]= {0x05,0x3A,0x01,0x01,0x01};
const unsigned char INIT_ADDR5[5]= {0x06,0x3A,0x01,0x01,0x01};

#define CH_Num	120
#define debug_out(fmt,args...)  printf(fmt,##args)
//#define debug_out(fmt,args...) 

void delay_us(uint32_t n)
{
	unsigned char i;

	while(n--)
	{
		i = 8;
		while(i--);
	}
}
//��ʼ��24L01��IO��
void NRF24L01_Init(void)
{
	//spi init
	//gpio init
	Clr_NRF24L01_CE;    // chip enable
	Set_NRF24L01_CSN;   // Spi disable
	delay_us(100);
}

//��װspi��д����
unsigned char nRF24_SPI_Send_Byte(unsigned char txdata)
{
	unsigned char rxdata;
	HAL_SPI_TransmitReceive(&hspi1, &txdata, &rxdata, 1, 0x10);
	return(rxdata);							// return read unsigned char
}


//ͨ��SPIд�Ĵ���
unsigned char NRF24L01_Write_Reg(unsigned char regaddr,unsigned char data)
{
	unsigned char status;
	Clr_NRF24L01_CSN;                    //ʹ��SPI����
	status =nRF24_SPI_Send_Byte(regaddr); //���ͼĴ�����
	nRF24_SPI_Send_Byte(data);            //д��Ĵ�����ֵ
	Set_NRF24L01_CSN;                    //��ֹSPI����
	return(status);       		         //����״ֵ̬
}
//��ȡSPI�Ĵ���ֵ ��regaddr:Ҫ���ļĴ���
unsigned char NRF24L01_Read_Reg(unsigned char regaddr)
{
	unsigned char reg_val;
	Clr_NRF24L01_CSN;                //ʹ��SPI����
	nRF24_SPI_Send_Byte(regaddr);     //���ͼĴ�����
	reg_val=nRF24_SPI_Send_Byte(0XFF);//��ȡ�Ĵ�������
	Set_NRF24L01_CSN;                //��ֹSPI����
	return(reg_val);                 //����״ֵ̬
}
//��ָ��λ�ö���ָ�����ȵ�����
//*pBuf:����ָ��
//����ֵ,�˴ζ�����״̬�Ĵ���ֵ
unsigned char NRF24L01_Read_Buf(unsigned char regaddr,unsigned char *pBuf,unsigned char datalen)
{
	unsigned char status,u8_ctr;
	Clr_NRF24L01_CSN;                     //ʹ��SPI����
	status=nRF24_SPI_Send_Byte(regaddr);   //���ͼĴ���ֵ(λ��),����ȡ״ֵ̬
	//for(u8_ctr=0;u8_ctr<datalen;u8_ctr++)pBuf[u8_ctr]=SPI_ReadWriteByte(0XFF);//��������
	HAL_SPI_Receive(&hspi1, pBuf, datalen, 0x10);
	Set_NRF24L01_CSN;                     //�ر�SPI����
	return status;                        //���ض�����״ֵ̬
}
//��ָ��λ��дָ�����ȵ�����
//*pBuf:����ָ��
//����ֵ,�˴ζ�����״̬�Ĵ���ֵ
unsigned char NRF24L01_Write_Buf(unsigned char regaddr, unsigned char *pBuf, unsigned char datalen)
{
	unsigned char status,u8_ctr;
	Clr_NRF24L01_CSN;                                    //ʹ��SPI����
	status = nRF24_SPI_Send_Byte(regaddr);                //���ͼĴ���ֵ(λ��),����ȡ״ֵ̬
	//for(u8_ctr=0; u8_ctr<datalen; u8_ctr++)SPI_ReadWriteByte(*pBuf++); //д������
	HAL_SPI_Transmit(&hspi1, pBuf, datalen, 0x10);
	Set_NRF24L01_CSN;                                    //�ر�SPI����
	return status;                                       //���ض�����״ֵ̬
}
//����NRF24L01����һ������
//txbuf:�����������׵�ַ
//����ֵ:�������״��
unsigned char NRF24L01_TxPacket(unsigned char *txbuf)
{
	unsigned char state;
	Clr_NRF24L01_CE;
	NRF24L01_Write_Buf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);//д���ݵ�TX BUF  32���ֽ�
	Set_NRF24L01_CE;                                     //��������
	while(READ_NRF24L01_IRQ!=0);                         //�ȴ��������
	state=NRF24L01_Read_Reg(STATUS);                     //��ȡ״̬�Ĵ�����ֵ
	NRF24L01_Write_Reg(SPI_WRITE_REG+STATUS,state);      //���TX_DS��MAX_RT�жϱ�־
	if(state&MAX_TX)                                     //�ﵽ����ط�����
	{
		NRF24L01_Write_Reg(FLUSH_TX,0xff);               //���TX FIFO�Ĵ���
		debug_out("TX MAX_TX error!\r\n");
		return MAX_TX;
	}
	if(state&TX_OK)                                      //�������
	{
		debug_out("TX OK!\r\n");
		return TX_OK;
	}
	debug_out("TX other error!\r\n");
	return 0xff;                                         //����ԭ����ʧ��
}

//����NRF24L01����һ������
//txbuf:�����������׵�ַ
//����ֵ:0��������ɣ��������������
unsigned char NRF24L01_RxPacket(unsigned char *rxbuf)
{
	unsigned char state;
	state=NRF24L01_Read_Reg(STATUS);                //��ȡ״̬�Ĵ�����ֵ
	NRF24L01_Write_Reg(SPI_WRITE_REG+STATUS,state); //���TX_DS��MAX_RT�жϱ�־
	if(state&TX_OK)
	{
		debug_out("RX send ack!\r\n"); //�ɹ�����ACK
	}
	if(state&RX_OK)                                 //���յ�����
	{
		NRF24L01_Read_Buf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//��ȡ����
		NRF24L01_Write_Reg(FLUSH_RX,0xff);          //���RX FIFO�Ĵ���
	//	debug_out("RX read data!\r\n");
		return 0;
	}
	return 1;                                      //û�յ��κ�����
}

//�ú�����ʼ��NRF24L01��RXģʽ
//����RX��ַ,дRX���ݿ���,ѡ��RFƵ��,�����ʺ�LNA HCURR
//��CE��ߺ�,������RXģʽ,�����Խ���������
void RX_Mode(void)
{
	Clr_NRF24L01_CE;
	//дRX�ڵ��ַ
	NRF24L01_Write_Buf(SPI_WRITE_REG+RX_ADDR_P0,(unsigned char*)INIT_ADDR0,RX_ADR_WIDTH);

	//ʹ��ͨ��0���Զ�Ӧ��
	NRF24L01_Write_Reg(SPI_WRITE_REG+EN_AA,0x01);
	//ʹ��ͨ��0�Ľ��յ�ַ
	NRF24L01_Write_Reg(SPI_WRITE_REG+EN_RXADDR,0x01);
	//����RFͨ��Ƶ��
	NRF24L01_Write_Reg(SPI_WRITE_REG+RF_CH,CH_Num);
	//ѡ��ͨ��0����Ч���ݿ���
	NRF24L01_Write_Reg(SPI_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);
	//����TX�������,0db����,2Mbps,���������濪��
	NRF24L01_Write_Reg(SPI_WRITE_REG+RF_SETUP,0x0f);
	//���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,PRIM_RX����ģʽ
	NRF24L01_Write_Reg(SPI_WRITE_REG+CONFIG, 0x0f);
	//CEΪ��,�������ģʽ
	Set_NRF24L01_CE;
}

//�ú�����ʼ��NRF24L01��TXģʽ
//����TX��ַ,дTX���ݿ���,����RX�Զ�Ӧ��ĵ�ַ,���TX��������,
//ѡ��RFƵ��,�����ʺ�LNA HCURR PWR_UP,CRCʹ��
//��CE��ߺ�,������RXģʽ,�����Խ���������
//CEΪ�ߴ���10us,����������.
void TX_Mode(void)
{
	Clr_NRF24L01_CE;
	//дTX�ڵ��ַ
	NRF24L01_Write_Buf(SPI_WRITE_REG+CONFIG,(unsigned char*)INIT_ADDR0,TX_ADR_WIDTH);
	//����TX�ڵ��ַ,��ҪΪ��ʹ��ACK
	NRF24L01_Write_Buf(SPI_WRITE_REG+RX_ADDR_P0,(unsigned char*)INIT_ADDR0,RX_ADR_WIDTH);

	//ʹ��ͨ��0���Զ�Ӧ��
	NRF24L01_Write_Reg(SPI_WRITE_REG+EN_AA,0x01);
	//ʹ��ͨ��0�Ľ��յ�ַ
	NRF24L01_Write_Reg(SPI_WRITE_REG+EN_RXADDR,0x01);
	//�����Զ��ط����ʱ��:500us + 86us;����Զ��ط�����:10��
	NRF24L01_Write_Reg(SPI_WRITE_REG+SETUP_RETR,0x1a);
	//����RFͨ��Ϊ40
	NRF24L01_Write_Reg(SPI_WRITE_REG+RF_CH,CH_Num);
	//����TX�������,0db����,2Mbps,���������濪��
	NRF24L01_Write_Reg(SPI_WRITE_REG+RF_SETUP,0x0f);
	//���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,PRIM_RX����ģʽ,���������ж�
	NRF24L01_Write_Reg(SPI_WRITE_REG+CONFIG,0x0e);
	// CEΪ��,10us����������
	Set_NRF24L01_CE;
}

//�ϵ���NRF24L01�Ƿ���λ
//д5������Ȼ���ٶ��������бȽϣ�
//��ͬʱ����ֵ:0����ʾ��λ;���򷵻�1����ʾ����λ

unsigned char NRF24L01_Check(void)
{
	unsigned char buf[5]= {0XA5,0XA5,0XA5,0XA5,0XA5};
	unsigned char buf1[5];
	unsigned char i;
	NRF24L01_Write_Buf(SPI_WRITE_REG+TX_ADDR,buf,5);//д��5���ֽڵĵ�ַ.
	NRF24L01_Read_Buf(TX_ADDR,buf1,5);              //����д��ĵ�ַ
	for(i=0; i<5; i++)if(buf1[i]!=0XA5)break;
	if(i!=5)
	{
		debug_out(("nRF24L01 TEST FAIL\r\n"));
		return 1;                               //NRF24L01����λ
	}
	debug_out(("nRF24L01 TEST OK\r\n"));
	return 0;		                           //NRF24L01��λ
}

#endif

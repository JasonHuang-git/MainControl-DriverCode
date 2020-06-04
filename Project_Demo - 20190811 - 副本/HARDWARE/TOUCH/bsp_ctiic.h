#ifndef __BSP_CTIIC_H
#define __BSP_CTIIC_H

#include <bsp.h>

// IO��������
#define CT_SDA_IN()  {GPIOC->MODER&=~(3<<(2*3));GPIOC->MODER|=0<<2*3;}	// PC3����ģʽ
#define CT_SDA_OUT() {GPIOC->MODER&=~(3<<(2*3));GPIOC->MODER|=1<<2*3;} 	// PC3���ģʽ
// IO��������
#define CT_IIC_SCL    PCout(4) 	// SCL
#define CT_IIC_SDA    PCout(3)	// SDA	 
#define CT_READ_SDA   PCin(3)   // ����SDA 


// IIC���в�������
void CT_IIC_Init(void);                		// ��ʼ��IIC��IO��
void CT_IIC_Start(void);					// ����IIC��ʼ�ź�
void CT_IIC_Stop(void);	  					// ����IICֹͣ�ź�
void CT_IIC_Send_Byte(uint8_t txd);			// IIC����һ���ֽ�
uint8_t CT_IIC_Read_Byte(unsigned char ack);// IIC��ȡһ���ֽ�
uint8_t CT_IIC_Wait_Ack(void); 				// IIC�ȴ�ACK�ź�
void CT_IIC_Ack(void);						// IIC����ACK�ź�
void CT_IIC_NAck(void);						// IIC������ACK�ź�

#endif /* __BSP_CTIIC_H */

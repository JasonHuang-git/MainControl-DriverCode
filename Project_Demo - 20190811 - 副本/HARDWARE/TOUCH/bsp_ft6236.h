#ifndef __BSP_FT6236_H
#define __BSP_FT6236_H	
#include <bsp.h>
#include <bsp_touch.h>
#include <bsp_ctiic.h>
#include <bsp_lcd.h>

// ����ݴ��������ӵ�оƬ����(δ����IIC����) 
// IO��������	 
#define FT_INT    				PCin(0)		// FT6236�ж�����	

// I2C��д����	
#define FT_CMD_WR 				0X70    	// д����
#define FT_CMD_RD 				0X71		// ������

// FT6236 ���ּĴ������� 
#define FT_DEVIDE_MODE 			0x00   		// FT6236ģʽ���ƼĴ���
#define FT_REG_NUM_FINGER       0x02		// ����״̬�Ĵ���
											   
#define FT_TP1_REG 				0X03	  	// ��һ�����������ݵ�ַ
#define FT_TP2_REG 				0X09		// �ڶ������������ݵ�ַ
#define FT_TP3_REG 				0X0F		// ���������������ݵ�ַ
#define FT_TP4_REG 				0X15		// ���ĸ����������ݵ�ַ
#define FT_TP5_REG 				0X1B		// ��������������ݵ�ַ  


#define	FT_ID_G_LIB_VERSION		0xA1		// �汾		
#define FT_ID_G_MODE 			0xA4   		// FT6236�ж�ģʽ���ƼĴ���
#define FT_ID_G_THGROUP			0x80   		// ������Чֵ���üĴ���
#define FT_ID_G_PERIODACTIVE	0x88   		// ����״̬�������üĴ���

// �ⲿ����
uint8_t FT6236_WR_Reg(uint16_t reg,uint8_t *buf,uint8_t len);
void FT6236_RD_Reg(uint16_t reg,uint8_t *buf,uint8_t len);
uint8_t FT6236_Init(void);
uint8_t FT6236_Scan(uint8_t mode);

#endif /* __BSP_FT6236_H */

#ifndef __LAN8720_H
#define __LAN8720_H

#include <bsp.h>
			
#define LAN8720_PHY_ADDRESS  	0x00				// LAN8720 PHYоƬ��ַ.
#define LAN8720_RST 		   	PAout(3) 			// LAN8720��λ����	 
                                                       
extern __align(4)  ETH_DMADESCTypeDef *DMARxDscrTab;// ��̫��DMA�������������ݽṹ��ָ��
extern __align(4)  ETH_DMADESCTypeDef *DMATxDscrTab;// ��̫��DMA�������������ݽṹ��ָ�� 
extern __align(4)  uint8_t *Rx_Buff; 				// ��̫���ײ���������buffersָ�� 
extern __align(4)  uint8_t *Tx_Buff; 				// ��̫���ײ���������buffersָ��
extern ETH_DMADESCTypeDef  *DMATxDescToSet;			// DMA����������׷��ָ��
extern ETH_DMADESCTypeDef  *DMARxDescToGet; 		// DMA����������׷��ָ�� 
extern ETH_DMA_Rx_Frame_infos *DMA_RX_FRAME_infos;	// DMA�����յ���֡��Ϣָ��
 

uint8_t LAN8720_Init(void);
uint8_t LAN8720_Get_Speed(void);
uint8_t ETH_MACDMA_Config(void);
FrameTypeDef ETH_Rx_Packet(void);
uint8_t ETH_Tx_Packet(uint16_t FrameLength);
uint32_t ETH_GetCurrentTxBuffer(void);
uint8_t ETH_Mem_Malloc(void);
void ETH_Mem_Free(void);
#endif 


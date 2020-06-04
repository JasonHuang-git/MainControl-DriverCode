#ifndef __BSP_DAC_H
#define __BSP_DAC_H

#include <bsp.h>

// DAC�ο���ѹ������ʵ����������޸� REF_VOLATGE
#define vref  REF_VOLATGE

// �����źŷ������Ľṹ��
typedef struct
{
	uint8_t    ucWaveType;   // ��������  
	uint32_t   ulFreq;       // ����Ƶ��  
	uint32_t   ulAMP;        // ���θ�ֵ  
	uint8_t    ucDuty;       // ����ռ�ձ�
}DAC_T;

extern DAC_T g_DAC1;		// ��������������εĲ���

extern uint16_t g_Wave1[128];

// ����ĺ�������DAC��ѹ����ͳ�ʼ��
void bsp_InitDAC1(void);
void bsp_SetDAC1(double voltage);

void bsp_InitDAC2(void);
void bsp_SetDAC2(double voltage);

// ����ĺ�������DMA���η�����
void dac1_InitForDMA(uint32_t _BufAddr, uint32_t _Count, uint32_t _DacFreq);
void dac1_SetSinWave(uint16_t _vpp, uint32_t _freq);
void dac1_SetRectWave(uint16_t _low, uint16_t _high, uint32_t _freq, uint16_t _duty);
void dac1_SetTriWave(uint16_t _low, uint16_t _high, uint32_t _freq, uint16_t _duty);
void dac1_StopWave(void);

// ����������û��дʵ�壬�������вο�DAC1��д
void dac2_InitForDMA(uint32_t _BufAddr, uint32_t _Count, uint32_t _DacFreq);
void dac2_SetSinWave(uint16_t _vpp, uint32_t _freq);
void dac2_SetRectWave(uint16_t _low, uint16_t _high, uint32_t _freq, uint16_t _duty);
void dac2_SetTriWave(uint16_t _low, uint16_t _high, uint32_t _freq, uint16_t _duty);
void dac2_StopWave(void);


#endif /* __BSP_DAC_H */

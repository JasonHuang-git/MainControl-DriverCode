#ifndef __BSP_DAC_H
#define __BSP_DAC_H

#include <bsp.h>

// DAC参考电压，根据实际情况进行修改 REF_VOLATGE
#define vref  REF_VOLATGE

// 用于信号发生器的结构体
typedef struct
{
	uint8_t    ucWaveType;   // 波形类型  
	uint32_t   ulFreq;       // 波形频率  
	uint32_t   ulAMP;        // 波形赋值  
	uint8_t    ucDuty;       // 波形占空比
}DAC_T;

extern DAC_T g_DAC1;		// 用于配置输出波形的参数

extern uint16_t g_Wave1[128];

// 下面的函数用于DAC电压输出和初始化
void bsp_InitDAC1(void);
void bsp_SetDAC1(double voltage);

void bsp_InitDAC2(void);
void bsp_SetDAC2(double voltage);

// 下面的函数用于DMA波形发生器
void dac1_InitForDMA(uint32_t _BufAddr, uint32_t _Count, uint32_t _DacFreq);
void dac1_SetSinWave(uint16_t _vpp, uint32_t _freq);
void dac1_SetRectWave(uint16_t _low, uint16_t _high, uint32_t _freq, uint16_t _duty);
void dac1_SetTriWave(uint16_t _low, uint16_t _high, uint32_t _freq, uint16_t _duty);
void dac1_StopWave(void);

// 保留函数，没有写实体，可以自行参考DAC1编写
void dac2_InitForDMA(uint32_t _BufAddr, uint32_t _Count, uint32_t _DacFreq);
void dac2_SetSinWave(uint16_t _vpp, uint32_t _freq);
void dac2_SetRectWave(uint16_t _low, uint16_t _high, uint32_t _freq, uint16_t _duty);
void dac2_SetTriWave(uint16_t _low, uint16_t _high, uint32_t _freq, uint16_t _duty);
void dac2_StopWave(void);


#endif /* __BSP_DAC_H */

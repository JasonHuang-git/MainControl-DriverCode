#ifndef __BSP_DSP_FFT_H
#define __BSP_DSP_FFT_H

#include <bsp.h>


// ���FFT�⣬��������
uint32_t * DSP_FFTPhase_ASMLIB(uint32_t * input, uint32_t * output, uint16_t _usFFTPoints);
uint32_t * DSP_FFT_ASMLIB(uint32_t * input, uint32_t * output, uint16_t _usFFTPoints);

// ����FFT��IFFT
float32_t * arm_cfft_f32_app(float32_t *pData,uint32_t ifftFlag);
// ����ʾ��
void arm_cfft_32_test(uint8_t flag);
void arm_cfft_radix2_q31_app(void);
void arm_cfft_radix2_q15_app(void);
void arm_cfft_radix4_q15_app(void);

// ʵ��FFT��IFFT
float32_t * arm_rfft_fast_f32_app(float32_t *pSampData, uint32_t flag);
// ����ʾ��
void arm_rfft_q15_app(void);
void arm_rfft_q31_app(void);

#endif /* __BSP_DSP_FFT_H */

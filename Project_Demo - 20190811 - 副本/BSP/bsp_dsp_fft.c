#include <bsp_dsp_fft.h>

// 与复数FFT相关的输入输出缓冲区 
#define TEST_LENGTH_SAMPLES 2048 

// FFT输入和输出缓冲
float32_t testInput_fft_2048[2048];
float32_t *testOutput_fft_2048;

// 与实数FFT相关的输入输出缓冲区
static float32_t rfft_testInput_f32[TEST_LENGTH_SAMPLES];
static float32_t rfft_testOutput_f32[TEST_LENGTH_SAMPLES];
static float32_t rfft_testOutput[TEST_LENGTH_SAMPLES];

/**
  * @name	   arm_rfft_fast_f32_app
  * @brief     求浮点数2048点快速FFT
  * @param	   pSampData：采样回来的数据
			   flag：FFT(0)/IFFT(1)
  * @return    做完快速FFT后返回数据地址
  * @DateTime  2019-7-27
  */
float32_t * arm_rfft_fast_f32_app(float32_t *pSampData, uint32_t flag)
{
	uint16_t i;
	uint16_t count = 0;
	float32_t fre = 0, max_amp = 0;
	uint32_t fftSize;
	arm_rfft_fast_instance_f32 S;
	
	// 实数序列FFT长度
	fftSize = 1024; 
	
	// 初始化结构体S中的参数
 	arm_rfft_fast_init_f32(&S, fftSize);
	
	// 按照实部，虚部，实部，虚部..... 的顺序存储数据
	for(i=0; i<2048; i++)
	{
		if(i==0) rfft_testInput_f32[0] = pSampData[0]/2;
		else rfft_testInput_f32[i] = pSampData[i];
	}
	
	// 2048点实序列快速FFT，浮点
	arm_rfft_fast_f32(&S, rfft_testInput_f32, rfft_testOutput_f32, 0);
	
	if(!flag)
	{
		// 求1024点幅值
		arm_cmplx_mag_f32(rfft_testOutput_f32, rfft_testOutput, fftSize);
		
		// 采样频率为7MHz时，估算频率
		max_amp = rfft_testOutput[1];
		for(i = 2; i < fftSize; i++)
		{
			if(max_amp < rfft_testOutput[i])
			{
				max_amp  = rfft_testOutput[i];
				count = i;
			}
		}
		fre = count*7000000.0/2048.0;
		printf("输入信号频率估算值：%f Hz\r\n", fre);
		printf("-----------------------------------------------------\r\n");

		return rfft_testOutput;
	}
	else if(flag)
	{
		arm_rfft_fast_f32(&S, rfft_testOutput_f32, rfft_testInput_f32, flag);
		return rfft_testInput_f32;
	}
	return 0;
}

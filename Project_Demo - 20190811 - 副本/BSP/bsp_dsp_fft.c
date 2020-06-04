#include <bsp_dsp_fft.h>

// �븴��FFT��ص�������������� 
#define TEST_LENGTH_SAMPLES 2048 

// FFT������������
float32_t testInput_fft_2048[2048];
float32_t *testOutput_fft_2048;

// ��ʵ��FFT��ص��������������
static float32_t rfft_testInput_f32[TEST_LENGTH_SAMPLES];
static float32_t rfft_testOutput_f32[TEST_LENGTH_SAMPLES];
static float32_t rfft_testOutput[TEST_LENGTH_SAMPLES];

/**
  * @name	   arm_rfft_fast_f32_app
  * @brief     �󸡵���2048�����FFT
  * @param	   pSampData����������������
			   flag��FFT(0)/IFFT(1)
  * @return    �������FFT�󷵻����ݵ�ַ
  * @DateTime  2019-7-27
  */
float32_t * arm_rfft_fast_f32_app(float32_t *pSampData, uint32_t flag)
{
	uint16_t i;
	uint16_t count = 0;
	float32_t fre = 0, max_amp = 0;
	uint32_t fftSize;
	arm_rfft_fast_instance_f32 S;
	
	// ʵ������FFT����
	fftSize = 1024; 
	
	// ��ʼ���ṹ��S�еĲ���
 	arm_rfft_fast_init_f32(&S, fftSize);
	
	// ����ʵ�����鲿��ʵ�����鲿..... ��˳��洢����
	for(i=0; i<2048; i++)
	{
		if(i==0) rfft_testInput_f32[0] = pSampData[0]/2;
		else rfft_testInput_f32[i] = pSampData[i];
	}
	
	// 2048��ʵ���п���FFT������
	arm_rfft_fast_f32(&S, rfft_testInput_f32, rfft_testOutput_f32, 0);
	
	if(!flag)
	{
		// ��1024���ֵ
		arm_cmplx_mag_f32(rfft_testOutput_f32, rfft_testOutput, fftSize);
		
		// ����Ƶ��Ϊ7MHzʱ������Ƶ��
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
		printf("�����ź�Ƶ�ʹ���ֵ��%f Hz\r\n", fre);
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

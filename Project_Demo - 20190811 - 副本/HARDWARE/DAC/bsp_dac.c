#include <bsp_dac.h>


/*
	PA4���� DAC_OUT1

	DAC1ʹ����TIM6��Ϊ��ʱ������ DMAͨ��: DMA1_Stream5
	
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable ������DAC������壬������������,
	���˻���֮�󣬿���0V�Ͳο���Դʱ��ʧ�����������50mV
	�������岨�νϺã���0VĿ�ⲻ������ʧ�档
	
	���ܣ�
	1��������Ҳ������Ⱥ�Ƶ�ʿɵ���
	2���������������ƫ�ƿɵ��ڣ�Ƶ�ʿɵ��ڣ�ռ�ձȿ��Ե���
	3��������ǲ������ȿɵ��ڣ�Ƶ�ʿɵ��ڣ�������ռ�ȿɵ���
	4��������DAC���ֱ����ƽ�ĺ���
*/

/*  ���Ҳ����ݣ�12bit��1������128����, 0-4095֮��仯 */
const uint16_t g_SineWave128[] = {
	2047 ,
	2147 ,
	2248 ,
	2347 ,
	2446 ,
	2544 ,
	2641 ,
	2737 ,
	2830 ,
	2922 ,
	3012 ,
	3099 ,
	3184 ,
	3266 ,
	3346 ,
	3422 ,
	3494 ,
	3564 ,
	3629 ,
	3691 ,
	3749 ,
	3803 ,
	3852 ,
	3897 ,
	3938 ,
	3974 ,
	4006 ,
	4033 ,
	4055 ,
	4072 ,
	4084 ,
	4092 ,
	4094 ,
	4092 ,
	4084 ,
	4072 ,
	4055 ,
	4033 ,
	4006 ,
	3974 ,
	3938 ,
	3897 ,
	3852 ,
	3803 ,
	3749 ,
	3691 ,
	3629 ,
	3564 ,
	3494 ,
	3422 ,
	3346 ,
	3266 ,
	3184 ,
	3099 ,
	3012 ,
	2922 ,
	2830 ,
	2737 ,
	2641 ,
	2544 ,
	2446 ,
	2347 ,
	2248 ,
	2147 ,
	2047 ,
	1947 ,
	1846 ,
	1747 ,
	1648 ,
	1550 ,
	1453 ,
	1357 ,
	1264 ,
	1172 ,
	1082 ,
	995  ,
	910  ,
	828  ,
	748  ,
	672  ,
	600  ,
	530  ,
	465  ,
	403  ,
	345  ,
	291  ,
	242  ,
	197  ,
	156  ,
	120  ,
	88   ,
	61   ,
	39   ,
	22   ,
	10   ,
	2    ,
	0    ,
	2    ,
	10   ,
	22   ,
	39   ,
	61   ,
	88   ,
	120  ,
	156  ,
	197  ,
	242  ,
	291  ,
	345  ,
	403  ,
	465  ,
	530  ,
	600  ,
	672  ,
	748  ,
	828  ,
	910  ,
	995  ,
	1082 ,
	1172 ,
	1264 ,
	1357 ,
	1453 ,
	1550 ,
	1648 ,
	1747 ,
	1846 ,
	1947
};

/* ���Ҳ� (32�������ʺϸ�Ƶ�� */
const uint16_t g_SineWave32[32] = {
                      2047, 2447, 2831, 3185, 3498, 3750, 3939, 4056, 4095, 4056,
                      3939, 3750, 3495, 3185, 2831, 2447, 2047, 1647, 1263, 909,
                      599, 344, 155, 38, 0, 38, 155, 344, 599, 909, 1263, 1647};

const char *g_WaveType[] =
{
    "Sine",
	"Square",
	"Ramp",
};

/* DMA���λ����� */
uint16_t g_Wave1[128];					  
DAC_T g_DAC1;

/*
*********************************************************************************************************
*	�� �� ��: bsp_InitDAC1
*	����˵��: ����PA4/DAC1
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitDAC1(void)
{	
	/* ����GPIO */
	{
		GPIO_InitTypeDef GPIO_InitStructure;
		
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
		
		/* ����ADC����Ϊģ������ģʽ  PA4 / DAC_OUT1 */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	}	

	/* DACͨ��1���� */
	{
		DAC_InitTypeDef DAC_InitStructure;
		
		/* ʹ��DACʱ�� */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);		

		DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;	/* ѡ���������, ����޸�DAC���ݼĴ��� */
		DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
		DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;
		DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
		DAC_Init(DAC_Channel_1, &DAC_InitStructure);
		DAC_Cmd(DAC_Channel_1, ENABLE);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_InitDAC2
*	����˵��: ����PA5/DAC2
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitDAC2(void)
{	
	/* ����GPIO */
	{
		GPIO_InitTypeDef GPIO_InitStructure;
		
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
		
		/* ����ADC����Ϊģ������ģʽ  PA5 / DAC_OUT2 */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	}	

	/* DACͨ�������� */
	{
		DAC_InitTypeDef DAC_InitStructure;
		
		/* ʹ��DACʱ�� */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);		

		DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;	/* ѡ���������, ����޸�DAC���ݼĴ��� */
		DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
		DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;
		DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
		DAC_Init(DAC_Channel_2, &DAC_InitStructure);
		DAC_Cmd(DAC_Channel_2, ENABLE);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_SetDAC1
*	����˵��: ����DAC1������ݼĴ������ı������ѹ
*	��    ��: voltage : DAC�����ѹ��0-3.30
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_SetDAC1(double voltage)
{
	uint16_t _dac = voltage*409500.0/vref/100.0;
	DAC->DHR12R1 = _dac;
}


/*
*********************************************************************************************************
*	�� �� ��: bsp_SetDAC2
*	����˵��: ����DAC2������ݼĴ������ı������ѹ
*	��    ��: _voltage : DAC�����ѹ��0-3.30
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_SetDAC2(double voltage)
{
	uint16_t _dac = voltage*409500.0/vref/100.0;
	DAC->DHR12R2 = _dac;
}

/*
*********************************************************************************************************
*	�� �� ��: dac1_InitForDMA
*	����˵��: ����PA4 ΪDAC_OUT1, ����DMA2
*	��    ��: _BufAddr : DMA���ݻ�������ַ
*			  _Count   : ��������������
*			 _DacFreq  : DAC��������Ƶ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void dac1_InitForDMA(uint32_t _BufAddr, uint32_t _Count, uint32_t _DacFreq)
{	
	uint16_t usPeriod;
	uint16_t usPrescaler;
	uint32_t uiTIMxCLK;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	DMA_Cmd(DMA1_Stream5, DISABLE);
	DAC_DMACmd(DAC_Channel_1, DISABLE);
	TIM_Cmd(TIM6, DISABLE);
	
	/* ����GPIO.*/
	{
		GPIO_InitTypeDef GPIO_InitStructure;
		
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
		
		/* ����ADC����Ϊģ������ģʽ  PA4 / DAC_OUT1 */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	}	

	/* TIM6���� */
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

		uiTIMxCLK = SystemCoreClock / 2;
		
		if (_DacFreq < 100)
		{
			usPrescaler = 10000 - 1;						 /* ��Ƶ�� = 10000 */
			usPeriod =  (uiTIMxCLK / 10000) / _DacFreq  - 1; /* �Զ���װ��ֵ */
		}
		else if (_DacFreq < 3000)
		{
			usPrescaler = 100 - 1;							/* ��Ƶ�� = 100 */
			usPeriod =  (uiTIMxCLK / 100) / _DacFreq  - 1;	/* �Զ���װ��ֵ */
		}
		else	/* ����4K��Ƶ�ʣ������Ƶ */
		{
			usPrescaler = 0;						/* ��Ƶ�� = 1 */
			usPeriod = uiTIMxCLK / _DacFreq - 1;	/* �Զ���װ��ֵ */
		}

		TIM_TimeBaseStructure.TIM_Period = usPeriod;
		TIM_TimeBaseStructure.TIM_Prescaler = usPrescaler;
		TIM_TimeBaseStructure.TIM_ClockDivision = 0;
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
		TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0000;		/* TIM1 �� TIM8 �������� */	

		TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

		/* ѡ��TIM6��DAC�Ĵ���ʱ�� */
		TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);
	}

	/* DACͨ��1���� */
	{
		DAC_InitTypeDef DAC_InitStructure;
		
		/* ʹ��DACʱ�� */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);		

		DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
		DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
		DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;
		//DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
		DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
		DAC_Init(DAC_Channel_1, &DAC_InitStructure);
		DAC_Cmd(DAC_Channel_1, ENABLE);
	}

	/* DMA1_Stream5���� */
	{
		DMA_InitTypeDef DMA_InitStructure;

		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

		/* ����DMA1 Stream 5 channel 7����DAC1 */
		DMA_InitStructure.DMA_Channel = DMA_Channel_7;
		DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&DAC->DHR12R1; 
		DMA_InitStructure.DMA_Memory0BaseAddr = _BufAddr;	
		DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;		
		DMA_InitStructure.DMA_BufferSize = _Count;
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
		DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_HalfWord;
		DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
		DMA_InitStructure.DMA_Priority = DMA_Priority_High;
		DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
		DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
		DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
		DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
		DMA_Init(DMA1_Stream5, &DMA_InitStructure);
		DMA_Cmd(DMA1_Stream5, ENABLE);

		/* ʹ��DACͨ��1��DMA */
		DAC_DMACmd(DAC_Channel_1, ENABLE);
	}

	/* ʹ�ܶ�ʱ�� */
	TIM_Cmd(TIM6, ENABLE);
}

/*
*********************************************************************************************************
*	�� �� ��: dac1_SetSinWave
*	����˵��: DAC1������Ҳ�
*	��    ��: _vpp : ���� 0-4095;
*			  _freq : Ƶ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void dac1_SetSinWave(uint16_t _vpp, uint32_t _freq)
{	
	uint32_t i;
	uint32_t dac;
	
	TIM_Cmd(TIM6, DISABLE);
		
	/* �������Ҳ����� */		
	for (i = 0; i < 32; i++)
	{
		dac = (g_SineWave32[i] * _vpp) / 4095;
		if (dac > 4095)
		{
			dac = 4095;	
		}
		g_Wave1[i] = dac;
	}
	
	dac1_InitForDMA((uint32_t)&g_Wave1, 32, _freq * 32);
}

/*
*********************************************************************************************************
*	�� �� ��: dac1_SetRectWave
*	����˵��: DAC1�������
*	��    ��: _low  : �͵�ƽʱDAC, 
*			  _high : �ߵ�ƽʱDAC
*			  _freq : Ƶ�� Hz
*			  _duty : ռ�ձ� 2% - 98%, ���ڲ��� 1%
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void dac1_SetRectWave(uint16_t _low, uint16_t _high, uint32_t _freq, uint16_t _duty)
{	
	uint16_t i;
	TIM_Cmd(TIM6, DISABLE);
	
	for (i = 0; i < (_duty * 128) / 100; i++)
	{
		g_Wave1[i] = _high;
	}
	for (; i < 128; i++)
	{
		g_Wave1[i] = _low;
	}
	
	dac1_InitForDMA((uint32_t)&g_Wave1, 128, _freq * 128);
}

/*
*********************************************************************************************************
*	�� �� ��: dac1_SetTriWave
*	����˵��: DAC1������ǲ�
*	��    ��: _low : �͵�ƽʱDAC, 
*			  _high : �ߵ�ƽʱDAC
*			  _freq : Ƶ�� Hz
*			  _duty : ռ�ձ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void dac1_SetTriWave(uint16_t _low, uint16_t _high, uint32_t _freq, uint16_t _duty)
{	
	uint32_t i;
	uint16_t dac;
	uint16_t m;
	
	TIM_Cmd(TIM6, DISABLE);
		
	/* �������ǲ����飬128���������� _low �� _high */		
	m = (_duty * 128) / 100;
	
	if (m == 0)
	{
		m = 1;
	}
	
	if (m > 127)
	{
		m = 127;
	}
	for (i = 0; i < m; i++)
	{
		dac = _low + ((_high - _low) * i) / m;
		g_Wave1[i] = dac;
	}
	for (; i < 128; i++)
	{
		dac = _high - ((_high - _low) * (i - m)) / (128 - m);
		g_Wave1[i] = dac;
	}	
	
	dac1_InitForDMA((uint32_t)&g_Wave1, 128, _freq * 128);
}

/*
*********************************************************************************************************
*	�� �� ��: dac1_StopWave
*	����˵��: ֹͣDAC1���
*	��    ��: ��
*			  _freq : Ƶ�� 0-5Hz
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void dac1_StopWave(void)
{	
	DMA_Cmd(DMA1_Stream5, DISABLE);
	DAC_DMACmd(DAC_Channel_1, DISABLE);
	TIM_Cmd(TIM6, DISABLE);
}


#include <bsp_adc.h>

/* �궨�� ---------------------------------------------------------------------*/
#define ADC_CDR_ADDRESS    ((uint32_t)0x40012308)	// ����洢����ַ

/* ���� ----------------------------------------------------------------------*/
__IO uint16_t uhADCDualConvertedValue[1024*10];		// ADC�ɼ���������С
__IO uint16_t uhADCDualConver = 0;

/**
 * @name	  bsp_InitADC
 * @brief     ADC1��ADC2��ADC3��ʼ��������ADC1+ADC2ΪDMA˫�ؽ������ģʽ��������7MSPS,ADC3���������ڲ�����������DMA
 * @param	  ��
 * @return    ��
 * @DateTime  2019-7-29
 */
void bsp_InitADC(uint8_t mode)
{  
    ADC_InitTypeDef       ADC_InitStructure;
    ADC_CommonInitTypeDef ADC_CommonInitStructure;
    DMA_InitTypeDef       DMA_InitStructure;
    GPIO_InitTypeDef      GPIO_InitStructure;

    /* ʹ�� ADC1, ADC2, DMA2 �� GPIO ʱ�� ****************************************/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_ADC2 | 
						 RCC_APB2Periph_ADC3, ENABLE);

	if(mode == 1)
    {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 | RCC_AHB1Periph_GPIOA, ENABLE);

        /* DMA2 Stream0 channel0 ���� ����ADC1**************************************/
        DMA_InitStructure.DMA_Channel = DMA_Channel_0;  
        DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC_CDR_ADDRESS;
        DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&uhADCDualConver;
        DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
        DMA_InitStructure.DMA_BufferSize = 1;
        DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
        DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
        DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
        DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
        DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
        DMA_InitStructure.DMA_Priority = DMA_Priority_High;
        DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
        DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
        DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
        DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
        DMA_Init(DMA2_Stream0, &DMA_InitStructure);
        DMA_Cmd(DMA2_Stream0, ENABLE);


        // ����ADC����Ϊģ������ģʽ
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        /****************************************************************************   
          PCLK2 = HCLK / 2 
          ����ѡ�����2��Ƶ
          ADCCLK = PCLK2 /4 = HCLK / 4 = 168 / 4 = 42M
          ˫ADC����Ƶ�ʣ� 42 / 6 = 7Mbps
        *****************************************************************************/
        
        // ADC�������ֳ�ʼ��������˫�ؽ���ģʽ
        ADC_CommonInitStructure.ADC_Mode = ADC_DualMode_Interl;
        ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_6Cycles;
        ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_3;
        ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
        ADC_CommonInit(&ADC_CommonInitStructure);  

        // DMA mode 3 ���ڽ���ģʽ 6-bit ���� 8-bit �ֱ���
        ADC_InitStructure.ADC_Resolution = ADC_Resolution_8b;
        ADC_InitStructure.ADC_ScanConvMode = DISABLE;
        ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
        ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
        ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
        ADC_InitStructure.ADC_NbrOfConversion = 1;
        
        // ADC1 ����ͨ������
        ADC_Init(ADC1, &ADC_InitStructure);
        ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_3Cycles);

        // ADC2 ����ͨ������
        ADC_Init(ADC2, &ADC_InitStructure);
        ADC_RegularChannelConfig(ADC2, ADC_Channel_2, 1, ADC_SampleTime_3Cycles);

        // ʹ�� ADC1 DMA
        ADC_DMACmd(ADC1, ENABLE);
        
        // ʹ��DMA���� (��ADCģʽ)
        ADC_MultiModeDMARequestAfterLastTransferCmd(ENABLE);

        // ʹ�� ADC1
        ADC_Cmd(ADC1, ENABLE);

        // ʹ�� ADC2
        ADC_Cmd(ADC2, ENABLE);

        // �������ת��
        ADC_SoftwareStartConv(ADC1);
    }
    else if(mode == 0)
    {
        // ADC3��س�ʼ��
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        
        ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
        ADC_InitStructure.ADC_ScanConvMode = DISABLE;
        ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
        ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
        ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
        ADC_InitStructure.ADC_NbrOfConversion = 1;
        
        // ADC3 ����ͨ������
        ADC_Init(ADC1, &ADC_InitStructure);
        ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_480Cycles);
        
        // ʹ�� ADC3
        ADC_Cmd(ADC1, ENABLE);
    }
        // ADC3��س�ʼ��
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        
        ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
        ADC_InitStructure.ADC_ScanConvMode = DISABLE;
        ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
        ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
        ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
        ADC_InitStructure.ADC_NbrOfConversion = 1;
        
        // ADC3 ����ͨ������
        ADC_Init(ADC3, &ADC_InitStructure);
        ADC_RegularChannelConfig(ADC3, ADC_Channel_3, 1, ADC_SampleTime_480Cycles);
        
        // ʹ�� ADC3
        ADC_Cmd(ADC3, ENABLE);
    
     
            // ADC3��س�ʼ��
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        
        ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
        ADC_InitStructure.ADC_ScanConvMode = DISABLE;
        ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
        ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
        ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
        ADC_InitStructure.ADC_NbrOfConversion = 1;
        
        // ADC3 ����ͨ������
        ADC_Init(ADC2, &ADC_InitStructure);
        ADC_RegularChannelConfig(ADC2, ADC_Channel_6, 1, ADC_SampleTime_480Cycles);
        
        // ʹ�� ADC3
        ADC_Cmd(ADC2, ENABLE);                
}

void adc1_channel_init(uint8_t mode)
{
    ADC_InitTypeDef       ADC_InitStructure;
    DMA_InitTypeDef       DMA_InitStructure;
    GPIO_InitTypeDef      GPIO_InitStructure;

    if(mode==1)
    {
        /* ʹ�� ADC1, ADC2, DMA2 �� GPIO ʱ�� ****************************************/
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
        // ADC1��س�ʼ��
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        
        ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
        ADC_InitStructure.ADC_ScanConvMode = DISABLE;
        ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
        ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
        ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
        ADC_InitStructure.ADC_NbrOfConversion = 1;
        
        // ADC3 ����ͨ������
        ADC_Init(ADC1, &ADC_InitStructure);
        ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 1, ADC_SampleTime_480Cycles);
        
        // ʹ�� ADC3
        ADC_Cmd(ADC1, ENABLE);
    }
    else if(mode==0)
    {
                /* ʹ�� ADC1, ADC2, DMA2 �� GPIO ʱ�� ****************************************/
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
        // ADC1��س�ʼ��
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        
        ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
        ADC_InitStructure.ADC_ScanConvMode = DISABLE;
        ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
        ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
        ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
        ADC_InitStructure.ADC_NbrOfConversion = 1;
        
        // ADC3 ����ͨ������
        ADC_Init(ADC1, &ADC_InitStructure);
        ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_480Cycles);
        
        // ʹ�� ADC3
        ADC_Cmd(ADC1, ENABLE);
    }
}

/**
 * @name	  Get_Voltage
 * @brief     ���ADC3��ֵ
 * @param	  ��
 * @return    ADC������������
 * @DateTime  2019-7-29
 */
uint16_t Get_Voltage(void)   
{
	// ����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	// ADC1,ADCͨ��,480������,��߲���ʱ�������߾�ȷ��
	ADC_RegularChannelConfig(ADC3, ADC_Channel_3, 3, ADC_SampleTime_480Cycles );				    
  
	ADC_SoftwareStartConv(ADC3);					// ʹ��ָ����ADC3�����ת����������	
	 
	while(!ADC_GetFlagStatus(ADC3, ADC_FLAG_EOC ));	// �ȴ�ת������

	return ADC_GetConversionValue(ADC3);			// �������һ��ADC3�������ת�����
}

/**
 * @name	  Get_Voltage
 * @brief     ���ADC3��ֵ
 * @param	  times:��ȡ����
 * @return    ADC3ͨ��3��times��ת�����ƽ��ֵ
 * @DateTime  2019-7-29
 */
uint16_t Get_ADC_Average(uint8_t times)
{
	uint32_t temp_val = 0;
	uint8_t t;
	for(t = 0; t < times; t++)
	{
		temp_val += Get_Voltage();
//		delay_ms(5);
	}
	return temp_val/times;
} 

/////////////////////////
/*ADC1��غ���*/

uint16_t Get_ADC1_Voltage(void)   
{
	// ����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	// ADC1,ADCͨ��,480������,��߲���ʱ�������߾�ȷ��
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_480Cycles );				    
  
	ADC_SoftwareStartConv(ADC1);					// ʹ��ָ����ADC3�����ת����������	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));	// �ȴ�ת������

	return ADC_GetConversionValue(ADC1);			// �������һ��ADC3�������ת�����
}

uint16_t Get_ADC1_Average(uint8_t times)
{
	uint32_t temp_val = 0;
	uint8_t t;
	for(t = 0; t < times; t++)
	{
		temp_val += Get_ADC1_Voltage();
//		delay_ms(5);
	}
	return temp_val/times;
} 


uint16_t Get_ADC1Voltage(void)   
{
	// ����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	// ADC1,ADCͨ��,480������,��߲���ʱ�������߾�ȷ��
	ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 1, ADC_SampleTime_480Cycles );				    
  
	ADC_SoftwareStartConv(ADC1);					// ʹ��ָ����ADC3�����ת����������	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));	// �ȴ�ת������

	return ADC_GetConversionValue(ADC1);			// �������һ��ADC3�������ת�����
}

uint16_t Get_ADC2_Voltage(void)   
{
	// ����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	// ADC1,ADCͨ��,480������,��߲���ʱ�������߾�ȷ��
	ADC_RegularChannelConfig(ADC2, ADC_Channel_6, 1, ADC_SampleTime_480Cycles );				    
  
	ADC_SoftwareStartConv(ADC2);					// ʹ��ָ����ADC3�����ת����������	
	 
	while(!ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC ));	// �ȴ�ת������

	return ADC_GetConversionValue(ADC2);			// �������һ��ADC3�������ת�����
}

uint16_t Get_ADC2_Average(uint8_t times)
{
	uint32_t temp_val = 0;
	uint8_t t;
	for(t = 0; t < times; t++)
	{
		temp_val += Get_ADC2_Voltage();
//		delay_ms(5);
	}
	return temp_val/times;
} 

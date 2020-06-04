#include <bsp_timer.h>

// ����ɨƵʹ��
extern float fre_step;
extern float next_fre;
extern uint8_t update_flag;
// ϵͳʱ��Ƶ�ʣ���λMHz
extern uint8_t sys_clk;
// ����ɨƵʱʱ�����
uint16_t ncount = 0;

/*************************************************************************************************************************************
															�ڲ���������
*************************************************************************************************************************************/
// ���ö�ʱ��2���ж����ȼ�
static void NVIC_Configuration2(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;            
	// TIM2�ж�
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;		 
	// ��ռ���ȼ�1��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	
	// �����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
	// ʹ��TIM1�ж�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								
	NVIC_Init(&NVIC_InitStructure);
}
// ���ö�ʱ��3���ж����ȼ�
static void NVIC_Configuration3(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;            
	// TIM1�ж�
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;		
	// ��ռ���ȼ�1��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	
	// �����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
	// ʹ��TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								
	NVIC_Init(&NVIC_InitStructure);
}
// ���ö�ʱ��4���ж����ȼ�
static void NVIC_Configuration4(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;             
	// TIM4�ж�
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;		 
	// ��ռ���ȼ�1��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	
	// �����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
	// ʹ��TIM1�ж�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								
	NVIC_Init(&NVIC_InitStructure);
}

// arr���Զ���װֵ��
// psc��ʱ��Ԥ��Ƶ��
// ��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
// Ft=��ʱ������Ƶ��,��λ:Mhz
// ����ʹ�õ��Ƕ�ʱ��2��3��4!
static void TIM2_Mode_Init(uint16_t arr,uint16_t psc)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);  	  		// TIM2ʱ��ʹ��    
	
	TIM_TimeBaseStructure.TIM_Prescaler		= psc;  				// ��ʱ����Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode	= TIM_CounterMode_Up; 	// ���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_Period		= arr;   				// �Զ���װ��ֵ
	TIM_TimeBaseStructure.TIM_ClockDivision	= TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);			  	    // ��ʼ����ʱ��2
 
	TIM_OCInitStructure.TIM_OCMode 		 = TIM_OCMode_Toggle; 		// ѡ��ʱ��ģʽ�����ȡ��ģʽ
 	TIM_OCInitStructure.TIM_OutputState  = TIM_OutputState_Enable; 	
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	TIM_OCInitStructure.TIM_OCIdleState  = TIM_OCIdleState_Set;
	TIM_OCInitStructure.TIM_OCPolarity 	 = TIM_OCPolarity_Low; 		// �������:TIM����Ƚϼ��Ե�
	TIM_OCInitStructure.TIM_OCNPolarity  = TIM_OCNPolarity_High;
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);  						// ����Tָ���Ĳ�����ʼ������TIM2 4OC1
	
	// �Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	// ���ô�װ�벶��ȽϼĴ���������ֵ����ռ�ձ�
	TIM_OCInitStructure.TIM_Pulse = CCR1_Val; 					
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);  
	TIM_OC1PreloadConfig(TIM2,TIM_OCPreload_Disable);
	
	// �Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CCR2_Val;
	TIM_OC2Init(TIM2,&TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Disable);
	
	// �Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CCR3_Val;
	TIM_OC3Init(TIM2,&TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Disable);
	
	// �Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CCR4_Val;
	TIM_OC4Init(TIM2,&TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Disable);
  
	// ʹ��TIMx��ARR�ϵ�Ԥװ�ؼĴ���
	// TIM_ARRPreloadConfig(TIM2, ENABLE); 															
	// ʹ��TIMx
	TIM_Cmd(TIM2, ENABLE); 
	TIM_CtrlPWMOutputs(TIM2,ENABLE);
	TIM_ITConfig(TIM2,TIM_IT_CC1 | TIM_IT_CC2 | TIM_IT_CC3 | TIM_IT_CC4 ,ENABLE);
}
// ��ʱ��3ģʽ����
static void TIM3_Mode_Init(uint16_t arr,uint16_t psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  		  		// ʹ��TIM3ʱ��
	
	TIM_TimeBaseInitStructure.TIM_Period        = arr; 				  	// �Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler     = psc;  				// ��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode   = TIM_CounterMode_Up; 	// ���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);			  		// ��ʼ��TIM3
	
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); 					  		// ����ʱ��3�����ж�
	TIM_Cmd(TIM3, ENABLE); 										  		// ʹ�ܶ�ʱ��3
}
// ��ʱ��4ģʽ����
static void TIM4_Mode_Init(uint16_t arr,uint16_t psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);  		  		// ʹ��TIM4ʱ��
	
	TIM_TimeBaseInitStructure.TIM_Period        = arr; 				  	// �Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler     = psc;  				// ��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode   = TIM_CounterMode_Up; 	// ���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);			  		// ��ʼ��TIM4
	
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE); 					  		// ����ʱ��4�����ж�
	TIM_Cmd(TIM4, ENABLE); 										  		// ʹ�ܶ�ʱ��4
}

/*************************************************************************************************************************************
															�ⲿ��������
*************************************************************************************************************************************/

/**
 * @name	  TIM2_Init
 * @brief     ��ʼ����ʱ��2���4·PWM
 * @param	  ��
 * @return    ��
 * @DateTime  2019-7-17
 */
void TIM2_Init(void)
{
	TIM2_GPIO_Init();
	TIM2_Mode_Init(65536-1, 3-1);
	NVIC_Configuration2();
}

/**
 * @name	  TIM3_Init
 * @brief     ��ʼ����ʱ��3Ϊ��ͨ����ģʽ
 * @param	  ��
 * @return    ��
 * @DateTime  2019-7-17
 */
void TIM3_Init(void)
{
	TIM3_Mode_Init(6-1, 6000-1);
	NVIC_Configuration3();
}

/**
 * @name	  TIM4_Init
 * @brief     ��ʼ����ʱ��4Ϊ��ͨ����ģʽ
 * @param	  ��
 * @return    ��
 * @DateTime  2019-7-17
 */

void TIM4_Init(uint16_t step_time_ms)
{
	uint32_t TIM_Clk = sys_clk / 2;
	
	TIM4_Mode_Init(step_time_ms-1, TIM_Clk-1);
	NVIC_Configuration4();
}

/**
 * @name	  TIM2_GPIO_Init
 * @brief     ��ʼ����ʱ��2��GPIO�������
 * @param	  ��
 * @return    ��
 * @DateTime  2019-7-17
 */
void TIM2_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	// ʹ��GPIO�͸���ʱ��
	RCC_APB2PeriphClockCmd(AF_CLK, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = AF_PIN_0 | AF_PIN_1 |AF_PIN_2 |AF_PIN_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(AF_PORT, &GPIO_InitStructure);	
}


/*************************************************************************************************************************************
															��ʱ���жϷ���������
*************************************************************************************************************************************/

// ��ʱ��2�жϷ�����
void TIM2_IRQHandler(void)   		// TIM2�ж�
{
	if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);
		TIM_SetCompare1(TIM2, TIM_GetCapture1(TIM2) + CCR1_Val);
	}

	 if (TIM_GetITStatus(TIM2, TIM_IT_CC2) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);
		TIM_SetCompare2(TIM2, TIM_GetCapture2(TIM2) + CCR2_Val);
	}


	if (TIM_GetITStatus(TIM2, TIM_IT_CC3) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC3);
		TIM_SetCompare3(TIM2, TIM_GetCapture3(TIM2) + CCR3_Val);
	}

	if (TIM_GetITStatus(TIM2, TIM_IT_CC4) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC4);
		TIM_SetCompare4(TIM2, TIM_GetCapture4(TIM2) + CCR4_Val);
	}
}

// ��ʱ��3�жϷ�����
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)  // ����ж�
	{
		PS0 = ~PS0;
		PS1 = ~PS1;
		PS2 = ~PS2;
		PS2 = ~PS3;
	}
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);     // ����жϱ�־λ
}

// ��ʱ��4�жϷ�����
void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4, TIM_IT_Update) == SET)  // ����ж�
	{
		if(ncount < 1000)		// �ȴ�1ms�ĵ���
			ncount++;
		else
		{
			update_flag = 1;
			next_fre = next_fre + fre_step;
			printf("next_fre = %f\r\n", next_fre);
			ncount = 0;
		}
	}
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);     // ����жϱ�־λ
}

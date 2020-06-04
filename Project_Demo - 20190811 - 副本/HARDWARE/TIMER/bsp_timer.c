#include <bsp_timer.h>

// 用于扫频使用
extern float fre_step;
extern float next_fre;
extern uint8_t update_flag;
// 系统时钟频率，单位MHz
extern uint8_t sys_clk;
// 用于扫频时时间计数
uint16_t ncount = 0;

/*************************************************************************************************************************************
															内部函数区域
*************************************************************************************************************************************/
// 配置定时器2的中断优先级
static void NVIC_Configuration2(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;            
	// TIM2中断
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;		 
	// 先占优先级1级
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	
	// 从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
	// 使能TIM1中断
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								
	NVIC_Init(&NVIC_InitStructure);
}
// 配置定时器3的中断优先级
static void NVIC_Configuration3(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;            
	// TIM1中断
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;		
	// 先占优先级1级
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	
	// 从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
	// 使能TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								
	NVIC_Init(&NVIC_InitStructure);
}
// 配置定时器4的中断优先级
static void NVIC_Configuration4(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;             
	// TIM4中断
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;		 
	// 先占优先级1级
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	
	// 从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
	// 使能TIM1中断
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								
	NVIC_Init(&NVIC_InitStructure);
}

// arr：自动重装值。
// psc：时钟预分频数
// 定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
// Ft=定时器工作频率,单位:Mhz
// 这里使用的是定时器2、3、4!
static void TIM2_Mode_Init(uint16_t arr,uint16_t psc)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);  	  		// TIM2时钟使能    
	
	TIM_TimeBaseStructure.TIM_Prescaler		= psc;  				// 定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode	= TIM_CounterMode_Up; 	// 向上计数模式
	TIM_TimeBaseStructure.TIM_Period		= arr;   				// 自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision	= TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);			  	    // 初始化定时器2
 
	TIM_OCInitStructure.TIM_OCMode 		 = TIM_OCMode_Toggle; 		// 选择定时器模式：溢出取反模式
 	TIM_OCInitStructure.TIM_OutputState  = TIM_OutputState_Enable; 	
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	TIM_OCInitStructure.TIM_OCIdleState  = TIM_OCIdleState_Set;
	TIM_OCInitStructure.TIM_OCPolarity 	 = TIM_OCPolarity_Low; 		// 输出极性:TIM输出比较极性低
	TIM_OCInitStructure.TIM_OCNPolarity  = TIM_OCNPolarity_High;
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);  						// 根据T指定的参数初始化外设TIM2 4OC1
	
	// 比较输出使能
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	// 设置待装入捕获比较寄存器的脉冲值，即占空比
	TIM_OCInitStructure.TIM_Pulse = CCR1_Val; 					
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);  
	TIM_OC1PreloadConfig(TIM2,TIM_OCPreload_Disable);
	
	// 比较输出使能
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CCR2_Val;
	TIM_OC2Init(TIM2,&TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Disable);
	
	// 比较输出使能
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CCR3_Val;
	TIM_OC3Init(TIM2,&TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Disable);
	
	// 比较输出使能
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CCR4_Val;
	TIM_OC4Init(TIM2,&TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Disable);
  
	// 使能TIMx在ARR上的预装载寄存器
	// TIM_ARRPreloadConfig(TIM2, ENABLE); 															
	// 使能TIMx
	TIM_Cmd(TIM2, ENABLE); 
	TIM_CtrlPWMOutputs(TIM2,ENABLE);
	TIM_ITConfig(TIM2,TIM_IT_CC1 | TIM_IT_CC2 | TIM_IT_CC3 | TIM_IT_CC4 ,ENABLE);
}
// 定时器3模式设置
static void TIM3_Mode_Init(uint16_t arr,uint16_t psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  		  		// 使能TIM3时钟
	
	TIM_TimeBaseInitStructure.TIM_Period        = arr; 				  	// 自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler     = psc;  				// 定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode   = TIM_CounterMode_Up; 	// 向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);			  		// 初始化TIM3
	
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); 					  		// 允许定时器3更新中断
	TIM_Cmd(TIM3, ENABLE); 										  		// 使能定时器3
}
// 定时器4模式设置
static void TIM4_Mode_Init(uint16_t arr,uint16_t psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);  		  		// 使能TIM4时钟
	
	TIM_TimeBaseInitStructure.TIM_Period        = arr; 				  	// 自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler     = psc;  				// 定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode   = TIM_CounterMode_Up; 	// 向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);			  		// 初始化TIM4
	
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE); 					  		// 允许定时器4更新中断
	TIM_Cmd(TIM4, ENABLE); 										  		// 使能定时器4
}

/*************************************************************************************************************************************
															外部函数区域
*************************************************************************************************************************************/

/**
 * @name	  TIM2_Init
 * @brief     初始化定时器2输出4路PWM
 * @param	  空
 * @return    空
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
 * @brief     初始化定时器3为普通计数模式
 * @param	  空
 * @return    空
 * @DateTime  2019-7-17
 */
void TIM3_Init(void)
{
	TIM3_Mode_Init(6-1, 6000-1);
	NVIC_Configuration3();
}

/**
 * @name	  TIM4_Init
 * @brief     初始化定时器4为普通计数模式
 * @param	  空
 * @return    空
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
 * @brief     初始化定时器2的GPIO输出引脚
 * @param	  空
 * @return    空
 * @DateTime  2019-7-17
 */
void TIM2_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	// 使能GPIO和复用时钟
	RCC_APB2PeriphClockCmd(AF_CLK, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = AF_PIN_0 | AF_PIN_1 |AF_PIN_2 |AF_PIN_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(AF_PORT, &GPIO_InitStructure);	
}


/*************************************************************************************************************************************
															定时器中断服务函数区域
*************************************************************************************************************************************/

// 定时器2中断服务函数
void TIM2_IRQHandler(void)   		// TIM2中断
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

// 定时器3中断服务函数
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)  // 溢出中断
	{
		PS0 = ~PS0;
		PS1 = ~PS1;
		PS2 = ~PS2;
		PS2 = ~PS3;
	}
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);     // 清除中断标志位
}

// 定时器4中断服务函数
void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4, TIM_IT_Update) == SET)  // 溢出中断
	{
		if(ncount < 1000)		// 等待1ms的到来
			ncount++;
		else
		{
			update_flag = 1;
			next_fre = next_fre + fre_step;
			printf("next_fre = %f\r\n", next_fre);
			ncount = 0;
		}
	}
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);     // 清除中断标志位
}

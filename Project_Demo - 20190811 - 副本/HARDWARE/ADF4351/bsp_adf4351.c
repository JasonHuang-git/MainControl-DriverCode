#include <bsp_adf4351.h>

// 分4个8位存储要写入ADF4351寄存器的值
uint8_t buf[4] = {0, 0, 0, 0};

/**
 * @name	  ADF_GPIO_Init
 * @brief     锁相环GPIO初始化
 * @param	  空
 * @return    空
 * @DateTime  2019-7-20
 */
static void ADF_GPIO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	// 使能对应GPIO口时钟
	RCC_AHB1PeriphClockCmd(ADF_GPIO_CLK, ENABLE);
	// PG12 ---> CLK
	// PG13 ---> DATA
	// PG14 ---> CE
	// PG15 ---> LE
	GPIO_InitStructure.GPIO_Pin   = LE_GPIO_PIN | CE_GPIO_PIN | DATA_GPIO_PIN | SCK_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(ADF_GPIO_PORT, &GPIO_InitStructure);

	ADF_CE  = 0;    // 关闭芯片片选
	ADF_LE  = 0;    // 关闭数据加载
	ADF_CLK = 0;    // 拉低串行时钟线
}

/**
 * @name	  ADF_Write
 * @brief     ADF4351写count字节数据函数，先写MSB
 * @param	  count：写入的字节数
			  reg：待写入的数据地址
 * @return    空
 * @DateTime  2019-7-20
 */
static void ADF_Write(uint8_t count, uint8_t * reg)
{
	uint8_t ValueToWrite = 0;
	uint8_t i = 0;
	uint8_t j = 0;

	// 配置数据线方向为为主机输出状态，
	ADF_DATA_WRITE();
	// 使能芯片片选
	ADF_CE = 1;
	delay_us(1);
	ADF_CLK = 0;
	ADF_LE = 0;
	delay_us(1);

	for (i = count; i > 0; i--)
	{
		ValueToWrite = *(reg + i - 1);
		for (j = 0; j < 8; j++)
		{
			if (0x80 == (ValueToWrite & 0x80))
			{
				ADF_DATA_OUT = 1;
			}
			else
			{
				ADF_DATA_OUT = 0;
			}
			delay_us(1);
			ADF_CLK = 1;
			delay_us(1);
			ValueToWrite <<= 1;
			ADF_CLK = 0;
		}
	}
	// 使能数据加载
	ADF_LE = 1;
	delay_us(1);
	ADF_LE = 0;
	delay_us(1);
}

/**
 * @name	  ADF4351_Write_OneReg
 * @brief     ADF4351写一个寄存器
 * @param	  Regster：
			  reg：待写入的数据地址
 * @return    空
 * @DateTime  2019-7-20
 */
static void ADF4351_Write_OneReg(uint32_t Regster)
{
	buf[3] = (uint8_t)((Regster >> 24) & (0X000000FF));
	buf[2] = (uint8_t)((Regster >> 16) & (0X000000FF));
	buf[1] = (uint8_t)((Regster >> 8) & (0X000000FF));
	buf[0] = (uint8_t)((Regster) & (0X000000FF));
	ADF_Write(4, buf);
}

/**
 * @name	  ADF_Init
 * @brief     ADF4351初始化寄存器的值，初始化输出100MHz
 * @param	  空
 * @return    空
 * @DateTime  2019-7-20
 */
void ADF_Init(void)
{
	// 初始化GPIO
	ADF_GPIO_Init();

	buf[3] = 0x00;
	buf[2] = 0x58;
	buf[1] = 0x00;				// write communication register 0x00580005 to control the progress
	buf[0] = 0x05;				// to write Register 5 to set digital lock detector
	ADF_Write(4, buf);

	buf[3] = 0x00;
	buf[2] = 0xDC;				// (DB23=1)The signal is taken from the VCO directly;(DB22-20:4H)the RF divider is 16;(DB19-12:50H)R is 80
	buf[1] = 0x80;				// (DB11=0)VCO powerd up;
	buf[0] = 0x3C;				// (DB5=1)RF output is enabled;(DB4-3=3H)Output power level is +5dBm
	ADF_Write(4, buf);

	buf[3] = 0x00;
	buf[2] = 0x00;
	buf[1] = 0x04;				// (DB14-3:96H)clock divider value is 150.
	buf[0] = 0xB3;
	ADF_Write(4, buf);

	// 默认外部晶振为100MHz，R分频系数为4，f_PFD = 100/4 = 25MHz,如果修改外部晶振的话，需要自行修改R分频器的值
	buf[3] = 0x00;
	buf[2] = 0x01;				// (DB6=1)set PD polarity is positive;(DB7=1)LDP is 6nS;
	buf[1] = 0x1E;				// (DB8=0)enable fractional-N digital lock detect;
	buf[0] = 0xC2;				// (DB12-9:7H)set Icp 5 mA;
	ADF_Write(4, buf);			// (DB23-14:1H)R counter is 1

	buf[3] = 0x00;
	buf[2] = 0x00;
	buf[1] = 0x80;			   	// (DB14-3:6H)MOD counter is 2;
	buf[0] = 0x11;			   	// (DB26-15:6H)PHASE word is 1,neither the phase resync
	ADF_Write(4, buf);	   		// nor the spurious optimization functions are being used
	// (DB27=1)prescaler value is 4/5
	buf[3] = 0x00;
	buf[2] = 0x40;
	buf[1] = 0x00;
	buf[0] = 0x00;				// (DB14-3:0H)FRAC value is 0;
	ADF_Write(4, buf);			// (DB30-15:140H)INT value is 128;

}


/**
 * @name	  ADF_Init
 * @brief     设置输出频率
 * @param	  Fre：需要输出的频率，范围35M~4400MHz，步进0.1MHz
 * @return    空
 * @DateTime  2019-7-20
 */
void ADF4351_WriteFreq(float Fre)		//	(xx.x) M Hz
{
	uint16_t Fre_temp, N_Mul = 1, Mul_Core = 0;
	uint16_t INT_Fre, Frac_temp, Mod_temp, i;
	uint32_t W_ADF4351_R0 = 0, W_ADF4351_R1 = 0, W_ADF4351_R4 = 0;
	float multiple;

	if (Fre < 35.0f)
		Fre = 35.0f;
	if (Fre > 4400.0f)
		Fre = 4400.0f;
	Mod_temp = 1000;
	Fre = ((float)((uint32_t)(Fre * 10.0f))) / 10.0f;

	Fre_temp = Fre;
	// 确定整数分频系数INT的值
	for (i = 0; i < 10; i++)
	{
		if (((Fre_temp * N_Mul) >= 2199.9f) && ((Fre_temp * N_Mul) <= 4400.1))
			break;
		Mul_Core++;
		N_Mul = N_Mul * 2;
	}
	multiple = (Fre * N_Mul) / f_PFD;
	INT_Fre = (uint16_t)multiple;

	// 通过约简来确定小数分频的分子和分母
	Frac_temp = ((uint32_t)(multiple * 1000)) % 1000;
	while (((Frac_temp % 5) == 0) && ((Mod_temp % 5) == 0))
	{
		Frac_temp = Frac_temp / 5;
		Mod_temp = Mod_temp / 5;
	}
	while (((Frac_temp % 2) == 0) && ((Mod_temp % 2) == 0))
	{
		Frac_temp = Frac_temp / 2;
		Mod_temp = Mod_temp / 2;
	}
	// 确定RF Divider的值，1,2,4,8,16,32,64七组取值
	Mul_Core %= 7;
	
	// 转换为待写入的寄存器值
	W_ADF4351_R0 = (INT_Fre << 15) + (Frac_temp << 3);
	W_ADF4351_R1 = ADF4351_R1_Base + (Mod_temp << 3);
	W_ADF4351_R4 = ADF4351_R4_ON + (Mul_Core << 20);

	ADF4351_Write_OneReg(ADF4351_RF_OFF);	// 关闭RF OUT，开始配置PLL
	ADF4351_Write_OneReg(W_ADF4351_R1);		// 配置小数分频因子分母MOD
	ADF4351_Write_OneReg(W_ADF4351_R0); 	// 配置整数分频因子INT和小数分频因子FRAC
	ADF4351_Write_OneReg(W_ADF4351_R4); 	// 配置RF Divider，并使能RF OUT
}

/**
 * @name	  ADF_SweepFre
 * @brief     扫频输出
 * @param	  start_fre：起始频率
			  end_fre：终止频率
			  step：步进频率
			  step_time：步进时间，ms为单位
 * @return    空
 * @DateTime  2019-7-20
 */
float fre_step = 0;		// 扫频步进
float next_fre = 0;		// 记录下一个要写入的频率
uint8_t update_flag = 0;
void ADF_SweepFre(float start_fre, float end_fre, float step, uint16_t step_time)
{
	static uint16_t current_fre;
	// 把起始频率赋值给当前频率
	current_fre = start_fre;
	// 步进值传入定时器4中断服务函数中使用
	fre_step = step;
	next_fre = start_fre;
	// 启动定时器
	TIM4_Init(step_time);
	while(1)
	{
		if(next_fre <= end_fre && update_flag)
		{
			update_flag = 0;
			current_fre = next_fre;
			ADF4351_WriteFreq(current_fre);
		}
		else if(next_fre > end_fre)
		{
			// 关闭定时器
			TIM_DeInit(TIM4);
			printf("扫频结束\r\n");
			break;
		}
	}
}

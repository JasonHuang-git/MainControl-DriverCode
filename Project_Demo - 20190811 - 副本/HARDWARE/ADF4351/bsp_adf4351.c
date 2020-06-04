#include <bsp_adf4351.h>

// ��4��8λ�洢Ҫд��ADF4351�Ĵ�����ֵ
uint8_t buf[4] = {0, 0, 0, 0};

/**
 * @name	  ADF_GPIO_Init
 * @brief     ���໷GPIO��ʼ��
 * @param	  ��
 * @return    ��
 * @DateTime  2019-7-20
 */
static void ADF_GPIO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	// ʹ�ܶ�ӦGPIO��ʱ��
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

	ADF_CE  = 0;    // �ر�оƬƬѡ
	ADF_LE  = 0;    // �ر����ݼ���
	ADF_CLK = 0;    // ���ʹ���ʱ����
}

/**
 * @name	  ADF_Write
 * @brief     ADF4351дcount�ֽ����ݺ�������дMSB
 * @param	  count��д����ֽ���
			  reg����д������ݵ�ַ
 * @return    ��
 * @DateTime  2019-7-20
 */
static void ADF_Write(uint8_t count, uint8_t * reg)
{
	uint8_t ValueToWrite = 0;
	uint8_t i = 0;
	uint8_t j = 0;

	// ���������߷���ΪΪ�������״̬��
	ADF_DATA_WRITE();
	// ʹ��оƬƬѡ
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
	// ʹ�����ݼ���
	ADF_LE = 1;
	delay_us(1);
	ADF_LE = 0;
	delay_us(1);
}

/**
 * @name	  ADF4351_Write_OneReg
 * @brief     ADF4351дһ���Ĵ���
 * @param	  Regster��
			  reg����д������ݵ�ַ
 * @return    ��
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
 * @brief     ADF4351��ʼ���Ĵ�����ֵ����ʼ�����100MHz
 * @param	  ��
 * @return    ��
 * @DateTime  2019-7-20
 */
void ADF_Init(void)
{
	// ��ʼ��GPIO
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

	// Ĭ���ⲿ����Ϊ100MHz��R��Ƶϵ��Ϊ4��f_PFD = 100/4 = 25MHz,����޸��ⲿ����Ļ�����Ҫ�����޸�R��Ƶ����ֵ
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
 * @brief     �������Ƶ��
 * @param	  Fre����Ҫ�����Ƶ�ʣ���Χ35M~4400MHz������0.1MHz
 * @return    ��
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
	// ȷ��������Ƶϵ��INT��ֵ
	for (i = 0; i < 10; i++)
	{
		if (((Fre_temp * N_Mul) >= 2199.9f) && ((Fre_temp * N_Mul) <= 4400.1))
			break;
		Mul_Core++;
		N_Mul = N_Mul * 2;
	}
	multiple = (Fre * N_Mul) / f_PFD;
	INT_Fre = (uint16_t)multiple;

	// ͨ��Լ����ȷ��С����Ƶ�ķ��Ӻͷ�ĸ
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
	// ȷ��RF Divider��ֵ��1,2,4,8,16,32,64����ȡֵ
	Mul_Core %= 7;
	
	// ת��Ϊ��д��ļĴ���ֵ
	W_ADF4351_R0 = (INT_Fre << 15) + (Frac_temp << 3);
	W_ADF4351_R1 = ADF4351_R1_Base + (Mod_temp << 3);
	W_ADF4351_R4 = ADF4351_R4_ON + (Mul_Core << 20);

	ADF4351_Write_OneReg(ADF4351_RF_OFF);	// �ر�RF OUT����ʼ����PLL
	ADF4351_Write_OneReg(W_ADF4351_R1);		// ����С����Ƶ���ӷ�ĸMOD
	ADF4351_Write_OneReg(W_ADF4351_R0); 	// ����������Ƶ����INT��С����Ƶ����FRAC
	ADF4351_Write_OneReg(W_ADF4351_R4); 	// ����RF Divider����ʹ��RF OUT
}

/**
 * @name	  ADF_SweepFre
 * @brief     ɨƵ���
 * @param	  start_fre����ʼƵ��
			  end_fre����ֹƵ��
			  step������Ƶ��
			  step_time������ʱ�䣬msΪ��λ
 * @return    ��
 * @DateTime  2019-7-20
 */
float fre_step = 0;		// ɨƵ����
float next_fre = 0;		// ��¼��һ��Ҫд���Ƶ��
uint8_t update_flag = 0;
void ADF_SweepFre(float start_fre, float end_fre, float step, uint16_t step_time)
{
	static uint16_t current_fre;
	// ����ʼƵ�ʸ�ֵ����ǰƵ��
	current_fre = start_fre;
	// ����ֵ���붨ʱ��4�жϷ�������ʹ��
	fre_step = step;
	next_fre = start_fre;
	// ������ʱ��
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
			// �رն�ʱ��
			TIM_DeInit(TIM4);
			printf("ɨƵ����\r\n");
			break;
		}
	}
}

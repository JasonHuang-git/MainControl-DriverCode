#include <bsp_ad9959.h>

uint8_t CSR_DATA0[1]    = {0x10};                                            // Enable CH0
uint8_t CSR_DATA1[1]    = {0x20};                                            // Enable CH1
uint8_t CSR_DATA2[1]    = {0x40};                                            // Enable CH2
uint8_t CSR_DATA3[1]    = {0x80};                                            // Enable CH3

uint8_t FR2_DATA[2]     = {0x00, 0x00};                                      // default Value = 0x0000
uint8_t CFR_DATA[3]     = {0x00, 0x03, 0x02};                                // default Value = 0x000302
uint8_t CPOW0_DATA[2]   = {0x00, 0x00};                                      // default Value = 0x0000   @ = POW/2^14*360
uint8_t LSRR_DATA[2]    = {0x00, 0x00};                                      // default Value = 0x----
uint8_t RDW_DATA[4]     = {0x00, 0x00, 0x00, 0x00};                          // default Value = 0x--------
uint8_t FDW_DATA[4]     = {0x00, 0x00, 0x00, 0x00};                          // default Value = 0x--------

uint32_t SinFre[4]      = {150000000, 150000000, 150000000, 150000000};
uint32_t SinAmp[4]      = {0x23ff, 0x23ff, 0x23ff, 0x23ff};
uint32_t SinPhr[4]      = {0, 90, 180, 270};


/**
 * @name	  Init_AD9959
 * @brief     ��ʼ��AD9959��ͨ�������Ƶ�ʡ������Լ���λ
 * @param	  ��
 * @return    ��
 * @DateTime  2019-7-19
 */
void Init_AD9959(void)
{
	// REF CLK = 25MHz, Multiplication factor = 20
	// ����ϵͳʱ��Ƶ��
    uint8_t FR1_DATA[3] = {0xD3, 0x00, 0x00};  // 20 frequency doubling
	
    InitIO_9959();
    InitReset();

	// VCO gain control = 1, system clock = 500MHz
    WriteData_AD9959(FR1_ADD, 3, FR1_DATA, 1);
	
	// ���ø�ͨ�����Ƶ��
    Write_frequence(0, SinFre[0]);
    Write_frequence(1, SinFre[1]);
    Write_frequence(2, SinFre[2]);
    Write_frequence(3, SinFre[3]);
	// ���ø�ͨ�������λ
    Write_Phase(0, SinPhr[0]);
    Write_Phase(1, SinPhr[1]);
    Write_Phase(2, SinPhr[2]);
    Write_Phase(3, SinPhr[3]);
	// ���ø�ͨ���������
    Write_Amplitude(0, SinAmp[0]);
    Write_Amplitude(1, SinAmp[1]);
    Write_Amplitude(2, SinAmp[2]);
	Write_Amplitude(3, SinAmp[3]);
}

/**
 * @name	  delay_9959
 * @brief     ��DDSʹ�õ���ʱ����
 * @param	  ��ʱʱ�䳤��(168MHz��Ƶ)
 * @return    ��
 * @DateTime  2019-7-19
 */
void delay_9959(uint32_t length)
{
    length = length * 12;
    while(length--);
}

/**
 * @name	  InitIO_9959
 * @brief     ��ʼ��9959��IO��
 * @param	  ��
 * @return    ��
 * @DateTime  2019-7-19
 */
void InitIO_9959(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(DDS_CLK, ENABLE); // PF/PG port clock enable
/*
	P0     --->	PF0
	P1     --->	PF1
	P2     --->	PF2
	P3     --->	PF3

	SDIO-0 --->	PF4
	SDIO-1 --->	PF5
	SDIO-2 --->	PF6
	SDIO-3 --->	PF7
	UPDATE ---> PF8

	PD_CTL ---> PG8
	RST    ---> PG9
	CS     ---> PG10
	SCLK   ---> PG11
*/
	// GPIOF��ʼ��
	GPIO_InitStructure.GPIO_Pin   = DDS_PS0 | DDS_PS1 | DDS_PS2 | DDS_PS3 | DDS_SDIO0
									| DDS_SDIO1 | DDS_SDIO2 | DDS_SDIO3 | DDS_UPDATE;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(DDS_PORT1, &GPIO_InitStructure);
	
	// GPIOG��ʼ��
	GPIO_InitStructure.GPIO_Pin   = DDS_SCLK_PIN | DDS_CS_PIN | DDS_RESET | DDS_PWR;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(DDS_PORT2, &GPIO_InitStructure);
	
    AD9959_PWR = 0;
    CS = 1;
    SCLK = 0;
    UPDATE = 0;
    PS0 = 0;
    PS1 = 0;
    PS2 = 0;
    PS3 = 0;
    SDIO0 = 0;
    SDIO1 = 0;
    SDIO2 = 0;
    SDIO3 = 0;
}

/**
 * @name	  InitReset
 * @brief     ��λDDS
 * @param	  ��
 * @return    ��
 * @DateTime  2019-7-19
 */
void InitReset(void)
{
    RESET = 0;
    delay_9959(1);
    RESET = 1;
    delay_9959(30);
    RESET = 0;
}

/**
 * @name	  IO_Update
 * @brief     ����DDS
 * @param	  ��
 * @return    ��
 * @DateTime  2019-7-19
 */
void IO_Update(void)
{
    UPDATE = 0;
    delay_9959(4);
    UPDATE = 1;
    delay_9959(4);
    UPDATE = 0;
}

/**
 * @name	  ReadData_9959
 * @brief     ��ȡDDS�Ĵ����е�ֵ
 * @param	  RegisterAddress����Ҫ��ȡ�ļĴ�����ַ
			  NumberofRegisters����Ҫ��ȡ�ļĴ�������
              *RegisterData��������ʼ��ַ
 * @return    ��
 * @DateTime  2019-7-19
 */
void ReadData_9959(uint8_t RegisterAddress, uint8_t NumberofRegisters, uint8_t *RegisterData)
{
    uint8_t ControlValue = 0;
    uint8_t RegisterIndex = 0;
    uint8_t ReceiveData = 0;
    uint8_t i = 0;
    GPIO_InitTypeDef GPIO_InitStructure;
    //Create the 8-bit header
    ControlValue = RegisterAddress;
    SCLK = 1;
    delay_9959(0x20);
    CS = 1;
    delay_9959(0x20);
    CS = 0;
    delay_9959(0x20);

    GPIO_InitStructure.GPIO_Pin =  DDS_SCLK_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_Init(DDS_PORT2, &GPIO_InitStructure);

    delay_9959(0x20);
    //Write out the control word
    for(i = 0; i < 8; i++)
    {
        SCLK = 0;
        if(0x80 == (ControlValue & 0x80))
        {
            SDIO0 = 1;	        //Send one to SDIO pin
        }
        else
        {
            SDIO0 = 0;	        //Send zero to SDIO pin
        }
        delay_9959(0x20);
        SCLK = 1;
        delay_9959(0x20);
        ControlValue <<= 1;	    //Rotate data
    }

    GPIO_InitStructure.GPIO_Pin =  DDS_PS0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(DDS_PORT1, &GPIO_InitStructure);

    delay_9959(0x20);
    for (RegisterIndex = NumberofRegisters; RegisterIndex > 0; RegisterIndex--)
    {
        for(i = 0; i < 8; i++)
        {
            SCLK = 0;
            ReceiveData <<= 1;		// Rotate data
            if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_3))
            {
                ReceiveData |= 1;
            }
            delay_9959(0x20);
            SCLK = 1;
            delay_9959(0x20);
        }
        *(RegisterData + RegisterIndex - 1) = ReceiveData;
    }
    delay_9959(0x20);
    CS = 1;	                        // bring CS high again
}

/**
 * @name	WriteData_AD9959
 * @brief   ������ͨ��SPI��AD9959д����
 * @param	RegisterAddress: �Ĵ�����ַ
			NumberofRegisters: �����ֽ���
			*RegisterData: ������ʼ��ַ
			temp: �Ƿ����IO�Ĵ���
 * @return    ��
 * @DateTime  2019-7-19
 */
void WriteData_AD9959(uint8_t RegisterAddress, uint8_t NumberofRegisters, uint8_t *RegisterData, uint8_t temp)
{
    uint8_t  ControlValue = 0;
    uint8_t  ValueToWrite = 0;
    uint8_t  RegisterIndex = 0;
    uint8_t  i = 0;

    ControlValue = RegisterAddress;
    // Write address
    SCLK = 0;
    CS = 0;
    for(i = 0; i < 8; i++)
    {
        SCLK = 0;
        if(0x80 == (ControlValue & 0x80))
            SDIO0 = 1;
        else
            SDIO0 = 0;
        SCLK = 1;
        ControlValue <<= 1;
        delay_9959(2);
    }

    SCLK = 0;
    // Write data
    for (RegisterIndex = 0; RegisterIndex < NumberofRegisters; RegisterIndex++)
    {
        ValueToWrite = RegisterData[RegisterIndex];
        for (i = 0; i < 8; i++)
        {
            SCLK = 0;
            if(0x80 == (ValueToWrite & 0x80))
                SDIO0 = 1;
            else
                SDIO0 = 0;
            SCLK = 1;
            ValueToWrite <<= 1;
            delay_9959(2);
        }
        SCLK = 0;
    }
    if(temp == 1)
        IO_Update();
    CS = 1;
}

/**
 * @name	Write_frequence
 * @brief   ����ͨ�����Ƶ��
 * @param	Channel:  ���ͨ��
			Freq:     ���Ƶ��
 * @return    ��
 * @DateTime  2019-7-19
 */
void Write_frequence(uint8_t Channel, uint32_t Freq)
{
    uint8_t CFTW0_DATA[4] = {0x00, 0x00, 0x00, 0x00};
    uint32_t Temp;
    Temp = (uint32_t)Freq * 4294967296 / system_clk;    // The input frequency factor is divided into four bytes.  4.294967296=(2^32)/500000000
    CFTW0_DATA[3] = (uint8_t)Temp;
    CFTW0_DATA[2] = (uint8_t)(Temp >> 8);
    CFTW0_DATA[1] = (uint8_t)(Temp >> 16);
    CFTW0_DATA[0] = (uint8_t)(Temp >> 24);
    if(Channel == 0)
    {
        WriteData_AD9959(CSR_ADD, 1, CSR_DATA0, 0); 	// Control register writes to CH0 channel
        WriteData_AD9959(CFTW0_ADD, 4, CFTW0_DATA, 1);  // CTW0 address 0x04.Output CH0 setting frequency
    }
    else if(Channel == 1)
    {
        WriteData_AD9959(CSR_ADD, 1, CSR_DATA1, 0); 	// Control register writes to CH1 channel
        WriteData_AD9959(CFTW0_ADD, 4, CFTW0_DATA, 1); 	// CTW0 address 0x04.Output CH1 setting frequency
    }
    else if(Channel == 2)
    {
        WriteData_AD9959(CSR_ADD, 1, CSR_DATA2, 0);		// Control register writes to CH2 channel
        WriteData_AD9959(CFTW0_ADD, 4, CFTW0_DATA, 1);  // CTW0 address 0x04.Output CH2 setting frequency
    }
    else if(Channel == 3)
    {
        WriteData_AD9959(CSR_ADD, 1, CSR_DATA3, 0); 	// Control register writes to CH3 channel
        WriteData_AD9959(CFTW0_ADD, 4, CFTW0_DATA, 1);  // CTW0 address 0x04.Output CH3 setting frequency
    }
}

/**
 * @name	Write_Amplitude
 * @brief   ����ͨ���������
 * @param	Channel:  ���ͨ��
			Ampli:    �������
 * @return    ��
 * @DateTime  2019-7-19
 */
void Write_Amplitude(uint8_t Channel, uint16_t Ampli)
{
    uint16_t A_temp;                                // =0x23ff;
    uint8_t ACR_DATA[3] = {0x00, 0x00, 0x00};       // default Value = 0x--0000 Rest = 18.91/Iout

    A_temp = Ampli | 0x1000;
    ACR_DATA[2] = (uint8_t)A_temp;                  // Low bit data
    ACR_DATA[1] = (uint8_t)(A_temp >> 8);           // High bit data
    if(Channel == 0)
    {
        WriteData_AD9959(CSR_ADD, 1, CSR_DATA0, 0);
        WriteData_AD9959(ACR_ADD, 3, ACR_DATA, 1);
    }
    else if(Channel == 1)
    {
        WriteData_AD9959(CSR_ADD, 1, CSR_DATA1, 0);
        WriteData_AD9959(ACR_ADD, 3, ACR_DATA, 1);
    }
    else if(Channel == 2)
    {
        WriteData_AD9959(CSR_ADD, 1, CSR_DATA2, 0);
        WriteData_AD9959(ACR_ADD, 3, ACR_DATA, 1);
    }
    else if(Channel == 3)
    {
        WriteData_AD9959(CSR_ADD, 1, CSR_DATA3, 0);
        WriteData_AD9959(ACR_ADD, 3, ACR_DATA, 1);
    }
}

/**
 * @name	  Write_Phase
 * @brief     ����ͨ�������λ
 * @param	  Channel:  ���ͨ��
			  Phase:    �����λ,��Χ��0��~360��(��Ӧ�Ƕȣ�0~2^14)
 * @return    ��
 * @DateTime  2019-7-19
 */
void Write_Phase(uint8_t Channel, uint16_t Phase)
{
    uint16_t POW = 0;
	POW = (uint16_t)Phase * 16384 / 360;
    CPOW0_DATA[1] = (uint8_t)POW;
    CPOW0_DATA[0] = (uint8_t)(POW >> 8);
    if(Channel == 0)
    {
        WriteData_AD9959(CSR_ADD, 1, CSR_DATA0, 0);
        WriteData_AD9959(CPOW0_ADD, 2, CPOW0_DATA, 1);
    }
    else if(Channel == 1)
    {
        WriteData_AD9959(CSR_ADD, 1, CSR_DATA1, 0);
        WriteData_AD9959(CPOW0_ADD, 2, CPOW0_DATA, 1);
    }
    else if(Channel == 2)
    {
        WriteData_AD9959(CSR_ADD, 1, CSR_DATA2, 0);
        WriteData_AD9959(CPOW0_ADD, 2, CPOW0_DATA, 1);
    }
    else if(Channel == 3)
    {
        WriteData_AD9959(CSR_ADD, 1, CSR_DATA3, 0);
        WriteData_AD9959(CPOW0_ADD, 2, CPOW0_DATA, 1);
    }
}

/**
 * @name	  AD9959_Setwavefrequency
 * @brief     �������Ƶ��
 * @param	  f: ���Ƶ��
 * @return    ��
 * @DateTime  2019-7-19
 */
void AD9959_Setwavefrequency (double f)                                 	// �������Ƶ��
{
    uint8_t ChannelFrequencyTuningWord1data[4];
    uint8_t ChannelFunctionRegisterdata[3] = {0x00, 0x23, 0x35};        	// ��Ƶģʽʱѡ���������ҹ��ܣ�sine��
    uint8_t FunctionRegister1data[3] = {0xD0, 0x00, 0x00};

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);             	// ���ù��ܼĴ���
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);    		// ����ͨ�����ܼĴ�����Ŀ����Ϊ��ʵ�������ҹ���
    //��û��Ҫ��ʱ���Բ����øüĴ���ͬ��Ҳ�������������
    // �Ĵ���8��9λ����������ȣ����ĸ���λ
    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord1data);  		// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWord1data, 1); 	// д��Ƶ�ʿ�����
}

/**
 * @name	  AD9959_Setwavefrequency
 * @brief     ����������ε���λ
 * @param	  f: ���Ƶ��
			  p�������λ
 * @return    ��
 * @DateTime  2019-7-19
 */
void AD9959_Setwavephase(double f, int p)                                   // �����λ����ɵ�
{
    uint8_t ChannelPhaseOffsetTuningWorddata[2];
    uint8_t ChannelFrequencyTuningWorddata[4];

    uint8_t ChannelFunctionRegisterdata[3] = {0x00, 0x23, 0x35};      		// ��Ƶģʽʱѡ���������ҹ��ܣ�sine��
    uint8_t FunctionRegister1data[3] = {0xD0, 0x00, 0x00};
    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);             	// ���ù��ܼĴ���

    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);    		// ����ͨ�����ܼĴ�����Ŀ����Ϊ��ʵ�������ҹ���

    WrPhaseOffsetTuningWorddata(p, ChannelPhaseOffsetTuningWorddata); 		// ������λת����
    WriteData_AD9959(CPOW0_ADD, 2, ChannelPhaseOffsetTuningWorddata, 0); 	// д����λ������

    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWorddata);    		// ����Ƶ��ת����
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWorddata, 1);  	// д��Ƶ�ʿ�����
}

/**
 * @name	  AD9959_Setwaveamplitute
 * @brief     ����������εķ���
 * @param	  f: ���Ƶ��
			  a���������
 * @return    ��
 * @DateTime  2019-7-19
 */
void AD9959_Setwaveamplitute(double f, int a)                    			// ������ȿ��Զ������
{
    uint8_t ChannelFrequencyTuningWorddata[4];
    uint8_t ASRAmplituteWordata[3];
    uint8_t AmplitudeControldata[3] = {0xff, 0x17, 0xff};             		// �ֶ������������
    uint8_t ChannelFunctionRegisterdata[3] = {0x00, 0x23, 0x35};      		// ��Ƶģʽʱѡ���������ҹ��ܣ�sine��

    uint8_t FunctionRegister1data[3] = {0xD0, 0x00, 0x00};
    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);            		// ���ù��ܼĴ���
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);   		// ����ͨ�����ܼĴ�����Ŀ����Ϊ��ʵ�������ҹ���
    WrAmplitudeTuningWorddata1(a, AmplitudeControldata, ASRAmplituteWordata);
    WriteData_AD9959(ACR_ADD, 3, ASRAmplituteWordata, 0);           		// ����������ȿ���ģʽ

    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWorddata);  			// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWorddata, 1); 		// д��Ƶ��ת����
}

/*******************************************************************************************/
/*	                          ���׵���                                                     */
/*******************************************************************************************/

void AD9959_SetFremodulation2(double f1, double f2)
{
    uint8_t ChannelFrequencyTuningWord0data[4];
    uint8_t ChannelFrequencyTuningWord1data[4];
    uint8_t ChannelFunctionRegisterdata[3] = {0x80, 0x23, 0x30};      	// ��RU/RD
    uint8_t FunctionRegister1data[3] = {0xD0, 0x00, 0x00};

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);            	// ���ù��ܼĴ���
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);       // ����ͨ�����ܼĴ���

    // ������ʼƵ��S0��0x0A��
    WrFrequencyTuningWorddata(f1, ChannelFrequencyTuningWord0data);     // ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0A, 4, ChannelFrequencyTuningWord0data, 0); 		// д��Ƶ�ʿ�����

    // ������ֹƵ��E0��0x04��
    WrFrequencyTuningWorddata(f2, ChannelFrequencyTuningWord1data);   	// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWord1data, 1); // д��Ƶ�ʿ�����
}

void AD9959_SetPhamodulation2(double f, int p1, int p2)            		// p1��ʼ��λ��p2��ֹ��λ
{
//    uint8_t ChannelFrequencyTuningWord0data[4];
    uint8_t ChannelFrequencyTuningWord1data[4];
    uint8_t ChannelPhaseOffsetTuningWord0data[2];
    uint8_t ChannelPhaseOffsetTuningWord1data[2];

    uint8_t ChannelFunctionRegisterdata[3] = {0xc0, 0x03, 0x34};            // ��λ����ģʽ���ã�����ɨ����ܿ�����
//    uint8_t AmplitudeControldata[3] = {0x00, 0x03, 0xff};                 // ��������ʱ���ã�Ĭ��������
    uint8_t FunctionRegister1data[3] = {0xD0, 0x00, 0x00};
//   uint8_t FunctionRegister1data[3] = {0xD0,0x54,0x00};                   // ��������ʱ����з���RU/RDʱѡ��
    // PO���е�Ƶ��P2���з���RU/RD
//   uint8_t AmplitudeControldata[3] = {0xff,0x1f,0xff};                    // ����RU/RD

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);                 // ���ù��ܼĴ���

    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);           // ����ͨ�����ܼĴ���������λ��������
//    WriteData_AD9959(ACR,3,AmplitudeControldata,0);                    	// ���������RU/RDģʽ��ѡ��,��Ҫ�޸Ĺ��ܼĴ���FR1
    //���õ�ַ0x05��CPOW0�� ����λ

    WrPhaseOffsetTuningWorddata(p1, ChannelPhaseOffsetTuningWord0data); 	// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(CPOW0_ADD, 2, ChannelPhaseOffsetTuningWord0data, 0); 	// д����λ������
    //���õ�ַ0x0a��CW1�� ����λ

    WrPhaseOffsetTuningWorddata(p2, ChannelPhaseOffsetTuningWord1data); 	// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0a, 4, ChannelPhaseOffsetTuningWord1data, 0);     	// д����λ������
    // f=2000000;    //�������Ƶ��
    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord1data);     		// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWord1data, 1);     // д��Ƶ�ʿ�����
}

void AD9959_SetAM2(double f)
{
    uint8_t ChannelFrequencyTuningWord0data[4];
    uint8_t ChannelAmplitudeTuningWorddata[4] = {0x1f, 0x30, 0x00, 0x00}; 	// 72mV
    uint8_t ChannelFunctionRegisterdata[3] = {0x40, 0x03, 0x30};
    uint8_t AmplitudeControldata[3] = {0x00, 0x03, 0xff}; 					// 500mV
    uint8_t FunctionRegister1data[3] = {0xD0, 0x00, 0x00};

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);             	// ���ù��ܼĴ���
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);       	// ����ͨ�����ܼĴ���

    WriteData_AD9959(ACR_ADD, 3, AmplitudeControldata, 0);              	// ���÷��ȴ�С��S1��

    WriteData_AD9959(0x0a, 4, ChannelAmplitudeTuningWorddata, 0);  			// ���÷��ȴ�С��S2��
    // �������Ƶ�ʴ�С
    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);      	// дƵ�ʿ�����
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWord0data, 1);
}

/*******************************************************************************************/
/*	                          �Ľ׵��� CH0/1                                               */
/*******************************************************************************************/

void AD9959_SetFremodulation4(double f1, double f2, double f3, double f4)
{
    uint8_t ChannelFrequencyTuningWord0data[4];
    uint8_t ChannelFrequencyTuningWord1data[4];
    uint8_t ChannelFrequencyTuningWord2data[4];
    uint8_t ChannelFrequencyTuningWord3data[4];
    uint8_t ChannelFunctionRegisterdata[3] = {0x80, 0x23, 0x30};       // ��RU/RD
    uint8_t FunctionRegister1data[3] = {0xD0, 0x01, 0x00};

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);            // ���ù��ܼĴ���
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);      // ����ͨ�����ܼĴ���


    WrFrequencyTuningWorddata(f1, ChannelFrequencyTuningWord0data);    // ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x04, 4, ChannelFrequencyTuningWord0data, 0);     // д��Ƶ�ʿ�����

    WrFrequencyTuningWorddata(f2, ChannelFrequencyTuningWord1data);    // ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0A, 4, ChannelFrequencyTuningWord1data, 0);     // д��Ƶ�ʿ�����

    WrFrequencyTuningWorddata(f3, ChannelFrequencyTuningWord2data);    // ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0B, 4, ChannelFrequencyTuningWord2data, 0);     // д��Ƶ�ʿ�����

    WrFrequencyTuningWorddata(f4, ChannelFrequencyTuningWord3data);    // ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0C, 4, ChannelFrequencyTuningWord3data, 1);     // д��Ƶ�ʿ�����
}

void AD9959_SetPhamodulation4(double f, int p1, int p2, int p3, int p4)
{
    uint8_t ChannelFrequencyTuningWord0data[4];
    uint8_t ChannelPhaseOffsetTuningWord0data[4];
    uint8_t ChannelPhaseOffsetTuningWord1data[4];
    uint8_t ChannelPhaseOffsetTuningWord2data[4];
    uint8_t ChannelPhaseOffsetTuningWord3data[4];

    uint8_t ChannelFunctionRegisterdata[3] = {0xc0, 0x03, 0x30};        	// ��λ����ģʽ���ã�����ɨ����ܿ�����
//    uint8_t AmplitudeControldata[3] = {0x00, 0x03, 0xff};             	// �ļ�����ʱ���ã�Ĭ��������
    uint8_t FunctionRegister1data[3] = {0xD0, 0x01, 0x00};

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);             	// ���ù��ܼĴ���
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);       	// ����ͨ�����ܼĴ���������λ��������

    WrPhaseOffsetTuningWorddata(p1, ChannelPhaseOffsetTuningWord0data); 	// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(CPOW0_ADD, 2, ChannelPhaseOffsetTuningWord0data, 1); 	// д����λ������

    WrPhaseOffsetTuningWorddata(p2, ChannelPhaseOffsetTuningWord1data); 	// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0a, 4, ChannelPhaseOffsetTuningWord1data, 1);    	// д����λ������

    WrPhaseOffsetTuningWorddata(p3, ChannelPhaseOffsetTuningWord2data); 	// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0b, 4, ChannelPhaseOffsetTuningWord2data, 1); 		// д����λ������

    WrPhaseOffsetTuningWorddata(p4, ChannelPhaseOffsetTuningWord3data); 	// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0c, 4, ChannelPhaseOffsetTuningWord3data, 1);    	// д����λ������
    f = 200000;
    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);     		// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWord0data, 1);     // д��Ƶ�ʿ�����
}

void AD9959_SetAM4(double f)
{
    uint8_t ChannelFrequencyTuningWord0data[4];
    uint8_t AmplitudeControldata[3] = {0x00, 0x03, 0xff};
    uint8_t ChannelAmplitudeTuningWord1data[4] = {0xdf, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord2data[4] = {0x8f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord3data[4] = {0x5f, 0x30, 0x00, 0x00};

    uint8_t ChannelFunctionRegisterdata[3] = {0x40, 0x03, 0x30};
    uint8_t FunctionRegister1data[3] = {0xD0, 0x01, 0x00};

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);              	// ���ù��ܼĴ���
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);        	// ����ͨ�����ܼĴ���

    WriteData_AD9959(ACR_ADD, 3, AmplitudeControldata, 0);                  // ���÷��ȴ�С��S1��

    WriteData_AD9959(0x0a, 4, ChannelAmplitudeTuningWord1data, 0); 			// ���÷��ȴ�С��S2��

    WriteData_AD9959(0x0b, 4, ChannelAmplitudeTuningWord2data, 0);

    WriteData_AD9959(0x0c, 4, ChannelAmplitudeTuningWord3data, 0);

    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);     		// дƵ�ʿ�����
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWord0data, 1);
}

/*******************************************************************************************/
/*	                          �Ľ׵��� CH2/3                                               */
/*******************************************************************************************/

void AD9959_SetFremodulation42(double f1, double f2, double f3, double f4)
{
    uint8_t ChannelFrequencyTuningWord0data[4];
    uint8_t ChannelFrequencyTuningWord1data[4];
    uint8_t ChannelFrequencyTuningWord2data[4];
    uint8_t ChannelFrequencyTuningWord3data[4];
    uint8_t ChannelFunctionRegisterdata[3] = {0x80, 0x23, 0x30};      	// ��RU/RD
    uint8_t FunctionRegister1data[3] = {0xD0, 0x51, 0x00};                 
																		   
    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);            	// ���ù��ܼĴ���
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);    	// ����ͨ�����ܼĴ���
																		   
    WrFrequencyTuningWorddata(f1, ChannelFrequencyTuningWord0data);   	// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x04, 4, ChannelFrequencyTuningWord0data, 0); 		// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f2, ChannelFrequencyTuningWord1data);   	// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0A, 4, ChannelFrequencyTuningWord1data, 0); 		// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f3, ChannelFrequencyTuningWord2data);   	// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0B, 4, ChannelFrequencyTuningWord2data, 0); 		// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f4, ChannelFrequencyTuningWord3data);   	// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0C, 4, ChannelFrequencyTuningWord3data, 1); 		// д��Ƶ�ʿ�����
}

void AD9959_SetPhamodulation42(double f, int p1, int p2, int p3, int p4)
{
    uint8_t ChannelFrequencyTuningWord0data[4];
    uint8_t ChannelPhaseOffsetTuningWord0data[4];
    uint8_t ChannelPhaseOffsetTuningWord1data[4];
    uint8_t ChannelPhaseOffsetTuningWord2data[4];
    uint8_t ChannelPhaseOffsetTuningWord3data[4];

    uint8_t ChannelFunctionRegisterdata[3] = {0xc0, 0x03, 0x30};        	// ��λ����ģʽ���ã�����ɨ����ܿ�����
//    uint8_t AmplitudeControldata[3] = {0x00, 0x03, 0xff};             	// �ļ�����ʱ���ã�Ĭ��������
    uint8_t FunctionRegister1data[3] = {0xD0, 0x51, 0x00};                     
																			   
    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);             	// ���ù��ܼĴ���
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);       	// ����ͨ�����ܼĴ���������λ��������
																			   
    WrPhaseOffsetTuningWorddata(p1, ChannelPhaseOffsetTuningWord0data); 	// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(CPOW0_ADD, 2, ChannelPhaseOffsetTuningWord0data, 1); 	// д����λ������
																			   
    WrPhaseOffsetTuningWorddata(p2, ChannelPhaseOffsetTuningWord1data); 	// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0a, 4, ChannelPhaseOffsetTuningWord1data, 1);    	// д����λ������
																			   
    WrPhaseOffsetTuningWorddata(p3, ChannelPhaseOffsetTuningWord2data); 	// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0b, 4, ChannelPhaseOffsetTuningWord2data, 1);		// д����λ������
																			   
    WrPhaseOffsetTuningWorddata(p4, ChannelPhaseOffsetTuningWord3data); 	// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0c, 4, ChannelPhaseOffsetTuningWord3data, 1);    	// д����λ������
    f = 200000;                                                                
    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);     		// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWord0data, 1);     // д��Ƶ�ʿ�����
}

void AD9959_SetAM42(double f)
{
    uint8_t ChannelFrequencyTuningWord0data[4];
    uint8_t AmplitudeControldata[3] = {0x00, 0x03, 0xff};
    uint8_t ChannelAmplitudeTuningWord1data[4] = {0xdf, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord2data[4] = {0x8f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord3data[4] = {0x5f, 0x30, 0x00, 0x00};

    uint8_t ChannelFunctionRegisterdata[3] = {0x40, 0x03, 0x30};
    uint8_t FunctionRegister1data[3] = {0xD0, 0x51, 0x00};

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);              // ���ù��ܼĴ���
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);        // ����ͨ�����ܼĴ���

    WriteData_AD9959(ACR_ADD, 3, AmplitudeControldata, 0);               // ���÷��ȴ�С��S1��

    WriteData_AD9959(0x0a, 4, ChannelAmplitudeTuningWord1data, 0); 		 // ���÷��ȴ�С��S2��

    WriteData_AD9959(0x0b, 4, ChannelAmplitudeTuningWord2data, 0);

    WriteData_AD9959(0x0c, 4, ChannelAmplitudeTuningWord3data, 0);

    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);     	 // дƵ�ʿ�����
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWord0data, 1);
}


/*******************************************************************************************/
/*	                          �˽׵��� CH0                                                 */
/*******************************************************************************************/

void AD9959_SetFremodulation80(double f1, double f2, double f3, double f4, double f5, double f6, double f7, double f8)
{
    uint8_t ChannelFrequencyTuningWord0data[4];
    uint8_t ChannelFrequencyTuningWord1data[4];
    uint8_t ChannelFrequencyTuningWord2data[4];
    uint8_t ChannelFrequencyTuningWord3data[4];
    uint8_t ChannelFrequencyTuningWord4data[4];
    uint8_t ChannelFrequencyTuningWord5data[4];
    uint8_t ChannelFrequencyTuningWord6data[4];
    uint8_t ChannelFrequencyTuningWord7data[4];
    uint8_t ChannelFunctionRegisterdata[3] = {0x80, 0x23, 0x30};      	// ��RU/RD
    uint8_t FunctionRegister1data[3] = {0xD0, 0xc2, 0x00};

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);            	// ���ù��ܼĴ���
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);    	// ����ͨ�����ܼĴ���
																		   
    WrFrequencyTuningWorddata(f1, ChannelFrequencyTuningWord0data);   	// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x04, 4, ChannelFrequencyTuningWord0data, 0); 		// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f2, ChannelFrequencyTuningWord1data);		// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0A, 4, ChannelFrequencyTuningWord1data, 0); 		// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f3, ChannelFrequencyTuningWord2data);		// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0B, 4, ChannelFrequencyTuningWord2data, 0); 		// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f4, ChannelFrequencyTuningWord3data);		// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0C, 4, ChannelFrequencyTuningWord3data, 0); 		// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f5, ChannelFrequencyTuningWord4data);		// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0D, 4, ChannelFrequencyTuningWord4data, 0); 		// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f6, ChannelFrequencyTuningWord5data);		// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0E, 4, ChannelFrequencyTuningWord5data, 0); 		// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f7, ChannelFrequencyTuningWord6data);		// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0F, 4, ChannelFrequencyTuningWord6data, 0); 		// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f8, ChannelFrequencyTuningWord7data);		// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x10, 4, ChannelFrequencyTuningWord7data, 1); 		// д��Ƶ�ʿ�����
}

void AD9959_SetPhamodulation80(double f, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8)
{
    uint8_t ChannelFrequencyTuningWord0data[4];
    uint8_t ChannelPhaseOffsetTuningWord0data[4];
    uint8_t ChannelPhaseOffsetTuningWord1data[4];
    uint8_t ChannelPhaseOffsetTuningWord2data[4];
    uint8_t ChannelPhaseOffsetTuningWord3data[4];
    uint8_t ChannelPhaseOffsetTuningWord4data[4];
    uint8_t ChannelPhaseOffsetTuningWord5data[4];
    uint8_t ChannelPhaseOffsetTuningWord6data[4];
    uint8_t ChannelPhaseOffsetTuningWord7data[4];
 
    uint8_t ChannelFunctionRegisterdata[3] = {0xc0, 0x03, 0x30};           	// ��λ����ģʽ���ã�����ɨ����ܿ�����
//    uint8_t AmplitudeControldata[3] = {0x00, 0x03, 0xff};                	// �˼�����ʱ���ã�Ĭ��������
    uint8_t FunctionRegister1data[3] = {0xD0, 0xc2, 0x00};

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);                	// ���ù��ܼĴ���
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);          	// ����ͨ�����ܼĴ���������λ��������
	
    WrPhaseOffsetTuningWorddata(p1, ChannelPhaseOffsetTuningWord0data);   	// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(CPOW0_ADD, 2, ChannelPhaseOffsetTuningWord0data, 1);  	// д����λ������

    WrPhaseOffsetTuningWorddata(p2, ChannelPhaseOffsetTuningWord1data); 	// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0a, 4, ChannelPhaseOffsetTuningWord1data, 1);    	// д����λ������
																			   
    WrPhaseOffsetTuningWorddata(p3, ChannelPhaseOffsetTuningWord2data); 	// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0b, 4, ChannelPhaseOffsetTuningWord2data, 1); 		// д����λ������
																			   
    WrPhaseOffsetTuningWorddata(p4, ChannelPhaseOffsetTuningWord3data); 	// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0c, 4, ChannelPhaseOffsetTuningWord3data, 1);    	// д����λ������
																			   
    WrPhaseOffsetTuningWorddata(p5, ChannelPhaseOffsetTuningWord4data); 	// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0d, 4, ChannelPhaseOffsetTuningWord4data, 1); 		// д����λ������
																			   
    WrPhaseOffsetTuningWorddata(p6, ChannelPhaseOffsetTuningWord5data); 	// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0e, 4, ChannelPhaseOffsetTuningWord5data, 1);    	// д����λ������
																			   
    WrPhaseOffsetTuningWorddata(p7, ChannelPhaseOffsetTuningWord6data); 	// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0f, 4, ChannelPhaseOffsetTuningWord6data, 1); 		// д����λ������
																			   
    WrPhaseOffsetTuningWorddata(p8, ChannelPhaseOffsetTuningWord7data); 	// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x10, 4, ChannelPhaseOffsetTuningWord7data, 1);    	// д����λ������
    f = 200000;                                                                
    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);     		// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWord0data, 1);     // д��Ƶ�ʿ�����
}

void AD9959_SetAM80(double f)
{
    uint8_t ChannelFrequencyTuningWord0data[4];

    uint8_t AmplitudeControldata[3] = {0x00, 0x03, 0xff};
    uint8_t ChannelAmplitudeTuningWord1data[4] = {0xdf, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord2data[4] = {0xbf, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord3data[4] = {0x9f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord4data[4] = {0x7f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord5data[4] = {0x5f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord6data[4] = {0x3f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord7data[4] = {0x1f, 0x30, 0x00, 0x00};

    uint8_t ChannelFunctionRegisterdata[3] = {0x40, 0x03, 0x30};
    uint8_t FunctionRegister1data[3] = {0xD0, 0xc2, 0x00};

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);              	// ���ù��ܼĴ���
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);        	// ����ͨ�����ܼĴ���

    WriteData_AD9959(ACR_ADD, 3, AmplitudeControldata, 0);                  // ���÷��ȴ�С��S1��

    WriteData_AD9959(0x0a, 4, ChannelAmplitudeTuningWord1data, 0); 			// ���÷��ȴ�С��S2��

    WriteData_AD9959(0x0b, 4, ChannelAmplitudeTuningWord2data, 0);

    WriteData_AD9959(0x0c, 4, ChannelAmplitudeTuningWord3data, 0);

    WriteData_AD9959(0x0d, 4, ChannelAmplitudeTuningWord4data, 0);           // ���÷��ȴ�С��S1��

    WriteData_AD9959(0x0e, 4, ChannelAmplitudeTuningWord5data, 0); 			 // ���÷��ȴ�С��S2��

    WriteData_AD9959(0x0f, 4, ChannelAmplitudeTuningWord6data, 0);

    WriteData_AD9959(0x10, 4, ChannelAmplitudeTuningWord7data, 0);

    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);     		 // дƵ�ʿ�����
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWord0data, 1);
}

/*******************************************************************************************/
/*	                          �˽׵��� CH1                                                 */
/*******************************************************************************************/

void AD9959_SetFremodulation81(double f1, double f2, double f3, double f4, double f5, double f6, double f7, double f8)
{
    uint8_t ChannelFrequencyTuningWord0data[4];
    uint8_t ChannelFrequencyTuningWord1data[4];
    uint8_t ChannelFrequencyTuningWord2data[4];
    uint8_t ChannelFrequencyTuningWord3data[4];
    uint8_t ChannelFrequencyTuningWord4data[4];
    uint8_t ChannelFrequencyTuningWord5data[4];
    uint8_t ChannelFrequencyTuningWord6data[4];
    uint8_t ChannelFrequencyTuningWord7data[4];
    uint8_t ChannelFunctionRegisterdata[3] = {0x80, 0x23, 0x30};      	// ��RU/RD
    uint8_t FunctionRegister1data[3] = {0xD0, 0xd2, 0x00};

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);            	// ���ù��ܼĴ���
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);    	// ����ͨ�����ܼĴ���

    WrFrequencyTuningWorddata(f1, ChannelFrequencyTuningWord0data);   	// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x04, 4, ChannelFrequencyTuningWord0data, 0); 		// д��Ƶ�ʿ�����
		
    WrFrequencyTuningWorddata(f2, ChannelFrequencyTuningWord1data);		// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0A, 4, ChannelFrequencyTuningWord1data, 0); 		// д��Ƶ�ʿ�����
		
    WrFrequencyTuningWorddata(f3, ChannelFrequencyTuningWord2data);		// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0B, 4, ChannelFrequencyTuningWord2data, 0); 		// д��Ƶ�ʿ�����
		
    WrFrequencyTuningWorddata(f4, ChannelFrequencyTuningWord3data);		// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0C, 4, ChannelFrequencyTuningWord3data, 0); 		// д��Ƶ�ʿ�����
		
    WrFrequencyTuningWorddata(f5, ChannelFrequencyTuningWord4data);		// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0D, 4, ChannelFrequencyTuningWord4data, 0); 		// д��Ƶ�ʿ�����
		
    WrFrequencyTuningWorddata(f6, ChannelFrequencyTuningWord5data);		// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0E, 4, ChannelFrequencyTuningWord5data, 0); 		// д��Ƶ�ʿ�����
		
    WrFrequencyTuningWorddata(f7, ChannelFrequencyTuningWord6data);		// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0F, 4, ChannelFrequencyTuningWord6data, 0); 		// д��Ƶ�ʿ�����
		
    WrFrequencyTuningWorddata(f8, ChannelFrequencyTuningWord7data);		// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x10, 4, ChannelFrequencyTuningWord7data, 1); 		// д��Ƶ�ʿ�����
}

void AD9959_SetPhamodulation81(double f, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8)
{
    uint8_t ChannelFrequencyTuningWord0data[4];
    uint8_t ChannelPhaseOffsetTuningWord0data[4];
    uint8_t ChannelPhaseOffsetTuningWord1data[4];
    uint8_t ChannelPhaseOffsetTuningWord2data[4];
    uint8_t ChannelPhaseOffsetTuningWord3data[4];
    uint8_t ChannelPhaseOffsetTuningWord4data[4];
    uint8_t ChannelPhaseOffsetTuningWord5data[4];
    uint8_t ChannelPhaseOffsetTuningWord6data[4];
    uint8_t ChannelPhaseOffsetTuningWord7data[4];

    uint8_t ChannelFunctionRegisterdata[3] = {0xc0, 0x03, 0x30};            // ��λ����ģʽ���ã�����ɨ����ܿ�����
//    uint8_t AmplitudeControldata[3] = {0x00, 0x03, 0xff};                 // �˼�����ʱ���ã�Ĭ��������
    uint8_t FunctionRegister1data[3] = {0xD0, 0xd2, 0x00};

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);                 // ���ù��ܼĴ���
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);           // ����ͨ�����ܼĴ���������λ��������
																			   
    WrPhaseOffsetTuningWorddata(p1, ChannelPhaseOffsetTuningWord0data); 	// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(CPOW0_ADD, 2, ChannelPhaseOffsetTuningWord0data, 1); 	// д����λ������
																			   
    WrPhaseOffsetTuningWorddata(p2, ChannelPhaseOffsetTuningWord1data); 	// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0a, 4, ChannelPhaseOffsetTuningWord1data, 1);    	// д����λ������
																			   
    WrPhaseOffsetTuningWorddata(p3, ChannelPhaseOffsetTuningWord2data); 	// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0b, 4, ChannelPhaseOffsetTuningWord2data, 1); 		// д����λ������
																			   
    WrPhaseOffsetTuningWorddata(p4, ChannelPhaseOffsetTuningWord3data); 	// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0c, 4, ChannelPhaseOffsetTuningWord3data, 1);    	// д����λ������
																			   
    WrPhaseOffsetTuningWorddata(p5, ChannelPhaseOffsetTuningWord4data); 	// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0d, 4, ChannelPhaseOffsetTuningWord4data, 1); 		// д����λ������
																			   
    WrPhaseOffsetTuningWorddata(p6, ChannelPhaseOffsetTuningWord5data); 	// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0e, 4, ChannelPhaseOffsetTuningWord5data, 1);    	// д����λ������
																			   
    WrPhaseOffsetTuningWorddata(p7, ChannelPhaseOffsetTuningWord6data); 	// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0f, 4, ChannelPhaseOffsetTuningWord6data, 1); 		// д����λ������
																			   
    WrPhaseOffsetTuningWorddata(p8, ChannelPhaseOffsetTuningWord7data); 	// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x10, 4, ChannelPhaseOffsetTuningWord7data, 1);    	// д����λ������
    f = 200000;                                                                
    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);     		// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWord0data, 1); 	// д��Ƶ�ʿ�����
}        

void AD9959_SetAM81(double f)
{
    uint8_t ChannelFrequencyTuningWord0data[4];

    uint8_t AmplitudeControldata[3] = {0x00, 0x03, 0xff};
    uint8_t ChannelAmplitudeTuningWord1data[4] = {0xdf, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord2data[4] = {0xbf, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord3data[4] = {0x9f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord4data[4] = {0x7f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord5data[4] = {0x5f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord6data[4] = {0x3f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord7data[4] = {0x1f, 0x30, 0x00, 0x00};

    uint8_t ChannelFunctionRegisterdata[3] = {0x40, 0x03, 0x30};
    uint8_t FunctionRegister1data[3] = {0xD0, 0xd2, 0x00};

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);              	// ���ù��ܼĴ���
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);        	// ����ͨ�����ܼĴ���

    WriteData_AD9959(ACR_ADD, 3, AmplitudeControldata, 0);                  // ���÷��ȴ�С��S1��

    WriteData_AD9959(0x0a, 4, ChannelAmplitudeTuningWord1data, 0); 			// ���÷��ȴ�С��S2��

    WriteData_AD9959(0x0b, 4, ChannelAmplitudeTuningWord2data, 0);

    WriteData_AD9959(0x0c, 4, ChannelAmplitudeTuningWord3data, 0);

    WriteData_AD9959(0x0d, 4, ChannelAmplitudeTuningWord4data, 0);           // ���÷��ȴ�С��S1��

    WriteData_AD9959(0x0e, 4, ChannelAmplitudeTuningWord5data, 0); 			 // ���÷��ȴ�С��S2��

    WriteData_AD9959(0x0f, 4, ChannelAmplitudeTuningWord6data, 0);

    WriteData_AD9959(0x10, 4, ChannelAmplitudeTuningWord7data, 0);

    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);     		 // дƵ�ʿ�����
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWord0data, 1);
}

/*******************************************************************************************/
/*	                          �˽׵��� CH2                                                 */
/*******************************************************************************************/

void AD9959_SetFremodulation82(double f1, double f2, double f3, double f4, double f5, double f6, double f7, double f8)
{
    uint8_t ChannelFrequencyTuningWord0data[4];
    uint8_t ChannelFrequencyTuningWord1data[4];
    uint8_t ChannelFrequencyTuningWord2data[4];
    uint8_t ChannelFrequencyTuningWord3data[4];
    uint8_t ChannelFrequencyTuningWord4data[4];
    uint8_t ChannelFrequencyTuningWord5data[4];
    uint8_t ChannelFrequencyTuningWord6data[4];
    uint8_t ChannelFrequencyTuningWord7data[4];
    uint8_t ChannelFunctionRegisterdata[3] = {0x80, 0x23, 0x30};      	// ��RU/RD
    uint8_t FunctionRegister1data[3] = {0xD0, 0xe2, 0x00};

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);            	// ���ù��ܼĴ���
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);    	// ����ͨ�����ܼĴ���

    WrFrequencyTuningWorddata(f1, ChannelFrequencyTuningWord0data);   	// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x04, 4, ChannelFrequencyTuningWord0data, 0); 		// д��Ƶ�ʿ�����

    WrFrequencyTuningWorddata(f2, ChannelFrequencyTuningWord1data);   	// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0A, 4, ChannelFrequencyTuningWord1data, 0); 		// д��Ƶ�ʿ�����

    WrFrequencyTuningWorddata(f3, ChannelFrequencyTuningWord2data);   	// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0B, 4, ChannelFrequencyTuningWord2data, 0); 		// д��Ƶ�ʿ�����

    WrFrequencyTuningWorddata(f4, ChannelFrequencyTuningWord3data);   	// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0C, 4, ChannelFrequencyTuningWord3data, 0); 		// д��Ƶ�ʿ�����

    WrFrequencyTuningWorddata(f5, ChannelFrequencyTuningWord4data);   	// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0D, 4, ChannelFrequencyTuningWord4data, 0); 		// д��Ƶ�ʿ�����

    WrFrequencyTuningWorddata(f6, ChannelFrequencyTuningWord5data);   	// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0E, 4, ChannelFrequencyTuningWord5data, 0); 		// д��Ƶ�ʿ�����

    WrFrequencyTuningWorddata(f7, ChannelFrequencyTuningWord6data);   	// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0F, 4, ChannelFrequencyTuningWord6data, 0); 		// д��Ƶ�ʿ�����

    WrFrequencyTuningWorddata(f8, ChannelFrequencyTuningWord7data);   	// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x10, 4, ChannelFrequencyTuningWord7data, 1); 		// д��Ƶ�ʿ�����
}

void AD9959_SetPhamodulation82(double f, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8)
{
    uint8_t ChannelFrequencyTuningWord0data[4];
    uint8_t ChannelPhaseOffsetTuningWord0data[4];
    uint8_t ChannelPhaseOffsetTuningWord1data[4];
    uint8_t ChannelPhaseOffsetTuningWord2data[4];
    uint8_t ChannelPhaseOffsetTuningWord3data[4];
    uint8_t ChannelPhaseOffsetTuningWord4data[4];
    uint8_t ChannelPhaseOffsetTuningWord5data[4];
    uint8_t ChannelPhaseOffsetTuningWord6data[4];
    uint8_t ChannelPhaseOffsetTuningWord7data[4];

    uint8_t ChannelFunctionRegisterdata[3] = {0xc0, 0x03, 0x30};           // ��λ����ģʽ���ã�����ɨ����ܿ�����
//    uint8_t AmplitudeControldata[3] = {0x00, 0x03, 0xff};                // �˼�����ʱ���ã�Ĭ��������
    uint8_t FunctionRegister1data[3] = {0xD0, 0xe2, 0x00};

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);                // ���ù��ܼĴ���
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);          // ����ͨ�����ܼĴ���������λ��������

    WrPhaseOffsetTuningWorddata(p1, ChannelPhaseOffsetTuningWord0data);    // ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(CPOW0_ADD, 2, ChannelPhaseOffsetTuningWord0data, 1);  // д����λ������

    WrPhaseOffsetTuningWorddata(p2, ChannelPhaseOffsetTuningWord1data);    // ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0a, 4, ChannelPhaseOffsetTuningWord1data, 1);       // д����λ������

    WrPhaseOffsetTuningWorddata(p3, ChannelPhaseOffsetTuningWord2data);    // ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0b, 4, ChannelPhaseOffsetTuningWord2data, 1);       // д����λ������

    WrPhaseOffsetTuningWorddata(p4, ChannelPhaseOffsetTuningWord3data);    // ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0c, 4, ChannelPhaseOffsetTuningWord3data, 1);       // д����λ������

    WrPhaseOffsetTuningWorddata(p5, ChannelPhaseOffsetTuningWord4data);    // ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0d, 4, ChannelPhaseOffsetTuningWord4data, 1);       // д����λ������

    WrPhaseOffsetTuningWorddata(p6, ChannelPhaseOffsetTuningWord5data);    // ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0e, 4, ChannelPhaseOffsetTuningWord5data, 1);       // д����λ������

    WrPhaseOffsetTuningWorddata(p7, ChannelPhaseOffsetTuningWord6data);    // ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0f, 4, ChannelPhaseOffsetTuningWord6data, 1);       // д����λ������

    WrPhaseOffsetTuningWorddata(p8, ChannelPhaseOffsetTuningWord7data);    // ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x10, 4, ChannelPhaseOffsetTuningWord7data, 1);       // д����λ������
    f = 200000;
    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);         // ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWord0data, 1);    // д��Ƶ�ʿ�����
}

void AD9959_SetAM82(double f)
{
    uint8_t ChannelFrequencyTuningWord0data[4];

    uint8_t AmplitudeControldata[3] = {0x00, 0x03, 0xff};
    uint8_t ChannelAmplitudeTuningWord1data[4] = {0xdf, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord2data[4] = {0xbf, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord3data[4] = {0x9f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord4data[4] = {0x7f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord5data[4] = {0x5f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord6data[4] = {0x3f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord7data[4] = {0x1f, 0x30, 0x00, 0x00};

    uint8_t ChannelFunctionRegisterdata[3] = {0x40, 0x03, 0x30};
    uint8_t FunctionRegister1data[3] = {0xD0, 0xe2, 0x00};

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);              // ���ù��ܼĴ���
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);        // ����ͨ�����ܼĴ���

    WriteData_AD9959(ACR_ADD, 3, AmplitudeControldata, 0);               // ���÷��ȴ�С��S1��

    WriteData_AD9959(0x0a, 4, ChannelAmplitudeTuningWord1data, 0); 		 // ���÷��ȴ�С��S2��

    WriteData_AD9959(0x0b, 4, ChannelAmplitudeTuningWord2data, 0);

    WriteData_AD9959(0x0c, 4, ChannelAmplitudeTuningWord3data, 0);

    WriteData_AD9959(0x0d, 4, ChannelAmplitudeTuningWord4data, 0);       // ���÷��ȴ�С��S1��

    WriteData_AD9959(0x0e, 4, ChannelAmplitudeTuningWord5data, 0); 		 // ���÷��ȴ�С��S2��

    WriteData_AD9959(0x0f, 4, ChannelAmplitudeTuningWord6data, 0);

    WriteData_AD9959(0x10, 4, ChannelAmplitudeTuningWord7data, 0);

    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);     	 // дƵ�ʿ�����
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWord0data, 1);
}

/*******************************************************************************************/
/*	                          �˽׵��� CH3                                                 */
/*******************************************************************************************/
void AD9959_SetFremodulation83(double f1, double f2, double f3, double f4, double f5, double f6, double f7, double f8)
{
    uint8_t ChannelFrequencyTuningWord0data[4];
    uint8_t ChannelFrequencyTuningWord1data[4];
    uint8_t ChannelFrequencyTuningWord2data[4];
    uint8_t ChannelFrequencyTuningWord3data[4];
    uint8_t ChannelFrequencyTuningWord4data[4];
    uint8_t ChannelFrequencyTuningWord5data[4];
    uint8_t ChannelFrequencyTuningWord6data[4];
    uint8_t ChannelFrequencyTuningWord7data[4];
    uint8_t ChannelFunctionRegisterdata[3] = {0x80, 0x23, 0x30};      	// ��RU/RD
    uint8_t FunctionRegister1data[3] = {0xD0, 0xf2, 0x00};

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);            	// ���ù��ܼĴ���
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);    	// ����ͨ�����ܼĴ���

    WrFrequencyTuningWorddata(f1, ChannelFrequencyTuningWord0data);   	// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x04, 4, ChannelFrequencyTuningWord0data, 0); 		// д��Ƶ�ʿ�����
		
    WrFrequencyTuningWorddata(f2, ChannelFrequencyTuningWord1data);		// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0A, 4, ChannelFrequencyTuningWord1data, 0); 		// д��Ƶ�ʿ�����
		
    WrFrequencyTuningWorddata(f3, ChannelFrequencyTuningWord2data);		// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0B, 4, ChannelFrequencyTuningWord2data, 0); 		// д��Ƶ�ʿ�����
		
    WrFrequencyTuningWorddata(f4, ChannelFrequencyTuningWord3data);		// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0C, 4, ChannelFrequencyTuningWord3data, 0); 		// д��Ƶ�ʿ�����
		
    WrFrequencyTuningWorddata(f5, ChannelFrequencyTuningWord4data);		// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0D, 4, ChannelFrequencyTuningWord4data, 0); 		// д��Ƶ�ʿ�����
		
    WrFrequencyTuningWorddata(f6, ChannelFrequencyTuningWord5data);		// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0E, 4, ChannelFrequencyTuningWord5data, 0); 		// д��Ƶ�ʿ�����
		
    WrFrequencyTuningWorddata(f7, ChannelFrequencyTuningWord6data);		// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0F, 4, ChannelFrequencyTuningWord6data, 0); 		// д��Ƶ�ʿ�����
		
    WrFrequencyTuningWorddata(f8, ChannelFrequencyTuningWord7data);		// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x10, 4, ChannelFrequencyTuningWord7data, 1); 		// д��Ƶ�ʿ�����
}

void AD9959_SetPhamodulation83(double f, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8)
{
    uint8_t ChannelFrequencyTuningWord0data[4];
    uint8_t ChannelPhaseOffsetTuningWord0data[4];
    uint8_t ChannelPhaseOffsetTuningWord1data[4];
    uint8_t ChannelPhaseOffsetTuningWord2data[4];
    uint8_t ChannelPhaseOffsetTuningWord3data[4];
    uint8_t ChannelPhaseOffsetTuningWord4data[4];
    uint8_t ChannelPhaseOffsetTuningWord5data[4];
    uint8_t ChannelPhaseOffsetTuningWord6data[4];
    uint8_t ChannelPhaseOffsetTuningWord7data[4];

    uint8_t ChannelFunctionRegisterdata[3] = {0xc0, 0x03, 0x30};            // ��λ����ģʽ���ã�����ɨ����ܿ�����
//    uint8_t AmplitudeControldata[3] = {0x00, 0x03, 0xff};                 // �˼�����ʱ���ã�Ĭ��������
    uint8_t FunctionRegister1data[3] = {0xD0, 0xf2, 0x00};

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);                 // ���ù��ܼĴ���
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);           // ����ͨ�����ܼĴ���������λ��������
																			   
    WrPhaseOffsetTuningWorddata(p1, ChannelPhaseOffsetTuningWord0data); 	// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(CPOW0_ADD, 2, ChannelPhaseOffsetTuningWord0data, 1); 	// д����λ������
																			   
    WrPhaseOffsetTuningWorddata(p2, ChannelPhaseOffsetTuningWord1data); 	// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0a, 4, ChannelPhaseOffsetTuningWord1data, 1);    	// д����λ������
																			   
    WrPhaseOffsetTuningWorddata(p3, ChannelPhaseOffsetTuningWord2data); 	// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0b, 4, ChannelPhaseOffsetTuningWord2data, 1); 		// д����λ������
																			   
    WrPhaseOffsetTuningWorddata(p4, ChannelPhaseOffsetTuningWord3data); 	// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0c, 4, ChannelPhaseOffsetTuningWord3data, 1);    	// д����λ������
																			   
    WrPhaseOffsetTuningWorddata(p5, ChannelPhaseOffsetTuningWord4data); 	// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0d, 4, ChannelPhaseOffsetTuningWord4data, 1); 		// д����λ������
																			   
    WrPhaseOffsetTuningWorddata(p6, ChannelPhaseOffsetTuningWord5data); 	// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0e, 4, ChannelPhaseOffsetTuningWord5data, 1);    	// д����λ������
																			   
    WrPhaseOffsetTuningWorddata(p7, ChannelPhaseOffsetTuningWord6data); 	// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0f, 4, ChannelPhaseOffsetTuningWord6data, 1); 		// д����λ������
																			   
    WrPhaseOffsetTuningWorddata(p8, ChannelPhaseOffsetTuningWord7data); 	// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x10, 4, ChannelPhaseOffsetTuningWord7data, 1);    	// д����λ������
    f = 200000;                                                                
    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);     		// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWord0data, 1);     // д��Ƶ�ʿ�����
}

void AD9959_SetAM83(double f)
{
    uint8_t ChannelFrequencyTuningWord0data[4];

    uint8_t AmplitudeControldata[3] = {0x00, 0x03, 0xff};
    uint8_t ChannelAmplitudeTuningWord1data[4] = {0xdf, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord2data[4] = {0xbf, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord3data[4] = {0x9f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord4data[4] = {0x7f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord5data[4] = {0x5f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord6data[4] = {0x3f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord7data[4] = {0x1f, 0x30, 0x00, 0x00};

    uint8_t ChannelFunctionRegisterdata[3] = {0x40, 0x03, 0x30};
    uint8_t FunctionRegister1data[3] = {0xD0, 0xf2, 0x00};

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);         	// ���ù��ܼĴ���
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);   	// ����ͨ�����ܼĴ���

    WriteData_AD9959(ACR_ADD, 3, AmplitudeControldata, 0);          	// ���÷��ȴ�С��S1��

    WriteData_AD9959(0x0a, 4, ChannelAmplitudeTuningWord1data, 0);  	// ���÷��ȴ�С��S2��

    WriteData_AD9959(0x0b, 4, ChannelAmplitudeTuningWord2data, 0);

    WriteData_AD9959(0x0c, 4, ChannelAmplitudeTuningWord3data, 0);

    WriteData_AD9959(0x0d, 4, ChannelAmplitudeTuningWord4data, 0);  	// ���÷��ȴ�С��S1��

    WriteData_AD9959(0x0e, 4, ChannelAmplitudeTuningWord5data, 0);  	// ���÷��ȴ�С��S2��

    WriteData_AD9959(0x0f, 4, ChannelAmplitudeTuningWord6data, 0);

    WriteData_AD9959(0x10, 4, ChannelAmplitudeTuningWord7data, 0);

    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);   	// дƵ�ʿ�����
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWord0data, 1);
}


/*******************************************************************************************/
/*	                          ʮ���׵��� CH0                                               */
/*******************************************************************************************/
void AD9959_SetFremodulation160(double f1, double f2, double f3, double f4, double f5, double f6, double f7, double f8, double f9, double f10, double f11, double f12, double f13, double f14, double f15, double f16)
{
    uint8_t ChannelFrequencyTuningWord0data[4];
    uint8_t ChannelFrequencyTuningWord1data[4];
    uint8_t ChannelFrequencyTuningWord2data[4];
    uint8_t ChannelFrequencyTuningWord3data[4];
    uint8_t ChannelFrequencyTuningWord4data[4];
    uint8_t ChannelFrequencyTuningWord5data[4];
    uint8_t ChannelFrequencyTuningWord6data[4];
    uint8_t ChannelFrequencyTuningWord7data[4];
    uint8_t ChannelFrequencyTuningWord8data[4];
    uint8_t ChannelFrequencyTuningWord9data[4];
    uint8_t ChannelFrequencyTuningWord10data[4];
    uint8_t ChannelFrequencyTuningWord11data[4];
    uint8_t ChannelFrequencyTuningWord12data[4];
    uint8_t ChannelFrequencyTuningWord13data[4];
    uint8_t ChannelFrequencyTuningWord14data[4];
    uint8_t ChannelFrequencyTuningWord15data[4];
    uint8_t ChannelFunctionRegisterdata[3] = {0x80, 0x23, 0x30};      // ��RU/RD
    uint8_t FunctionRegister1data[3] = {0xD0, 0xc3, 0x00};

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);           // ���ù��ܼĴ���
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);     // ����ͨ�����ܼĴ���

    WrFrequencyTuningWorddata(f1, ChannelFrequencyTuningWord0data);   // ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x04, 4, ChannelFrequencyTuningWord0data, 0); 	  // д��Ƶ�ʿ�����

    WrFrequencyTuningWorddata(f2, ChannelFrequencyTuningWord1data);   // ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0A, 4, ChannelFrequencyTuningWord1data, 0); 	  // д��Ƶ�ʿ�����

    WrFrequencyTuningWorddata(f3, ChannelFrequencyTuningWord2data);   // ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0B, 4, ChannelFrequencyTuningWord2data, 0); 	  // д��Ƶ�ʿ�����

    WrFrequencyTuningWorddata(f4, ChannelFrequencyTuningWord3data);   // ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0C, 4, ChannelFrequencyTuningWord3data, 0); 	  // д��Ƶ�ʿ�����

    WrFrequencyTuningWorddata(f5, ChannelFrequencyTuningWord4data);   // ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0D, 4, ChannelFrequencyTuningWord4data, 0); 	  // д��Ƶ�ʿ�����

    WrFrequencyTuningWorddata(f6, ChannelFrequencyTuningWord5data);   // ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0E, 4, ChannelFrequencyTuningWord5data, 0);    // д��Ƶ�ʿ�����

    WrFrequencyTuningWorddata(f7, ChannelFrequencyTuningWord6data);   // ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0F, 4, ChannelFrequencyTuningWord6data, 0);    // д��Ƶ�ʿ�����

    WrFrequencyTuningWorddata(f8, ChannelFrequencyTuningWord7data);   // ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x10, 4, ChannelFrequencyTuningWord7data, 0);    // д��Ƶ�ʿ�����
 
    WrFrequencyTuningWorddata(f9, ChannelFrequencyTuningWord8data);   // ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x11, 4, ChannelFrequencyTuningWord8data, 0);    // д��Ƶ�ʿ�����

    WrFrequencyTuningWorddata(f10, ChannelFrequencyTuningWord9data);  // ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x12, 4, ChannelFrequencyTuningWord9data, 0);    // д��Ƶ�ʿ�����

    WrFrequencyTuningWorddata(f11, ChannelFrequencyTuningWord10data); // ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x13, 4, ChannelFrequencyTuningWord10data, 0);   // д��Ƶ�ʿ�����

    WrFrequencyTuningWorddata(f12, ChannelFrequencyTuningWord11data); // ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x14, 4, ChannelFrequencyTuningWord11data, 0);   // д��Ƶ�ʿ�����

    WrFrequencyTuningWorddata(f13, ChannelFrequencyTuningWord12data); // ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x15, 4, ChannelFrequencyTuningWord12data, 0);   // д��Ƶ�ʿ�����

    WrFrequencyTuningWorddata(f14, ChannelFrequencyTuningWord13data); // ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x16, 4, ChannelFrequencyTuningWord13data, 0);   // д��Ƶ�ʿ�����

    WrFrequencyTuningWorddata(f15, ChannelFrequencyTuningWord14data); // ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x017, 4, ChannelFrequencyTuningWord14data, 0);  // д��Ƶ�ʿ�����

    WrFrequencyTuningWorddata(f16, ChannelFrequencyTuningWord15data); // ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x18, 4, ChannelFrequencyTuningWord15data, 1);   // д��Ƶ�ʿ�����
}

void AD9959_SetPhamodulation160(double f, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10, int p11, int p12, int p13, int p14, int p15, int p16)
{
    uint8_t ChannelFrequencyTuningWord0data[4];
    uint8_t ChannelPhaseOffsetTuningWord0data[4];
    uint8_t ChannelPhaseOffsetTuningWord1data[4];
    uint8_t ChannelPhaseOffsetTuningWord2data[4];
    uint8_t ChannelPhaseOffsetTuningWord3data[4];
    uint8_t ChannelPhaseOffsetTuningWord4data[4];
    uint8_t ChannelPhaseOffsetTuningWord5data[4];
    uint8_t ChannelPhaseOffsetTuningWord6data[4];
    uint8_t ChannelPhaseOffsetTuningWord7data[4];
    uint8_t ChannelPhaseOffsetTuningWord8data[4];
    uint8_t ChannelPhaseOffsetTuningWord9data[4];
    uint8_t ChannelPhaseOffsetTuningWord10data[4];
    uint8_t ChannelPhaseOffsetTuningWord11data[4];
    uint8_t ChannelPhaseOffsetTuningWord12data[4];
    uint8_t ChannelPhaseOffsetTuningWord13data[4];
    uint8_t ChannelPhaseOffsetTuningWord14data[4];
    uint8_t ChannelPhaseOffsetTuningWord15data[4];

    uint8_t ChannelFunctionRegisterdata[3] = {0xc0, 0x03, 0x30};                 	// ��λ����ģʽ���ã�����ɨ����ܿ�����
//    uint8_t AmplitudeControldata[3] = {0x00, 0x03, 0xff};                      	// ʮ��������ʱ���ã�Ĭ��������
    uint8_t FunctionRegister1data[3] = {0xD0, 0xc3, 0x00};

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);                   		// ���ù��ܼĴ���
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);           		// ����ͨ�����ܼĴ���������λ��������

    WrPhaseOffsetTuningWorddata(p1, ChannelPhaseOffsetTuningWord0data); 			// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(CPOW0_ADD, 2, ChannelPhaseOffsetTuningWord0data, 1); 			// д����λ������

    WrPhaseOffsetTuningWorddata(p2, ChannelPhaseOffsetTuningWord1data); 			// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0a, 4, ChannelPhaseOffsetTuningWord1data, 1);    			// д����λ������

    WrPhaseOffsetTuningWorddata(p3, ChannelPhaseOffsetTuningWord2data); 			// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0b, 4, ChannelPhaseOffsetTuningWord2data, 1); 				// д����λ������

    WrPhaseOffsetTuningWorddata(p4, ChannelPhaseOffsetTuningWord3data); 			// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0c, 4, ChannelPhaseOffsetTuningWord3data, 1);    			// д����λ������

    WrPhaseOffsetTuningWorddata(p5, ChannelPhaseOffsetTuningWord4data); 			// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0d, 4, ChannelPhaseOffsetTuningWord4data, 1); 				// д����λ������

    WrPhaseOffsetTuningWorddata(p6, ChannelPhaseOffsetTuningWord5data); 			// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0e, 4, ChannelPhaseOffsetTuningWord5data, 1);    			// д����λ������

    WrPhaseOffsetTuningWorddata(p7, ChannelPhaseOffsetTuningWord6data); 			// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0f, 4, ChannelPhaseOffsetTuningWord6data, 1); 				// д����λ������

    WrPhaseOffsetTuningWorddata(p8, ChannelPhaseOffsetTuningWord7data); 			// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x10, 4, ChannelPhaseOffsetTuningWord7data, 1);    			// д����λ������

    WrPhaseOffsetTuningWorddata(p9, ChannelPhaseOffsetTuningWord8data); 			// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x11, 4, ChannelPhaseOffsetTuningWord8data, 1); 				// д����λ������

    WrPhaseOffsetTuningWorddata(p10, ChannelPhaseOffsetTuningWord9data); 			// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x12, 4, ChannelPhaseOffsetTuningWord9data, 1);    			// д����λ������

    WrPhaseOffsetTuningWorddata(p11, ChannelPhaseOffsetTuningWord10data); 			// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x13, 4, ChannelPhaseOffsetTuningWord10data, 1); 				// д����λ������

    WrPhaseOffsetTuningWorddata(p12, ChannelPhaseOffsetTuningWord11data); 			// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x14, 4, ChannelPhaseOffsetTuningWord11data, 1);   			// д����λ������

    WrPhaseOffsetTuningWorddata(p13, ChannelPhaseOffsetTuningWord12data); 			// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x15, 4, ChannelPhaseOffsetTuningWord12data, 1); 				// д����λ������

    WrPhaseOffsetTuningWorddata(p14, ChannelPhaseOffsetTuningWord13data); 			// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x16, 4, ChannelPhaseOffsetTuningWord13data, 1);   			// д����λ������

    WrPhaseOffsetTuningWorddata(p15, ChannelPhaseOffsetTuningWord14data); 			// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x17, 4, ChannelPhaseOffsetTuningWord14data, 1); 				// д����λ������

    WrPhaseOffsetTuningWorddata(p16, ChannelPhaseOffsetTuningWord15data); 			// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x18, 4, ChannelPhaseOffsetTuningWord15data, 1);   			// д����λ������

    f = 200000;
    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);     				// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWord0data, 1);     		// д��Ƶ�ʿ�����
}

void AD9959_SetAM160(double f)
{
    uint8_t ChannelFrequencyTuningWord0data[4];

    uint8_t AmplitudeControldata[3] = {0x00, 0x03, 0xff};
    uint8_t ChannelAmplitudeTuningWord1data[4] = {0xef, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord2data[4] = {0xdf, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord3data[4] = {0xcf, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord4data[4] = {0xbf, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord5data[4] = {0xaf, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord6data[4] = {0x9f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord7data[4] = {0x8f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord8data[4] = {0x7f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord9data[4] = {0x6f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord10data[4] = {0x5f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord11data[4] = {0x4f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord12data[4] = {0x3f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord13data[4] = {0x2f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord14data[4] = {0x1f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord15data[4] = {0x0f, 0x30, 0x00, 0x00};
    uint8_t ChannelFunctionRegisterdata[3] = {0x40, 0x03, 0x30};
    uint8_t FunctionRegister1data[3] = {0xD0, 0xc3, 0x00};

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);               // ���ù��ܼĴ���
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);         // ����ͨ�����ܼĴ���

    WriteData_AD9959(ACR_ADD, 3, AmplitudeControldata, 0);                // ���÷��ȴ�С��S1��

    WriteData_AD9959(0x0a, 4, ChannelAmplitudeTuningWord1data, 0); 		  // ���÷��ȴ�С��S2��

    WriteData_AD9959(0x0b, 4, ChannelAmplitudeTuningWord2data, 0);

    WriteData_AD9959(0x0c, 4, ChannelAmplitudeTuningWord3data, 0);

    WriteData_AD9959(0x0d, 4, ChannelAmplitudeTuningWord4data, 0);        // ���÷��ȴ�С��S1��

    WriteData_AD9959(0x0e, 4, ChannelAmplitudeTuningWord5data, 0); 		  // ���÷��ȴ�С��S2��

    WriteData_AD9959(0x0f, 4, ChannelAmplitudeTuningWord6data, 0);

    WriteData_AD9959(0x10, 4, ChannelAmplitudeTuningWord7data, 0);

    WriteData_AD9959(0x11, 4, ChannelAmplitudeTuningWord8data, 0);        // ���÷��ȴ�С��S1��

    WriteData_AD9959(0x12, 4, ChannelAmplitudeTuningWord9data, 0); 		  // ���÷��ȴ�С��S2��

    WriteData_AD9959(0x13, 4, ChannelAmplitudeTuningWord10data, 0);

    WriteData_AD9959(0x14, 4, ChannelAmplitudeTuningWord11data, 0);

    WriteData_AD9959(0x15, 4, ChannelAmplitudeTuningWord12data, 0);        // ���÷��ȴ�С��S1��

    WriteData_AD9959(0x16, 4, ChannelAmplitudeTuningWord13data, 0); 	   // ���÷��ȴ�С��S2��

    WriteData_AD9959(0x17, 4, ChannelAmplitudeTuningWord14data, 0);

    WriteData_AD9959(0x18, 4, ChannelAmplitudeTuningWord15data, 0);

    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);     	   // дƵ�ʿ�����
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWord0data, 1);
}

/*******************************************************************************************/
/*	                          ʮ���׵��� CH1                                               */
/*******************************************************************************************/
void AD9959_SetFremodulation161(double f1, double f2, double f3, double f4, double f5, double f6, double f7, double f8, double f9, double f10, double f11, double f12, double f13, double f14, double f15, double f16)
{
    uint8_t ChannelFrequencyTuningWord0data[4];
    uint8_t ChannelFrequencyTuningWord1data[4];
    uint8_t ChannelFrequencyTuningWord2data[4];
    uint8_t ChannelFrequencyTuningWord3data[4];
    uint8_t ChannelFrequencyTuningWord4data[4];
    uint8_t ChannelFrequencyTuningWord5data[4];
    uint8_t ChannelFrequencyTuningWord6data[4];
    uint8_t ChannelFrequencyTuningWord7data[4];
    uint8_t ChannelFrequencyTuningWord8data[4];
    uint8_t ChannelFrequencyTuningWord9data[4];
    uint8_t ChannelFrequencyTuningWord10data[4];
    uint8_t ChannelFrequencyTuningWord11data[4];
    uint8_t ChannelFrequencyTuningWord12data[4];
    uint8_t ChannelFrequencyTuningWord13data[4];
    uint8_t ChannelFrequencyTuningWord14data[4];
    uint8_t ChannelFrequencyTuningWord15data[4];                           
    uint8_t ChannelFunctionRegisterdata[3] = {0x80, 0x23, 0x30};      	// ��RU/RD
    uint8_t FunctionRegister1data[3] = {0xD0, 0xd3, 0x00};                 
																		   
    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);            	// ���ù��ܼĴ���
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);    	// ����ͨ�����ܼĴ���
																		   
    WrFrequencyTuningWorddata(f1, ChannelFrequencyTuningWord0data);   	// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x04, 4, ChannelFrequencyTuningWord0data, 0); 		// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f2, ChannelFrequencyTuningWord1data);   	// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0A, 4, ChannelFrequencyTuningWord1data, 0); 		// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f3, ChannelFrequencyTuningWord2data);   	// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0B, 4, ChannelFrequencyTuningWord2data, 0); 		// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f4, ChannelFrequencyTuningWord3data);   	// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0C, 4, ChannelFrequencyTuningWord3data, 0); 		// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f5, ChannelFrequencyTuningWord4data);   	// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0D, 4, ChannelFrequencyTuningWord4data, 0); 		// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f6, ChannelFrequencyTuningWord5data);   	// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0E, 4, ChannelFrequencyTuningWord5data, 0); 		// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f7, ChannelFrequencyTuningWord6data);   	// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0F, 4, ChannelFrequencyTuningWord6data, 0); 		// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f8, ChannelFrequencyTuningWord7data);   	// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x10, 4, ChannelFrequencyTuningWord7data, 0); 		// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f9, ChannelFrequencyTuningWord8data);   	// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x11, 4, ChannelFrequencyTuningWord8data, 0); 		// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f10, ChannelFrequencyTuningWord9data);   	// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x12, 4, ChannelFrequencyTuningWord9data, 0);	 	// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f11, ChannelFrequencyTuningWord10data);   // ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x13, 4, ChannelFrequencyTuningWord10data, 0); 	// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f12, ChannelFrequencyTuningWord11data);   // ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x14, 4, ChannelFrequencyTuningWord11data, 0); 	// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f13, ChannelFrequencyTuningWord12data);   // ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x15, 4, ChannelFrequencyTuningWord12data, 0); 	// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f14, ChannelFrequencyTuningWord13data);   // ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x16, 4, ChannelFrequencyTuningWord13data, 0); 	// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f15, ChannelFrequencyTuningWord14data);   // ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x017, 4, ChannelFrequencyTuningWord14data, 0); 	// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f16, ChannelFrequencyTuningWord15data);   // ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x18, 4, ChannelFrequencyTuningWord15data, 1); 	// д��Ƶ�ʿ�����
}   

void AD9959_SetPhamodulation161(double f, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10, int p11, int p12, int p13, int p14, int p15, int p16)
{
    uint8_t ChannelFrequencyTuningWord0data[4];
    uint8_t ChannelPhaseOffsetTuningWord0data[4];
    uint8_t ChannelPhaseOffsetTuningWord1data[4];
    uint8_t ChannelPhaseOffsetTuningWord2data[4];
    uint8_t ChannelPhaseOffsetTuningWord3data[4];
    uint8_t ChannelPhaseOffsetTuningWord4data[4];
    uint8_t ChannelPhaseOffsetTuningWord5data[4];
    uint8_t ChannelPhaseOffsetTuningWord6data[4];
    uint8_t ChannelPhaseOffsetTuningWord7data[4];
    uint8_t ChannelPhaseOffsetTuningWord8data[4];
    uint8_t ChannelPhaseOffsetTuningWord9data[4];
    uint8_t ChannelPhaseOffsetTuningWord10data[4];
    uint8_t ChannelPhaseOffsetTuningWord11data[4];
    uint8_t ChannelPhaseOffsetTuningWord12data[4];
    uint8_t ChannelPhaseOffsetTuningWord13data[4];
    uint8_t ChannelPhaseOffsetTuningWord14data[4];
    uint8_t ChannelPhaseOffsetTuningWord15data[4];

    uint8_t ChannelFunctionRegisterdata[3] = {0xc0, 0x03, 0x30};                // ��λ����ģʽ���ã�����ɨ����ܿ�����
//    uint8_t AmplitudeControldata[3] = {0x00, 0x03, 0xff};                     // ʮ��������ʱ���ã�Ĭ��������
    uint8_t FunctionRegister1data[3] = {0xD0, 0xd3, 0x00};

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);                  	// ���ù��ܼĴ���
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);           	// ����ͨ�����ܼĴ���������λ��������

    WrPhaseOffsetTuningWorddata(p1, ChannelPhaseOffsetTuningWord0data); 		// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(CPOW0_ADD, 2, ChannelPhaseOffsetTuningWord0data, 1); 		// д����λ������
																				   
    WrPhaseOffsetTuningWorddata(p2, ChannelPhaseOffsetTuningWord1data); 		// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0a, 4, ChannelPhaseOffsetTuningWord1data, 1);    		// д����λ������
																				   
    WrPhaseOffsetTuningWorddata(p3, ChannelPhaseOffsetTuningWord2data); 		// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0b, 4, ChannelPhaseOffsetTuningWord2data, 1); 			// д����λ������
																				  
    WrPhaseOffsetTuningWorddata(p4, ChannelPhaseOffsetTuningWord3data); 		// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0c, 4, ChannelPhaseOffsetTuningWord3data, 1);   			// д����λ������
																				   
    WrPhaseOffsetTuningWorddata(p5, ChannelPhaseOffsetTuningWord4data); 		// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0d, 4, ChannelPhaseOffsetTuningWord4data, 1); 			// д����λ������
																				   
    WrPhaseOffsetTuningWorddata(p6, ChannelPhaseOffsetTuningWord5data); 		// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0e, 4, ChannelPhaseOffsetTuningWord5data, 1);    		// д����λ������
																				   
    WrPhaseOffsetTuningWorddata(p7, ChannelPhaseOffsetTuningWord6data); 		// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0f, 4, ChannelPhaseOffsetTuningWord6data, 1); 			// д����λ������
																				   
    WrPhaseOffsetTuningWorddata(p8, ChannelPhaseOffsetTuningWord7data); 		// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x10, 4, ChannelPhaseOffsetTuningWord7data, 1);    		// д����λ������
																				   
    WrPhaseOffsetTuningWorddata(p9, ChannelPhaseOffsetTuningWord8data); 		// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x11, 4, ChannelPhaseOffsetTuningWord8data, 1); 			// д����λ������
																				   
    WrPhaseOffsetTuningWorddata(p10, ChannelPhaseOffsetTuningWord9data); 		// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x12, 4, ChannelPhaseOffsetTuningWord9data, 1);    		// д����λ������
																				   
    WrPhaseOffsetTuningWorddata(p11, ChannelPhaseOffsetTuningWord10data); 		// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x13, 4, ChannelPhaseOffsetTuningWord10data, 1); 			// д����λ������
																				   
    WrPhaseOffsetTuningWorddata(p12, ChannelPhaseOffsetTuningWord11data); 		// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x14, 4, ChannelPhaseOffsetTuningWord11data, 1);   		// д����λ������
																				   
    WrPhaseOffsetTuningWorddata(p13, ChannelPhaseOffsetTuningWord12data); 		// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x15, 4, ChannelPhaseOffsetTuningWord12data, 1); 			// д����λ������
																				   
    WrPhaseOffsetTuningWorddata(p14, ChannelPhaseOffsetTuningWord13data); 		// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x16, 4, ChannelPhaseOffsetTuningWord13data, 1);   		// д����λ������
																				   
    WrPhaseOffsetTuningWorddata(p15, ChannelPhaseOffsetTuningWord14data); 		// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x17, 4, ChannelPhaseOffsetTuningWord14data, 1); 			// д����λ������
																				   
    WrPhaseOffsetTuningWorddata(p16, ChannelPhaseOffsetTuningWord15data); 		// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x18, 4, ChannelPhaseOffsetTuningWord15data, 1);   		// д����λ������
																				   
    f = 200000;                                                                    
    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);     			// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWord0data, 1);     	// д��Ƶ�ʿ�����
}

void AD9959_SetAM161(double f)
{

    uint8_t ChannelFrequencyTuningWord0data[4];

    uint8_t AmplitudeControldata[3] = {0x00, 0x03, 0xff};
    uint8_t ChannelAmplitudeTuningWord1data[4] = {0xef, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord2data[4] = {0xdf, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord3data[4] = {0xcf, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord4data[4] = {0xbf, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord5data[4] = {0xaf, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord6data[4] = {0x9f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord7data[4] = {0x8f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord8data[4] = {0x7f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord9data[4] = {0x6f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord10data[4] = {0x5f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord11data[4] = {0x4f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord12data[4] = {0x3f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord13data[4] = {0x2f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord14data[4] = {0x1f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord15data[4] = {0x0f, 0x30, 0x00, 0x00};
    uint8_t ChannelFunctionRegisterdata[3] = {0x40, 0x03, 0x30};
    uint8_t FunctionRegister1data[3] = {0xD0, 0xd3, 0x00};

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);              			// ���ù��ܼĴ���
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);        			// ����ͨ�����ܼĴ���

    WriteData_AD9959(ACR_ADD, 3, AmplitudeControldata, 0);                      	//���÷��ȴ�С��S1��

    WriteData_AD9959(0x0a, 4, ChannelAmplitudeTuningWord1data, 0); 					// ���÷��ȴ�С��S2��

    WriteData_AD9959(0x0b, 4, ChannelAmplitudeTuningWord2data, 0);

    WriteData_AD9959(0x0c, 4, ChannelAmplitudeTuningWord3data, 0);

    WriteData_AD9959(0x0d, 4, ChannelAmplitudeTuningWord4data, 0);                  // ���÷��ȴ�С��S1��

    WriteData_AD9959(0x0e, 4, ChannelAmplitudeTuningWord5data, 0); 					// ���÷��ȴ�С��S2��

    WriteData_AD9959(0x0f, 4, ChannelAmplitudeTuningWord6data, 0);

    WriteData_AD9959(0x10, 4, ChannelAmplitudeTuningWord7data, 0);

    WriteData_AD9959(0x11, 4, ChannelAmplitudeTuningWord8data, 0);                  // ���÷��ȴ�С��S1��

    WriteData_AD9959(0x12, 4, ChannelAmplitudeTuningWord9data, 0); 					// ���÷��ȴ�С��S2��

    WriteData_AD9959(0x13, 4, ChannelAmplitudeTuningWord10data, 0);

    WriteData_AD9959(0x14, 4, ChannelAmplitudeTuningWord11data, 0);

    WriteData_AD9959(0x15, 4, ChannelAmplitudeTuningWord12data, 0);                  // ���÷��ȴ�С��S1��

    WriteData_AD9959(0x16, 4, ChannelAmplitudeTuningWord13data, 0); 				 // ���÷��ȴ�С��S2��

    WriteData_AD9959(0x17, 4, ChannelAmplitudeTuningWord14data, 0);

    WriteData_AD9959(0x18, 4, ChannelAmplitudeTuningWord15data, 0);

    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);     				 // дƵ�ʿ�����
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWord0data, 1);
}

/*******************************************************************************************/
/*	                          ʮ���׵��� CH2                                               */
/*******************************************************************************************/
void AD9959_SetFremodulation162(double f1, double f2, double f3, double f4, double f5, double f6, double f7, double f8, double f9, double f10, double f11, double f12, double f13, double f14, double f15, double f16)
{
    uint8_t ChannelFrequencyTuningWord0data[4];
    uint8_t ChannelFrequencyTuningWord1data[4];
    uint8_t ChannelFrequencyTuningWord2data[4];
    uint8_t ChannelFrequencyTuningWord3data[4];
    uint8_t ChannelFrequencyTuningWord4data[4];
    uint8_t ChannelFrequencyTuningWord5data[4];
    uint8_t ChannelFrequencyTuningWord6data[4];
    uint8_t ChannelFrequencyTuningWord7data[4];
    uint8_t ChannelFrequencyTuningWord8data[4];
    uint8_t ChannelFrequencyTuningWord9data[4];
    uint8_t ChannelFrequencyTuningWord10data[4];
    uint8_t ChannelFrequencyTuningWord11data[4];
    uint8_t ChannelFrequencyTuningWord12data[4];
    uint8_t ChannelFrequencyTuningWord13data[4];
    uint8_t ChannelFrequencyTuningWord14data[4];
    uint8_t ChannelFrequencyTuningWord15data[4];
    uint8_t ChannelFunctionRegisterdata[3] = {0x80, 0x23, 0x30};      	// ��RU/RD
    uint8_t FunctionRegister1data[3] = {0xD0, 0xe3, 0x00};

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);            	// ���ù��ܼĴ���
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);    	// ����ͨ�����ܼĴ���
																		   
    WrFrequencyTuningWorddata(f1, ChannelFrequencyTuningWord0data);   	// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x04, 4, ChannelFrequencyTuningWord0data, 0); 		// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f2, ChannelFrequencyTuningWord1data);   	// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0A, 4, ChannelFrequencyTuningWord1data, 0); 		// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f3, ChannelFrequencyTuningWord2data);   	// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0B, 4, ChannelFrequencyTuningWord2data, 0); 		// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f4, ChannelFrequencyTuningWord3data);   	// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0C, 4, ChannelFrequencyTuningWord3data, 0); 		// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f5, ChannelFrequencyTuningWord4data);   	// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0D, 4, ChannelFrequencyTuningWord4data, 0); 		// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f6, ChannelFrequencyTuningWord5data);   	// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0E, 4, ChannelFrequencyTuningWord5data, 0); 		// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f7, ChannelFrequencyTuningWord6data);   	// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0F, 4, ChannelFrequencyTuningWord6data, 0); 		// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f8, ChannelFrequencyTuningWord7data);   	// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x10, 4, ChannelFrequencyTuningWord7data, 0); 		// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f9, ChannelFrequencyTuningWord8data);   	// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x11, 4, ChannelFrequencyTuningWord8data, 0); 		// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f10, ChannelFrequencyTuningWord9data);   	// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x12, 4, ChannelFrequencyTuningWord9data, 0); 		// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f11, ChannelFrequencyTuningWord10data);   // ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x13, 4, ChannelFrequencyTuningWord10data, 0); 	// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f12, ChannelFrequencyTuningWord11data);   // ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x14, 4, ChannelFrequencyTuningWord11data, 0); 	// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f13, ChannelFrequencyTuningWord12data);   // ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x15, 4, ChannelFrequencyTuningWord12data, 0); 	// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f14, ChannelFrequencyTuningWord13data);   // ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x16, 4, ChannelFrequencyTuningWord13data, 0); 	// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f15, ChannelFrequencyTuningWord14data);   // ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x017, 4, ChannelFrequencyTuningWord14data, 0); 	// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f16, ChannelFrequencyTuningWord15data);   // ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x18, 4, ChannelFrequencyTuningWord15data, 1); 	// д��Ƶ�ʿ�����
}

void AD9959_SetPhamodulation162(double f, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10, int p11, int p12, int p13, int p14, int p15, int p16)
{
    uint8_t ChannelFrequencyTuningWord0data[4];
    uint8_t ChannelPhaseOffsetTuningWord0data[4];
    uint8_t ChannelPhaseOffsetTuningWord1data[4];
    uint8_t ChannelPhaseOffsetTuningWord2data[4];
    uint8_t ChannelPhaseOffsetTuningWord3data[4];
    uint8_t ChannelPhaseOffsetTuningWord4data[4];
    uint8_t ChannelPhaseOffsetTuningWord5data[4];
    uint8_t ChannelPhaseOffsetTuningWord6data[4];
    uint8_t ChannelPhaseOffsetTuningWord7data[4];
    uint8_t ChannelPhaseOffsetTuningWord8data[4];
    uint8_t ChannelPhaseOffsetTuningWord9data[4];
    uint8_t ChannelPhaseOffsetTuningWord10data[4];
    uint8_t ChannelPhaseOffsetTuningWord11data[4];
    uint8_t ChannelPhaseOffsetTuningWord12data[4];
    uint8_t ChannelPhaseOffsetTuningWord13data[4];
    uint8_t ChannelPhaseOffsetTuningWord14data[4];
    uint8_t ChannelPhaseOffsetTuningWord15data[4];
 
    uint8_t ChannelFunctionRegisterdata[3] = {0xc0, 0x03, 0x30};                 	// ��λ����ģʽ���ã�����ɨ����ܿ�����
//    uint8_t AmplitudeControldata[3] = {0x00, 0x03, 0xff};                         // ʮ��������ʱ���ã�Ĭ��������
    uint8_t FunctionRegister1data[3] = {0xD0, 0xe3, 0x00};

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);                   		// ���ù��ܼĴ���
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);           		// ����ͨ�����ܼĴ���������λ��������

    WrPhaseOffsetTuningWorddata(p1, ChannelPhaseOffsetTuningWord0data); 			// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(CPOW0_ADD, 2, ChannelPhaseOffsetTuningWord0data, 1); 			// д����λ������

    WrPhaseOffsetTuningWorddata(p2, ChannelPhaseOffsetTuningWord1data); 			// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0a, 4, ChannelPhaseOffsetTuningWord1data, 1);    			// д����λ������

    WrPhaseOffsetTuningWorddata(p3, ChannelPhaseOffsetTuningWord2data); 			// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0b, 4, ChannelPhaseOffsetTuningWord2data, 1);	 			// д����λ������

    WrPhaseOffsetTuningWorddata(p4, ChannelPhaseOffsetTuningWord3data); 			// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0c, 4, ChannelPhaseOffsetTuningWord3data, 1);    			// д����λ������

    WrPhaseOffsetTuningWorddata(p5, ChannelPhaseOffsetTuningWord4data); 		    // ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0d, 4, ChannelPhaseOffsetTuningWord4data, 1); 			    // д����λ������
 
    WrPhaseOffsetTuningWorddata(p6, ChannelPhaseOffsetTuningWord5data); 			// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0e, 4, ChannelPhaseOffsetTuningWord5data, 1);    			// д����λ������
																					   
    WrPhaseOffsetTuningWorddata(p7, ChannelPhaseOffsetTuningWord6data); 			// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0f, 4, ChannelPhaseOffsetTuningWord6data, 1); 				// д����λ������
																					   
    WrPhaseOffsetTuningWorddata(p8, ChannelPhaseOffsetTuningWord7data); 			// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x10, 4, ChannelPhaseOffsetTuningWord7data, 1);    			// д����λ������
																					   
    WrPhaseOffsetTuningWorddata(p9, ChannelPhaseOffsetTuningWord8data); 			// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x11, 4, ChannelPhaseOffsetTuningWord8data, 1); 				// д����λ������
																					   
    WrPhaseOffsetTuningWorddata(p10, ChannelPhaseOffsetTuningWord9data); 			// ��ʮ������λ��ת��Ϊ2������λ������?
    WriteData_AD9959(0x12, 4, ChannelPhaseOffsetTuningWord9data, 1);    			// д����λ������

    WrPhaseOffsetTuningWorddata(p11, ChannelPhaseOffsetTuningWord10data); 			// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x13, 4, ChannelPhaseOffsetTuningWord10data, 1); 				// д����λ������

    WrPhaseOffsetTuningWorddata(p12, ChannelPhaseOffsetTuningWord11data); 			// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x14, 4, ChannelPhaseOffsetTuningWord11data, 1);   			// д����λ������

    WrPhaseOffsetTuningWorddata(p13, ChannelPhaseOffsetTuningWord12data);	 		// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x15, 4, ChannelPhaseOffsetTuningWord12data, 1); 			    // д����λ������

    WrPhaseOffsetTuningWorddata(p14, ChannelPhaseOffsetTuningWord13data); 			// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x16, 4, ChannelPhaseOffsetTuningWord13data, 1);     			// д����λ������

    WrPhaseOffsetTuningWorddata(p15, ChannelPhaseOffsetTuningWord14data); 			// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x17, 4, ChannelPhaseOffsetTuningWord14data, 1); 				// д����λ������

    WrPhaseOffsetTuningWorddata(p16, ChannelPhaseOffsetTuningWord15data); 			// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x18, 4, ChannelPhaseOffsetTuningWord15data, 1);   			// д����λ������

    f = 200000;
    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);     				// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWord0data, 1);     		// д��Ƶ�ʿ�����
}

void AD9959_SetAM162(double f)
{
    uint8_t ChannelFrequencyTuningWord0data[4];

    uint8_t AmplitudeControldata[3] = {0x00, 0x03, 0xff};
    uint8_t ChannelAmplitudeTuningWord1data[4] = {0xef, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord2data[4] = {0xdf, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord3data[4] = {0xcf, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord4data[4] = {0xbf, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord5data[4] = {0xaf, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord6data[4] = {0x9f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord7data[4] = {0x8f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord8data[4] = {0x7f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord9data[4] = {0x6f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord10data[4] = {0x5f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord11data[4] = {0x4f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord12data[4] = {0x3f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord13data[4] = {0x2f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord14data[4] = {0x1f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord15data[4] = {0x0f, 0x30, 0x00, 0x00};
    uint8_t ChannelFunctionRegisterdata[3] = {0x40, 0x03, 0x30};
    uint8_t FunctionRegister1data[3] = {0xD0, 0xe3, 0x00};

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);              // ���ù��ܼĴ���
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);        // ����ͨ�����ܼĴ���

    WriteData_AD9959(ACR_ADD, 3, AmplitudeControldata, 0);               // ���÷��ȴ�С��S1��

    WriteData_AD9959(0x0a, 4, ChannelAmplitudeTuningWord1data, 0); 		 // ���÷��ȴ�С��S2��

    WriteData_AD9959(0x0b, 4, ChannelAmplitudeTuningWord2data, 0);

    WriteData_AD9959(0x0c, 4, ChannelAmplitudeTuningWord3data, 0);

    WriteData_AD9959(0x0d, 4, ChannelAmplitudeTuningWord4data, 0);        // ���÷��ȴ�С��S1��

    WriteData_AD9959(0x0e, 4, ChannelAmplitudeTuningWord5data, 0); 		  // ���÷��ȴ�С��S2��

    WriteData_AD9959(0x0f, 4, ChannelAmplitudeTuningWord6data, 0);

    WriteData_AD9959(0x10, 4, ChannelAmplitudeTuningWord7data, 0);

    WriteData_AD9959(0x11, 4, ChannelAmplitudeTuningWord8data, 0);        // ���÷��ȴ�С��S1��

    WriteData_AD9959(0x12, 4, ChannelAmplitudeTuningWord9data, 0); 		  // ���÷��ȴ�С��S2��

    WriteData_AD9959(0x13, 4, ChannelAmplitudeTuningWord10data, 0);

    WriteData_AD9959(0x14, 4, ChannelAmplitudeTuningWord11data, 0);

    WriteData_AD9959(0x15, 4, ChannelAmplitudeTuningWord12data, 0);       // ���÷��ȴ�С��S1��

    WriteData_AD9959(0x16, 4, ChannelAmplitudeTuningWord13data, 0); 	  // ���÷��ȴ�С��S2��

    WriteData_AD9959(0x17, 4, ChannelAmplitudeTuningWord14data, 0);

    WriteData_AD9959(0x18, 4, ChannelAmplitudeTuningWord15data, 0);

    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);    	  // дƵ�ʿ�����
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWord0data, 1);
}

/*******************************************************************************************/
/*	                          ʮ���׵��� CH3                                               */
/*******************************************************************************************/
void AD9959_SetFremodulation163(double f1, double f2, double f3, double f4, double f5, double f6, double f7, double f8, double f9, double f10, double f11, double f12, double f13, double f14, double f15, double f16)
{
    uint8_t ChannelFrequencyTuningWord0data[4];
    uint8_t ChannelFrequencyTuningWord1data[4];
    uint8_t ChannelFrequencyTuningWord2data[4];
    uint8_t ChannelFrequencyTuningWord3data[4];
    uint8_t ChannelFrequencyTuningWord4data[4];
    uint8_t ChannelFrequencyTuningWord5data[4];
    uint8_t ChannelFrequencyTuningWord6data[4];
    uint8_t ChannelFrequencyTuningWord7data[4];
    uint8_t ChannelFrequencyTuningWord8data[4];
    uint8_t ChannelFrequencyTuningWord9data[4];
    uint8_t ChannelFrequencyTuningWord10data[4];
    uint8_t ChannelFrequencyTuningWord11data[4];
    uint8_t ChannelFrequencyTuningWord12data[4];
    uint8_t ChannelFrequencyTuningWord13data[4];
    uint8_t ChannelFrequencyTuningWord14data[4];
    uint8_t ChannelFrequencyTuningWord15data[4];
    uint8_t ChannelFunctionRegisterdata[3] = {0x80, 0x23, 0x30};      	// ��RU/RD
    uint8_t FunctionRegister1data[3] = {0xD0, 0xf3, 0x00};                 
																		   
    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);           	// ���ù��ܼĴ���
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);    	// ����ͨ�����ܼĴ���
																		   
    WrFrequencyTuningWorddata(f1, ChannelFrequencyTuningWord0data);   	// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x04, 4, ChannelFrequencyTuningWord0data, 0); 		// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f2, ChannelFrequencyTuningWord1data);		// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0A, 4, ChannelFrequencyTuningWord1data, 0); 		// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f3, ChannelFrequencyTuningWord2data);		// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0B, 4, ChannelFrequencyTuningWord2data, 0); 		// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f4, ChannelFrequencyTuningWord3data);		// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0C, 4, ChannelFrequencyTuningWord3data, 0); 		// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f5, ChannelFrequencyTuningWord4data);		// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0D, 4, ChannelFrequencyTuningWord4data, 0); 		// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f6, ChannelFrequencyTuningWord5data);		// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0E, 4, ChannelFrequencyTuningWord5data, 0); 		// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f7, ChannelFrequencyTuningWord6data);		// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x0F, 4, ChannelFrequencyTuningWord6data, 0); 		// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f8, ChannelFrequencyTuningWord7data);		// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x10, 4, ChannelFrequencyTuningWord7data, 0); 		// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f9, ChannelFrequencyTuningWord8data);		// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x11, 4, ChannelFrequencyTuningWord8data, 0); 		// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f10, ChannelFrequencyTuningWord9data);   	// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x12, 4, ChannelFrequencyTuningWord9data, 0);     	// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f11, ChannelFrequencyTuningWord10data);   // ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x13, 4, ChannelFrequencyTuningWord10data, 0); 	// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f12, ChannelFrequencyTuningWord11data);   // ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x14, 4, ChannelFrequencyTuningWord11data, 0); 	// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f13, ChannelFrequencyTuningWord12data);   // ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x15, 4, ChannelFrequencyTuningWord12data, 0); 	// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f14, ChannelFrequencyTuningWord13data);   // ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x16, 4, ChannelFrequencyTuningWord13data, 0); 	// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f15, ChannelFrequencyTuningWord14data);   // ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x017, 4, ChannelFrequencyTuningWord14data, 0);	// д��Ƶ�ʿ�����
																		   
    WrFrequencyTuningWorddata(f16, ChannelFrequencyTuningWord15data);   // ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(0x18, 4, ChannelFrequencyTuningWord15data, 1); 	// д��Ƶ�ʿ�����
}

void AD9959_SetPhamodulation163(double f, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10, int p11, int p12, int p13, int p14, int p15, int p16)
{
    uint8_t ChannelFrequencyTuningWord0data[4];
    uint8_t ChannelPhaseOffsetTuningWord0data[4];
    uint8_t ChannelPhaseOffsetTuningWord1data[4];
    uint8_t ChannelPhaseOffsetTuningWord2data[4];
    uint8_t ChannelPhaseOffsetTuningWord3data[4];
    uint8_t ChannelPhaseOffsetTuningWord4data[4];
    uint8_t ChannelPhaseOffsetTuningWord5data[4];
    uint8_t ChannelPhaseOffsetTuningWord6data[4];
    uint8_t ChannelPhaseOffsetTuningWord7data[4];
    uint8_t ChannelPhaseOffsetTuningWord8data[4];
    uint8_t ChannelPhaseOffsetTuningWord9data[4];
    uint8_t ChannelPhaseOffsetTuningWord10data[4];
    uint8_t ChannelPhaseOffsetTuningWord11data[4];
    uint8_t ChannelPhaseOffsetTuningWord12data[4];
    uint8_t ChannelPhaseOffsetTuningWord13data[4];
    uint8_t ChannelPhaseOffsetTuningWord14data[4];
    uint8_t ChannelPhaseOffsetTuningWord15data[4];

    uint8_t ChannelFunctionRegisterdata[3] = {0xc0, 0x03, 0x30};                 	// ��λ����ģʽ���ã�����ɨ����ܿ�����
//    uint8_t AmplitudeControldata[3] = {0x00, 0x03, 0xff};                         // ʮ��������ʱ���ã�Ĭ��������
    uint8_t FunctionRegister1data[3] = {0xD0, 0xf3, 0x00};

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);                   		// ���ù��ܼĴ���
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);           		// ����ͨ�����ܼĴ���������λ��������

    WrPhaseOffsetTuningWorddata(p1, ChannelPhaseOffsetTuningWord0data); 			// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(CPOW0_ADD, 2, ChannelPhaseOffsetTuningWord0data, 1); 			// д����λ������

    WrPhaseOffsetTuningWorddata(p2, ChannelPhaseOffsetTuningWord1data); 			// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0a, 4, ChannelPhaseOffsetTuningWord1data, 1);    			// д����λ������
	
    WrPhaseOffsetTuningWorddata(p3, ChannelPhaseOffsetTuningWord2data); 			// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0b, 4, ChannelPhaseOffsetTuningWord2data, 1); 				// д����λ������

    WrPhaseOffsetTuningWorddata(p4, ChannelPhaseOffsetTuningWord3data); 			// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0c, 4, ChannelPhaseOffsetTuningWord3data, 1);    			// д����λ������

    WrPhaseOffsetTuningWorddata(p5, ChannelPhaseOffsetTuningWord4data); 			//��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0d, 4, ChannelPhaseOffsetTuningWord4data, 1); 				//д����λ������

    WrPhaseOffsetTuningWorddata(p6, ChannelPhaseOffsetTuningWord5data); 			// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0e, 4, ChannelPhaseOffsetTuningWord5data, 1);    			// д����λ������

    WrPhaseOffsetTuningWorddata(p7, ChannelPhaseOffsetTuningWord6data); 			// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x0f, 4, ChannelPhaseOffsetTuningWord6data, 1); 				// д����λ������

    WrPhaseOffsetTuningWorddata(p8, ChannelPhaseOffsetTuningWord7data); 			// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x10, 4, ChannelPhaseOffsetTuningWord7data, 1);    			// д����λ������

    WrPhaseOffsetTuningWorddata(p9, ChannelPhaseOffsetTuningWord8data); 			// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x11, 4, ChannelPhaseOffsetTuningWord8data, 1);			 	// д����λ������

    WrPhaseOffsetTuningWorddata(p10, ChannelPhaseOffsetTuningWord9data); 			// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x12, 4, ChannelPhaseOffsetTuningWord9data, 1);    			// д����λ������

    WrPhaseOffsetTuningWorddata(p11, ChannelPhaseOffsetTuningWord10data); 			// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x13, 4, ChannelPhaseOffsetTuningWord10data, 1); 				// д����λ������

    WrPhaseOffsetTuningWorddata(p12, ChannelPhaseOffsetTuningWord11data); 			// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x14, 4, ChannelPhaseOffsetTuningWord11data, 1);   			// д����λ������

    WrPhaseOffsetTuningWorddata(p13, ChannelPhaseOffsetTuningWord12data); 			// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x15, 4, ChannelPhaseOffsetTuningWord12data, 1); 				// д����λ������

    WrPhaseOffsetTuningWorddata(p14, ChannelPhaseOffsetTuningWord13data); 			// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x16, 4, ChannelPhaseOffsetTuningWord13data, 1);   			// д����λ������

    WrPhaseOffsetTuningWorddata(p15, ChannelPhaseOffsetTuningWord14data); 			// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x17, 4, ChannelPhaseOffsetTuningWord14data, 1);			    // д����λ������

    WrPhaseOffsetTuningWorddata(p16, ChannelPhaseOffsetTuningWord15data); 			// ��ʮ������λ��ת��Ϊ2������λ������
    WriteData_AD9959(0x18, 4, ChannelPhaseOffsetTuningWord15data, 1);   			// д����λ������

    f = 200000;
    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);     				// ��ʮ����Ƶ����ת��Ϊ2����Ƶ�ʿ�����
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWord0data, 1);     		// д��Ƶ�ʿ�����
}

void AD9959_SetAM163(double f)
{
    uint8_t ChannelFrequencyTuningWord0data[4];

    uint8_t AmplitudeControldata[3] = {0x00, 0x03, 0xff};
    uint8_t ChannelAmplitudeTuningWord1data[4] = {0xef, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord2data[4] = {0xdf, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord3data[4] = {0xcf, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord4data[4] = {0xbf, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord5data[4] = {0xaf, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord6data[4] = {0x9f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord7data[4] = {0x8f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord8data[4] = {0x7f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord9data[4] = {0x6f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord10data[4] = {0x5f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord11data[4] = {0x4f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord12data[4] = {0x3f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord13data[4] = {0x2f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord14data[4] = {0x1f, 0x30, 0x00, 0x00};
    uint8_t ChannelAmplitudeTuningWord15data[4] = {0x0f, 0x30, 0x00, 0x00};
    uint8_t ChannelFunctionRegisterdata[3] = {0x40, 0x03, 0x30};
    uint8_t FunctionRegister1data[3] = {0xD0, 0xf3, 0x00};

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);              			// ���ù��ܼĴ���
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);        			// ����ͨ�����ܼĴ���

    WriteData_AD9959(ACR_ADD, 3, AmplitudeControldata, 0);                       	// ���÷��ȴ�С��S1��

    WriteData_AD9959(0x0a, 4, ChannelAmplitudeTuningWord1data, 0); 					// ���÷��ȴ�С��S2��

    WriteData_AD9959(0x0b, 4, ChannelAmplitudeTuningWord2data, 0);

    WriteData_AD9959(0x0c, 4, ChannelAmplitudeTuningWord3data, 0);

    WriteData_AD9959(0x0d, 4, ChannelAmplitudeTuningWord4data, 0);                  // ���÷��ȴ�С��S1��

    WriteData_AD9959(0x0e, 4, ChannelAmplitudeTuningWord5data, 0); 					// ���÷��ȴ�С��S2��

    WriteData_AD9959(0x0f, 4, ChannelAmplitudeTuningWord6data, 0);

    WriteData_AD9959(0x10, 4, ChannelAmplitudeTuningWord7data, 0);

    WriteData_AD9959(0x11, 4, ChannelAmplitudeTuningWord8data, 0);                  // ���÷��ȴ�С��S1��

    WriteData_AD9959(0x12, 4, ChannelAmplitudeTuningWord9data, 0); 					// ���÷��ȴ�С��S2��

    WriteData_AD9959(0x13, 4, ChannelAmplitudeTuningWord10data, 0);

    WriteData_AD9959(0x14, 4, ChannelAmplitudeTuningWord11data, 0);

    WriteData_AD9959(0x15, 4, ChannelAmplitudeTuningWord12data, 0);                 // ���÷��ȴ�С��S1��

    WriteData_AD9959(0x16, 4, ChannelAmplitudeTuningWord13data, 0); 				// ���÷��ȴ�С��S2��

    WriteData_AD9959(0x17, 4, ChannelAmplitudeTuningWord14data, 0);

    WriteData_AD9959(0x18, 4, ChannelAmplitudeTuningWord15data, 0);

    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);     				// дƵ�ʿ�����
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWord0data, 1);
}

/*******************************************************************************************/
/*	                          ����ɨ��ģʽ��غ���                                         */
/*******************************************************************************************/

/**
 * @name	  AD9959_Frequency_Sweep
 * @brief     Ƶ��ɨ��ģʽ
 * @param	  f1����ʼƵ�ʣ�
			  f2����ֹƵ�ʣ�
			  a1�������ģ�
			  a2���½���
 * @return    ��
 * @DateTime  2019-7-19
 */
void AD9959_Frequency_Sweep(double f1, double f2, double a1, double a2) // f1��ʼƵ�ʣ�f2��ֹƵ�ʣ�a1�����ģ�a2�½���
{
    uint8_t ChannelFrequencyTuningWorddata1[4];
    uint8_t ChannelFrequencyTuningWorddata2[4];
    uint8_t ChannelFrequencyTuningWorddata3[4];
    uint8_t ChannelFrequencyTuningWorddata4[4];
    uint8_t FunctionRegister1data[3] = {0xD0, 0x00, 0x00};                         	// Ĭ�������ѡ��
    uint8_t ChannelFunctionRegisterdata[3] = {0x80, 0x43, 0x20};                   	// ɨƵģʽʱѡ��;����ɨ��û�����ã���RU/RD
    uint8_t SweepRampRatedata[2] = {0xff, 0xff};                                   	// Ĭ�ϵ�λɨ��ʱ���
//    uint8_t FunctionRegister1data[3] = {0xD0,0x04,0x00};                          // ����ɨ��ʱ��ҪRU/RDʱѡ����
//    uint8_t AmplitudeControldata[3] = {0xff,0x1f,0xff};                           // ����RU/RD

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);                       	// ���ù��ܼĴ���
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);                 	// ����ͨ�����ܼĴ���
//  WriteData_AD9959(ACR,3,AmplitudeControldata,1);                           		// ���������RU/RDģʽ��ѡ��,��Ҫ�޸Ĺ��ܼĴ���FR1

	//  f=500000;    // ������ʼƵ��S0��0x04��
    WrFrequencyTuningWorddata(f1, ChannelFrequencyTuningWorddata1);         	 	// дƵ�ʿ�����
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWorddata1, 0);
	//  f=5000000; //������ֹƵ��(0x0A)
    WrFrequencyTuningWorddata(f2, ChannelFrequencyTuningWorddata2);    			  	// дƵ�ʿ�����
    WriteData_AD9959(0x0A, 4, ChannelFrequencyTuningWorddata2, 0);
	//  f=100;   //����������
    WrFrequencyTuningWorddata(a1, ChannelFrequencyTuningWorddata3);      		  	// дƵ�ʿ�����
    WriteData_AD9959(RDW_ADD, 4, ChannelFrequencyTuningWorddata3, 0);
	//   f=100;  //�����½���
    WrFrequencyTuningWorddata(a2, ChannelFrequencyTuningWorddata4);      		  	// дƵ�ʿ�����
    WriteData_AD9959(FDW_ADD, 4, ChannelFrequencyTuningWorddata4, 0);

    WriteData_AD9959(LSRR_ADD, 2, SweepRampRatedata, 1);                       		// ���õ�λ����ʱ��

}

/**
 * @name	  AD9959_Phase_Sweep
 * @brief     ��λɨ��ģʽ
 * @param	  p1����ʼ��λ��
			  p2����ֹ��λ��
              a1�����������ģ�
              a2�������½���
 * @return    ��
 * @DateTime  2019-7-19
 */
void AD9959_Phase_Sweep(int p1, int p2, int a1, int a2, double f)   				// p1��ʼ��λ��p2��ֹ��λ��a1���������ģ�a2�����½���
{
    uint8_t ChannelPhaseOffsetTuningWorddata1[2];
    uint8_t ChannelPhaseOffsetTuningWorddata2[2];
    uint8_t ChannelPhaseOffsetTuningWorddata3[2];
    uint8_t ChannelPhaseOffsetTuningWorddata4[2];
    uint8_t ChannelFrequencyTuningWord0data[4];

    uint8_t FunctionRegister1data[3] = {0xD0, 0x00, 0x00};                          // Ĭ�������ѡ��
    uint8_t ChannelFunctionRegisterdata[3] = {0xc0, 0xC3, 0x30};                   	// ɨ��ģʽʱѡ������ɨ��û�����ã���RU/RD
    uint8_t SweepRampRatedata[2] = {0xff, 0xff};                                    // Ĭ�ϵ�λɨ��ʱ���
//uint8_t FunctionRegister1data[3] = {0xD0,0x04,0x00};                             	// ����ɨ��ʱ��ҪRU/RDʱѡ����
// uint8_t AmplitudeControldata[3] = {0xff,0x1f,0xff};                             	// ����RU/RD

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);                   		// ���ù��ܼĴ���
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);            		// ����ͨ�����ܼĴ���
//   WriteData_AD9959(ACR_ADD,3,AmplitudeControldata,1);                          	// ���������RU/RDģʽ��ѡ��,��Ҫ�޸Ĺ��ܼĴ���FR1

	//   p=0;// ������ʼ��λS0��0x04��   �����������Ӻ������治һ��
    WrPhaseOffsetTuningWorddata(p1, ChannelPhaseOffsetTuningWorddata1);
    WriteData_AD9959(CPOW0_ADD, 2, ChannelPhaseOffsetTuningWorddata1, 0);
	//    p=360;// ������ֹ��λE0(0x0A)
    WrPhaseOffsetTuningWorddata(p2, ChannelPhaseOffsetTuningWorddata2);
    WriteData_AD9959(0x0a, 4, ChannelPhaseOffsetTuningWorddata2, 0);
	//    p=10;// ����������
    WrPhaseOffsetTuningWorddata(a1, ChannelPhaseOffsetTuningWorddata3);
    WriteData_AD9959(RDW_ADD, 4, ChannelPhaseOffsetTuningWorddata3, 0);
	//    p=10;// �����½���
    WrPhaseOffsetTuningWorddata(a2, ChannelPhaseOffsetTuningWorddata4);
    WriteData_AD9959(FDW_ADD, 4, ChannelPhaseOffsetTuningWorddata4, 0);
    WriteData_AD9959(LSRR_ADD, 2, SweepRampRatedata, 0);                            // д��λ����ʱ��
	//   f=400000;    // �������Ƶ�ʴ�С
    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);       			// дƵ�ʿ�����
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWord0data, 1);
}

/**
 * @name	  AD9959_Amplitute_Sweep
 * @brief     ����ɨ��ģʽ
 * @param	  a����ֹ���ȣ�
			  a1�������ģ�
			  a2���½��ģ�
			  f�����Ƶ��
 * @return    ��
 * @DateTime  2019-7-19
 */
void AD9959_Amplitute_Sweep(int a, int a1, int a2, double f)                    	// a��ֹ���ȣ�a1�����ģ�a2�½��ģ�f���Ƶ��
{
    uint8_t ChannelAmplitudeTuningWorddata1[4];
    uint8_t ChannelAmplitudeTuningWorddata2[4];
    uint8_t ChannelAmplitudeTuningWorddata3[4];
    uint8_t ChannelFrequencyTuningWorddata4[4];

    uint8_t FunctionRegister1data[3] = {0xD0, 0x00, 0x00};                        	// Ĭ�������ѡ��
    uint8_t ChannelFunctionRegisterdata[3] = {0x40, 0x43, 0x20};                  	// ����ɨ��λ����
    uint8_t AmplitudeControldata[3] = {0x00, 0x0, 0x3f};                          	// ����ɨ��ģʽʱ����
    uint8_t SweepRampRatedata[2] = {0xff, 0xff};                                  	// Ĭ�ϵ�λɨ��ʱ���

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);              		  	// ���ù��ܼĴ���
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);
    //д��ʼ����S0
    WriteData_AD9959(ACR_ADD, 3, AmplitudeControldata, 0);
    //д��ֹ����E0
	//  a=1023; //д������,����д��1024�����Ϊ1023
    WrAmplitudeTuningWorddata(a, ChannelAmplitudeTuningWorddata1);
    WriteData_AD9959(0x0a, 4, ChannelAmplitudeTuningWorddata1, 0);     				// CTW0 address 0x04.���10MHZƵ��
	//  a=10; // д������
    WrAmplitudeTuningWorddata(a1, ChannelAmplitudeTuningWorddata2);   				// дƵ�ʿ�����
    WriteData_AD9959(RDW_ADD, 4, ChannelAmplitudeTuningWorddata2, 0);
	//  a=10;  // д�½���
    WrAmplitudeTuningWorddata(a2, ChannelAmplitudeTuningWorddata3);   				// дƵ�ʿ�����
    WriteData_AD9959(FDW_ADD, 4, ChannelAmplitudeTuningWorddata3, 0);
    WriteData_AD9959(LSRR_ADD, 2, SweepRampRatedata, 0);                            // д��λ����ʱ��
	//  f=400000;
    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWorddata4);      			// дƵ�ʿ�����
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWorddata4, 1);
}

/**
 * @name	  AD9959_enablechannel0
 * @brief     ʹ��ͨ��0
 * @param	  ��
 * @return    ��
 * @DateTime  2019-7-19
 */
void AD9959_enablechannel0(void)                    	// ͨ��0ʹ��
{
    uint8_t ChannelSelectRegisterdata0[1] =  {0x10};   	// ͨ��ѡ��Ĵ�����3�ߴ��䣬���ݸ�λ����

    WriteData_AD9959(CSR_ADD, 1, ChannelSelectRegisterdata0, 0);
}

/**
 * @name	  AD9959_enablechannel1
 * @brief     ʹ��ͨ��1
 * @param	  ��
 * @return    ��
 * @DateTime  2019-7-19
 */
void AD9959_enablechannel1(void)                   		// ͨ��1ʹ��
{
    uint8_t ChannelSelectRegisterdata1[1] = {0x20};   	// ͨ��ѡ��Ĵ�����3�ߴ��䣬���ݸ�λ����
    // SDIO0 �������� and SDIO2 �������
    WriteData_AD9959(CSR_ADD, 1, ChannelSelectRegisterdata1, 0);
}

/**
 * @name	  AD9959_enablechannel2
 * @brief     ʹ��ͨ��2
 * @param	  ��
 * @return    ��
 * @DateTime  2019-7-19
 */
void AD9959_enablechannel2(void)                   		// ͨ��2ʹ��
{
    uint8_t ChannelSelectRegisterdata1[1] = {0x40};   	// ͨ��ѡ��Ĵ�����3�ߴ��䣬���ݸ�λ����
    // SDIO0 �������� and SDIO2 �������
    WriteData_AD9959(CSR_ADD, 1, ChannelSelectRegisterdata1, 0);
}

/**
 * @name	  AD9959_enablechannel3
 * @brief     ʹ��ͨ��3
 * @param	  ��
 * @return    ��
 * @DateTime  2019-7-19
 */
void AD9959_enablechannel3(void)                   		// ͨ��3ʹ��
{
    uint8_t ChannelSelectRegisterdata1[1] = {0x80};   	// ͨ��ѡ��Ĵ�����3�ߴ��䣬���ݸ�λ����
    // SDIO0 �������� and SDIO2 �������
    WriteData_AD9959(CSR_ADD, 1, ChannelSelectRegisterdata1, 0);
}

//////////////////////////////////////////////�ڲ�����/////////////////////////////////////////////////////////

/**
 * @name	  WrPhaseOffsetTuningWorddata
 * @brief     ����Ƶ��ת����
 * @param	  f�����Ƶ�ʴ�С
			  ChannelFrequencyTuningWorddata����ת����ʮ����Ƶ�ʿ�����ָ��
 * @return    ��
 * @DateTime  2019-7-19
 */
void WrFrequencyTuningWorddata(double f, uint8_t *ChannelFrequencyTuningWorddata)
{
    unsigned char z;

    long int y;
    double x;
    x = 4294967296.0 / system_clk; 	// Ƶ�ʿ����ּ���
    f = f * x;
    y = (long int)f;            	// ǿ��ת������

    z = (uint8_t)(y >> 24); 		// 8λ����������ǿ��תΪ8λ�ַ���
    ChannelFrequencyTuningWorddata[0] = z;

    z = (uint8_t)(y >> 16);
    ChannelFrequencyTuningWorddata[1] = z;

    z = (uint8_t)(y >> 8);
    ChannelFrequencyTuningWorddata[2] = z;
	
    z = (uint8_t)(y >> 0);
    ChannelFrequencyTuningWorddata[3] = z;
}

/**
 * @name	  WrPhaseOffsetTuningWorddata
 * @brief     ������λת����
 * @param	  f�����Ƶ�ʴ�С
			  ChannelPhaseOffsetTuningWorddata����ת����ʮ������λ������ָ��
 * @return    ��
 * @DateTime  2019-7-19
 */
void WrPhaseOffsetTuningWorddata(double f, uint8_t *ChannelPhaseOffsetTuningWorddata)
{
    unsigned char z;

    long int y;
    double x;
    x = 16384.0 / 360;		 	// ��λ�����ּ���
    f = f * x;
    y = (long int)f;            // ǿ��ת������

    z = (uint8_t)(y >> 8); 		// 8λ����������ǿ��תΪ8λ�ַ���
    ChannelPhaseOffsetTuningWorddata[0] = z;

    z = (uint8_t)(y >> 0);
    ChannelPhaseOffsetTuningWorddata[1] = z;
}

/**
 * @name	  WrAmplitudeTuningWorddata
 * @brief     ����ɨ��ʱ�����������½��ķ���
 * @param	  f�����Ƶ�ʴ�С
			  ChannelAmplitudeTuningWorddata����ת����ʮ���Ʒ��ȿ�����ָ��
 * @return    ��
 * @DateTime  2019-7-19
 */
void WrAmplitudeTuningWorddata(double f, uint8_t *ChannelAmplitudeTuningWorddata)
{
    unsigned char z;

    unsigned long int y;
    double x;
    x = 1024.0 / 1024; 					// ���ȿ����ּ���
    f = f * x;
    y = (unsigned long int)f;           // ǿ��ת������

    z = (uint8_t)(y >> 2); 				// 8λ����������ǿ��תΪ8λ�ַ���
    ChannelAmplitudeTuningWorddata[0] = z ;
    z = (uint8_t)(y << 6);    			// ȡ��2λ
    ChannelAmplitudeTuningWorddata[1] = (z & 0xC0);
    ChannelAmplitudeTuningWorddata[2] = 0x00;
    ChannelAmplitudeTuningWorddata[3] = 0x00;
}

/**
 * @name	  WrAmplitudeTuningWorddata1
 * @brief     ����������ȵĴ�С
 * @param	  f�����Ƶ�ʴ�С
			  ChannelAmplitudeTuningWorddata����ת����ʮ���Ʒ��ȿ�����ָ��
			  ASRAmplituteWordata��ת���õĶ����Ʒ��ȿ�����ָ��
 * @return    ��
 * @DateTime  2019-7-19
 */
void WrAmplitudeTuningWorddata1(double f, uint8_t *ChannelAmplitudeTuningWorddata, uint8_t *ASRAmplituteWordata)
{
    unsigned char z;

    unsigned long int y;
    double x;
    x = 1024.0 / 1024; 					// ���ȿ����ּ���
    f = f * x * 2.1;
    y = (unsigned long int)f;           // ǿ��ת������

    ASRAmplituteWordata[0] = ChannelAmplitudeTuningWorddata[0];
	
    z = (uint8_t)(y >> 8); 				// 8λ����������ǿ��תΪ8λ�ַ���
    ChannelAmplitudeTuningWorddata[1] = (ChannelAmplitudeTuningWorddata[1] & 0xfc );
    ASRAmplituteWordata[1] = (ChannelAmplitudeTuningWorddata[1] | z );
    z = (uint8_t)(y << 0);
    ASRAmplituteWordata[2] = z;
}

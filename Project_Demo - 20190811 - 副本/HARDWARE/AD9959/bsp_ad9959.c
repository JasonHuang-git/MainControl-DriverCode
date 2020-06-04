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
 * @brief     初始化AD9959各通道的输出频率、幅度以及相位
 * @param	  空
 * @return    空
 * @DateTime  2019-7-19
 */
void Init_AD9959(void)
{
	// REF CLK = 25MHz, Multiplication factor = 20
	// 配置系统时钟频率
    uint8_t FR1_DATA[3] = {0xD3, 0x00, 0x00};  // 20 frequency doubling
	
    InitIO_9959();
    InitReset();

	// VCO gain control = 1, system clock = 500MHz
    WriteData_AD9959(FR1_ADD, 3, FR1_DATA, 1);
	
	// 设置各通道输出频率
    Write_frequence(0, SinFre[0]);
    Write_frequence(1, SinFre[1]);
    Write_frequence(2, SinFre[2]);
    Write_frequence(3, SinFre[3]);
	// 设置各通道输出相位
    Write_Phase(0, SinPhr[0]);
    Write_Phase(1, SinPhr[1]);
    Write_Phase(2, SinPhr[2]);
    Write_Phase(3, SinPhr[3]);
	// 设置各通道输出幅度
    Write_Amplitude(0, SinAmp[0]);
    Write_Amplitude(1, SinAmp[1]);
    Write_Amplitude(2, SinAmp[2]);
	Write_Amplitude(3, SinAmp[3]);
}

/**
 * @name	  delay_9959
 * @brief     供DDS使用的延时函数
 * @param	  延时时间长度(168MHz主频)
 * @return    空
 * @DateTime  2019-7-19
 */
void delay_9959(uint32_t length)
{
    length = length * 12;
    while(length--);
}

/**
 * @name	  InitIO_9959
 * @brief     初始化9959的IO口
 * @param	  空
 * @return    空
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
	// GPIOF初始化
	GPIO_InitStructure.GPIO_Pin   = DDS_PS0 | DDS_PS1 | DDS_PS2 | DDS_PS3 | DDS_SDIO0
									| DDS_SDIO1 | DDS_SDIO2 | DDS_SDIO3 | DDS_UPDATE;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(DDS_PORT1, &GPIO_InitStructure);
	
	// GPIOG初始化
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
 * @brief     复位DDS
 * @param	  空
 * @return    空
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
 * @brief     更新DDS
 * @param	  空
 * @return    空
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
 * @brief     读取DDS寄存器中的值
 * @param	  RegisterAddress：需要读取的寄存器地址
			  NumberofRegisters：需要读取的寄存器数量
              *RegisterData：数据起始地址
 * @return    空
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
 * @brief   控制器通过SPI向AD9959写数据
 * @param	RegisterAddress: 寄存器地址
			NumberofRegisters: 所含字节数
			*RegisterData: 数据起始地址
			temp: 是否更新IO寄存器
 * @return    空
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
 * @brief   设置通道输出频率
 * @param	Channel:  输出通道
			Freq:     输出频率
 * @return    空
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
 * @brief   设置通道输出幅度
 * @param	Channel:  输出通道
			Ampli:    输出幅度
 * @return    空
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
 * @brief     设置通道输出相位
 * @param	  Channel:  输出通道
			  Phase:    输出相位,范围：0°~360°(对应角度：0~2^14)
 * @return    空
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
 * @brief     输出单个频率
 * @param	  f: 输出频率
 * @return    空
 * @DateTime  2019-7-19
 */
void AD9959_Setwavefrequency (double f)                                 	// 输出单个频率
{
    uint8_t ChannelFrequencyTuningWord1data[4];
    uint8_t ChannelFunctionRegisterdata[3] = {0x00, 0x23, 0x35};        	// 单频模式时选择，启用正弦功能（sine）
    uint8_t FunctionRegister1data[3] = {0xD0, 0x00, 0x00};

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);             	// 设置功能寄存器
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);    		// 设置通道功能寄存器，目的是为了实现正余弦功能
    //（没有要求时可以不设置该寄存器同样也可以正常输出）
    // 寄存器8、9位控制输出幅度，分四个档位
    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord1data);  		// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWord1data, 1); 	// 写入频率控制字
}

/**
 * @name	  AD9959_Setwavefrequency
 * @brief     设置输出波形的相位
 * @param	  f: 输出频率
			  p：输出相位
 * @return    空
 * @DateTime  2019-7-19
 */
void AD9959_Setwavephase(double f, int p)                                   // 输出相位程序可调
{
    uint8_t ChannelPhaseOffsetTuningWorddata[2];
    uint8_t ChannelFrequencyTuningWorddata[4];

    uint8_t ChannelFunctionRegisterdata[3] = {0x00, 0x23, 0x35};      		// 单频模式时选择，启用正弦功能（sine）
    uint8_t FunctionRegister1data[3] = {0xD0, 0x00, 0x00};
    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);             	// 设置功能寄存器

    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);    		// 设置通道功能寄存器，目的是为了实现正余弦功能

    WrPhaseOffsetTuningWorddata(p, ChannelPhaseOffsetTuningWorddata); 		// 设置相位转换字
    WriteData_AD9959(CPOW0_ADD, 2, ChannelPhaseOffsetTuningWorddata, 0); 	// 写入相位控制字

    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWorddata);    		// 设置频率转换字
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWorddata, 1);  	// 写入频率控制字
}

/**
 * @name	  AD9959_Setwaveamplitute
 * @brief     设置输出波形的幅度
 * @param	  f: 输出频率
			  a：输出幅度
 * @return    空
 * @DateTime  2019-7-19
 */
void AD9959_Setwaveamplitute(double f, int a)                    			// 输出幅度可自定义调节
{
    uint8_t ChannelFrequencyTuningWorddata[4];
    uint8_t ASRAmplituteWordata[3];
    uint8_t AmplitudeControldata[3] = {0xff, 0x17, 0xff};             		// 手动控制输出幅度
    uint8_t ChannelFunctionRegisterdata[3] = {0x00, 0x23, 0x35};      		// 单频模式时选择，启用正弦功能（sine）

    uint8_t FunctionRegister1data[3] = {0xD0, 0x00, 0x00};
    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);            		// 设置功能寄存器
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);   		// 设置通道功能寄存器，目的是为了实现正余弦功能
    WrAmplitudeTuningWorddata1(a, AmplitudeControldata, ASRAmplituteWordata);
    WriteData_AD9959(ACR_ADD, 3, ASRAmplituteWordata, 0);           		// 设置输出幅度控制模式

    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWorddata);  			// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWorddata, 1); 		// 写入频率转换字
}

/*******************************************************************************************/
/*	                          二阶调制                                                     */
/*******************************************************************************************/

void AD9959_SetFremodulation2(double f1, double f2)
{
    uint8_t ChannelFrequencyTuningWord0data[4];
    uint8_t ChannelFrequencyTuningWord1data[4];
    uint8_t ChannelFunctionRegisterdata[3] = {0x80, 0x23, 0x30};      	// 无RU/RD
    uint8_t FunctionRegister1data[3] = {0xD0, 0x00, 0x00};

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);            	// 设置功能寄存器
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);       // 设置通道功能寄存器

    // 设置起始频率S0（0x0A）
    WrFrequencyTuningWorddata(f1, ChannelFrequencyTuningWord0data);     // 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0A, 4, ChannelFrequencyTuningWord0data, 0); 		// 写入频率控制字

    // 设置终止频率E0（0x04）
    WrFrequencyTuningWorddata(f2, ChannelFrequencyTuningWord1data);   	// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWord1data, 1); // 写入频率控制字
}

void AD9959_SetPhamodulation2(double f, int p1, int p2)            		// p1起始相位，p2终止相位
{
//    uint8_t ChannelFrequencyTuningWord0data[4];
    uint8_t ChannelFrequencyTuningWord1data[4];
    uint8_t ChannelPhaseOffsetTuningWord0data[2];
    uint8_t ChannelPhaseOffsetTuningWord1data[2];

    uint8_t ChannelFunctionRegisterdata[3] = {0xc0, 0x03, 0x34};            // 相位调制模式启用（连续扫描可能开启）
//    uint8_t AmplitudeControldata[3] = {0x00, 0x03, 0xff};                 // 二级调制时启用，默认输出最大
    uint8_t FunctionRegister1data[3] = {0xD0, 0x00, 0x00};
//   uint8_t FunctionRegister1data[3] = {0xD0,0x54,0x00};                   // 二级调制时需进行幅度RU/RD时选择
    // PO进行调频，P2进行幅度RU/RD
//   uint8_t AmplitudeControldata[3] = {0xff,0x1f,0xff};                    // 开启RU/RD

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);                 // 设置功能寄存器

    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);           // 设置通道功能寄存器，即相位调制启动
//    WriteData_AD9959(ACR,3,AmplitudeControldata,0);                    	// 需输出幅度RU/RD模式才选择,且要修改功能寄存器FR1
    //设置地址0x05（CPOW0） 的相位

    WrPhaseOffsetTuningWorddata(p1, ChannelPhaseOffsetTuningWord0data); 	// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(CPOW0_ADD, 2, ChannelPhaseOffsetTuningWord0data, 0); 	// 写入相位控制字
    //设置地址0x0a（CW1） 的相位

    WrPhaseOffsetTuningWorddata(p2, ChannelPhaseOffsetTuningWord1data); 	// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0a, 4, ChannelPhaseOffsetTuningWord1data, 0);     	// 写入相位控制字
    // f=2000000;    //设置输出频率
    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord1data);     		// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWord1data, 1);     // 写入频率控制字
}

void AD9959_SetAM2(double f)
{
    uint8_t ChannelFrequencyTuningWord0data[4];
    uint8_t ChannelAmplitudeTuningWorddata[4] = {0x1f, 0x30, 0x00, 0x00}; 	// 72mV
    uint8_t ChannelFunctionRegisterdata[3] = {0x40, 0x03, 0x30};
    uint8_t AmplitudeControldata[3] = {0x00, 0x03, 0xff}; 					// 500mV
    uint8_t FunctionRegister1data[3] = {0xD0, 0x00, 0x00};

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);             	// 设置功能寄存器
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);       	// 设置通道功能寄存器

    WriteData_AD9959(ACR_ADD, 3, AmplitudeControldata, 0);              	// 设置幅度大小（S1）

    WriteData_AD9959(0x0a, 4, ChannelAmplitudeTuningWorddata, 0);  			// 设置幅度大小（S2）
    // 设置输出频率大小
    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);      	// 写频率控制字
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWord0data, 1);
}

/*******************************************************************************************/
/*	                          四阶调制 CH0/1                                               */
/*******************************************************************************************/

void AD9959_SetFremodulation4(double f1, double f2, double f3, double f4)
{
    uint8_t ChannelFrequencyTuningWord0data[4];
    uint8_t ChannelFrequencyTuningWord1data[4];
    uint8_t ChannelFrequencyTuningWord2data[4];
    uint8_t ChannelFrequencyTuningWord3data[4];
    uint8_t ChannelFunctionRegisterdata[3] = {0x80, 0x23, 0x30};       // 无RU/RD
    uint8_t FunctionRegister1data[3] = {0xD0, 0x01, 0x00};

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);            // 设置功能寄存器
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);      // 设置通道功能寄存器


    WrFrequencyTuningWorddata(f1, ChannelFrequencyTuningWord0data);    // 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x04, 4, ChannelFrequencyTuningWord0data, 0);     // 写入频率控制字

    WrFrequencyTuningWorddata(f2, ChannelFrequencyTuningWord1data);    // 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0A, 4, ChannelFrequencyTuningWord1data, 0);     // 写入频率控制字

    WrFrequencyTuningWorddata(f3, ChannelFrequencyTuningWord2data);    // 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0B, 4, ChannelFrequencyTuningWord2data, 0);     // 写入频率控制字

    WrFrequencyTuningWorddata(f4, ChannelFrequencyTuningWord3data);    // 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0C, 4, ChannelFrequencyTuningWord3data, 1);     // 写入频率控制字
}

void AD9959_SetPhamodulation4(double f, int p1, int p2, int p3, int p4)
{
    uint8_t ChannelFrequencyTuningWord0data[4];
    uint8_t ChannelPhaseOffsetTuningWord0data[4];
    uint8_t ChannelPhaseOffsetTuningWord1data[4];
    uint8_t ChannelPhaseOffsetTuningWord2data[4];
    uint8_t ChannelPhaseOffsetTuningWord3data[4];

    uint8_t ChannelFunctionRegisterdata[3] = {0xc0, 0x03, 0x30};        	// 相位调制模式启用（连续扫描可能开启）
//    uint8_t AmplitudeControldata[3] = {0x00, 0x03, 0xff};             	// 四级调制时启用，默认输出最大
    uint8_t FunctionRegister1data[3] = {0xD0, 0x01, 0x00};

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);             	// 设置功能寄存器
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);       	// 设置通道功能寄存器，即相位调制启动

    WrPhaseOffsetTuningWorddata(p1, ChannelPhaseOffsetTuningWord0data); 	// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(CPOW0_ADD, 2, ChannelPhaseOffsetTuningWord0data, 1); 	// 写入相位控制字

    WrPhaseOffsetTuningWorddata(p2, ChannelPhaseOffsetTuningWord1data); 	// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0a, 4, ChannelPhaseOffsetTuningWord1data, 1);    	// 写入相位控制字

    WrPhaseOffsetTuningWorddata(p3, ChannelPhaseOffsetTuningWord2data); 	// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0b, 4, ChannelPhaseOffsetTuningWord2data, 1); 		// 写入相位控制字

    WrPhaseOffsetTuningWorddata(p4, ChannelPhaseOffsetTuningWord3data); 	// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0c, 4, ChannelPhaseOffsetTuningWord3data, 1);    	// 写入相位控制字
    f = 200000;
    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);     		// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWord0data, 1);     // 写入频率控制字
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

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);              	// 设置功能寄存器
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);        	// 设置通道功能寄存器

    WriteData_AD9959(ACR_ADD, 3, AmplitudeControldata, 0);                  // 设置幅度大小（S1）

    WriteData_AD9959(0x0a, 4, ChannelAmplitudeTuningWord1data, 0); 			// 设置幅度大小（S2）

    WriteData_AD9959(0x0b, 4, ChannelAmplitudeTuningWord2data, 0);

    WriteData_AD9959(0x0c, 4, ChannelAmplitudeTuningWord3data, 0);

    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);     		// 写频率控制字
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWord0data, 1);
}

/*******************************************************************************************/
/*	                          四阶调制 CH2/3                                               */
/*******************************************************************************************/

void AD9959_SetFremodulation42(double f1, double f2, double f3, double f4)
{
    uint8_t ChannelFrequencyTuningWord0data[4];
    uint8_t ChannelFrequencyTuningWord1data[4];
    uint8_t ChannelFrequencyTuningWord2data[4];
    uint8_t ChannelFrequencyTuningWord3data[4];
    uint8_t ChannelFunctionRegisterdata[3] = {0x80, 0x23, 0x30};      	// 无RU/RD
    uint8_t FunctionRegister1data[3] = {0xD0, 0x51, 0x00};                 
																		   
    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);            	// 设置功能寄存器
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);    	// 设置通道功能寄存器
																		   
    WrFrequencyTuningWorddata(f1, ChannelFrequencyTuningWord0data);   	// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x04, 4, ChannelFrequencyTuningWord0data, 0); 		// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f2, ChannelFrequencyTuningWord1data);   	// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0A, 4, ChannelFrequencyTuningWord1data, 0); 		// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f3, ChannelFrequencyTuningWord2data);   	// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0B, 4, ChannelFrequencyTuningWord2data, 0); 		// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f4, ChannelFrequencyTuningWord3data);   	// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0C, 4, ChannelFrequencyTuningWord3data, 1); 		// 写入频率控制字
}

void AD9959_SetPhamodulation42(double f, int p1, int p2, int p3, int p4)
{
    uint8_t ChannelFrequencyTuningWord0data[4];
    uint8_t ChannelPhaseOffsetTuningWord0data[4];
    uint8_t ChannelPhaseOffsetTuningWord1data[4];
    uint8_t ChannelPhaseOffsetTuningWord2data[4];
    uint8_t ChannelPhaseOffsetTuningWord3data[4];

    uint8_t ChannelFunctionRegisterdata[3] = {0xc0, 0x03, 0x30};        	// 相位调制模式启用（连续扫描可能开启）
//    uint8_t AmplitudeControldata[3] = {0x00, 0x03, 0xff};             	// 四级调制时启用，默认输出最大
    uint8_t FunctionRegister1data[3] = {0xD0, 0x51, 0x00};                     
																			   
    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);             	// 设置功能寄存器
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);       	// 设置通道功能寄存器，即相位调制启动
																			   
    WrPhaseOffsetTuningWorddata(p1, ChannelPhaseOffsetTuningWord0data); 	// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(CPOW0_ADD, 2, ChannelPhaseOffsetTuningWord0data, 1); 	// 写入相位控制字
																			   
    WrPhaseOffsetTuningWorddata(p2, ChannelPhaseOffsetTuningWord1data); 	// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0a, 4, ChannelPhaseOffsetTuningWord1data, 1);    	// 写入相位控制字
																			   
    WrPhaseOffsetTuningWorddata(p3, ChannelPhaseOffsetTuningWord2data); 	// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0b, 4, ChannelPhaseOffsetTuningWord2data, 1);		// 写入相位控制字
																			   
    WrPhaseOffsetTuningWorddata(p4, ChannelPhaseOffsetTuningWord3data); 	// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0c, 4, ChannelPhaseOffsetTuningWord3data, 1);    	// 写入相位控制字
    f = 200000;                                                                
    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);     		// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWord0data, 1);     // 写入频率控制字
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

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);              // 设置功能寄存器
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);        // 设置通道功能寄存器

    WriteData_AD9959(ACR_ADD, 3, AmplitudeControldata, 0);               // 设置幅度大小（S1）

    WriteData_AD9959(0x0a, 4, ChannelAmplitudeTuningWord1data, 0); 		 // 设置幅度大小（S2）

    WriteData_AD9959(0x0b, 4, ChannelAmplitudeTuningWord2data, 0);

    WriteData_AD9959(0x0c, 4, ChannelAmplitudeTuningWord3data, 0);

    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);     	 // 写频率控制字
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWord0data, 1);
}


/*******************************************************************************************/
/*	                          八阶调制 CH0                                                 */
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
    uint8_t ChannelFunctionRegisterdata[3] = {0x80, 0x23, 0x30};      	// 无RU/RD
    uint8_t FunctionRegister1data[3] = {0xD0, 0xc2, 0x00};

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);            	// 设置功能寄存器
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);    	// 设置通道功能寄存器
																		   
    WrFrequencyTuningWorddata(f1, ChannelFrequencyTuningWord0data);   	// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x04, 4, ChannelFrequencyTuningWord0data, 0); 		// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f2, ChannelFrequencyTuningWord1data);		// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0A, 4, ChannelFrequencyTuningWord1data, 0); 		// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f3, ChannelFrequencyTuningWord2data);		// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0B, 4, ChannelFrequencyTuningWord2data, 0); 		// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f4, ChannelFrequencyTuningWord3data);		// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0C, 4, ChannelFrequencyTuningWord3data, 0); 		// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f5, ChannelFrequencyTuningWord4data);		// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0D, 4, ChannelFrequencyTuningWord4data, 0); 		// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f6, ChannelFrequencyTuningWord5data);		// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0E, 4, ChannelFrequencyTuningWord5data, 0); 		// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f7, ChannelFrequencyTuningWord6data);		// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0F, 4, ChannelFrequencyTuningWord6data, 0); 		// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f8, ChannelFrequencyTuningWord7data);		// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x10, 4, ChannelFrequencyTuningWord7data, 1); 		// 写入频率控制字
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
 
    uint8_t ChannelFunctionRegisterdata[3] = {0xc0, 0x03, 0x30};           	// 相位调制模式启用（连续扫描可能开启）
//    uint8_t AmplitudeControldata[3] = {0x00, 0x03, 0xff};                	// 八级调制时启用，默认输出最大
    uint8_t FunctionRegister1data[3] = {0xD0, 0xc2, 0x00};

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);                	// 设置功能寄存器
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);          	// 设置通道功能寄存器，即相位调制启动
	
    WrPhaseOffsetTuningWorddata(p1, ChannelPhaseOffsetTuningWord0data);   	// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(CPOW0_ADD, 2, ChannelPhaseOffsetTuningWord0data, 1);  	// 写入相位控制字

    WrPhaseOffsetTuningWorddata(p2, ChannelPhaseOffsetTuningWord1data); 	// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0a, 4, ChannelPhaseOffsetTuningWord1data, 1);    	// 写入相位控制字
																			   
    WrPhaseOffsetTuningWorddata(p3, ChannelPhaseOffsetTuningWord2data); 	// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0b, 4, ChannelPhaseOffsetTuningWord2data, 1); 		// 写入相位控制字
																			   
    WrPhaseOffsetTuningWorddata(p4, ChannelPhaseOffsetTuningWord3data); 	// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0c, 4, ChannelPhaseOffsetTuningWord3data, 1);    	// 写入相位控制字
																			   
    WrPhaseOffsetTuningWorddata(p5, ChannelPhaseOffsetTuningWord4data); 	// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0d, 4, ChannelPhaseOffsetTuningWord4data, 1); 		// 写入相位控制字
																			   
    WrPhaseOffsetTuningWorddata(p6, ChannelPhaseOffsetTuningWord5data); 	// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0e, 4, ChannelPhaseOffsetTuningWord5data, 1);    	// 写入相位控制字
																			   
    WrPhaseOffsetTuningWorddata(p7, ChannelPhaseOffsetTuningWord6data); 	// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0f, 4, ChannelPhaseOffsetTuningWord6data, 1); 		// 写入相位控制字
																			   
    WrPhaseOffsetTuningWorddata(p8, ChannelPhaseOffsetTuningWord7data); 	// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x10, 4, ChannelPhaseOffsetTuningWord7data, 1);    	// 写入相位控制字
    f = 200000;                                                                
    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);     		// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWord0data, 1);     // 写入频率控制字
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

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);              	// 设置功能寄存器
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);        	// 设置通道功能寄存器

    WriteData_AD9959(ACR_ADD, 3, AmplitudeControldata, 0);                  // 设置幅度大小（S1）

    WriteData_AD9959(0x0a, 4, ChannelAmplitudeTuningWord1data, 0); 			// 设置幅度大小（S2）

    WriteData_AD9959(0x0b, 4, ChannelAmplitudeTuningWord2data, 0);

    WriteData_AD9959(0x0c, 4, ChannelAmplitudeTuningWord3data, 0);

    WriteData_AD9959(0x0d, 4, ChannelAmplitudeTuningWord4data, 0);           // 设置幅度大小（S1）

    WriteData_AD9959(0x0e, 4, ChannelAmplitudeTuningWord5data, 0); 			 // 设置幅度大小（S2）

    WriteData_AD9959(0x0f, 4, ChannelAmplitudeTuningWord6data, 0);

    WriteData_AD9959(0x10, 4, ChannelAmplitudeTuningWord7data, 0);

    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);     		 // 写频率控制字
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWord0data, 1);
}

/*******************************************************************************************/
/*	                          八阶调制 CH1                                                 */
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
    uint8_t ChannelFunctionRegisterdata[3] = {0x80, 0x23, 0x30};      	// 无RU/RD
    uint8_t FunctionRegister1data[3] = {0xD0, 0xd2, 0x00};

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);            	// 设置功能寄存器
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);    	// 设置通道功能寄存器

    WrFrequencyTuningWorddata(f1, ChannelFrequencyTuningWord0data);   	// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x04, 4, ChannelFrequencyTuningWord0data, 0); 		// 写入频率控制字
		
    WrFrequencyTuningWorddata(f2, ChannelFrequencyTuningWord1data);		// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0A, 4, ChannelFrequencyTuningWord1data, 0); 		// 写入频率控制字
		
    WrFrequencyTuningWorddata(f3, ChannelFrequencyTuningWord2data);		// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0B, 4, ChannelFrequencyTuningWord2data, 0); 		// 写入频率控制字
		
    WrFrequencyTuningWorddata(f4, ChannelFrequencyTuningWord3data);		// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0C, 4, ChannelFrequencyTuningWord3data, 0); 		// 写入频率控制字
		
    WrFrequencyTuningWorddata(f5, ChannelFrequencyTuningWord4data);		// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0D, 4, ChannelFrequencyTuningWord4data, 0); 		// 写入频率控制字
		
    WrFrequencyTuningWorddata(f6, ChannelFrequencyTuningWord5data);		// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0E, 4, ChannelFrequencyTuningWord5data, 0); 		// 写入频率控制字
		
    WrFrequencyTuningWorddata(f7, ChannelFrequencyTuningWord6data);		// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0F, 4, ChannelFrequencyTuningWord6data, 0); 		// 写入频率控制字
		
    WrFrequencyTuningWorddata(f8, ChannelFrequencyTuningWord7data);		// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x10, 4, ChannelFrequencyTuningWord7data, 1); 		// 写入频率控制字
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

    uint8_t ChannelFunctionRegisterdata[3] = {0xc0, 0x03, 0x30};            // 相位调制模式启用（连续扫描可能开启）
//    uint8_t AmplitudeControldata[3] = {0x00, 0x03, 0xff};                 // 八级调制时启用，默认输出最大
    uint8_t FunctionRegister1data[3] = {0xD0, 0xd2, 0x00};

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);                 // 设置功能寄存器
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);           // 设置通道功能寄存器，即相位调制启动
																			   
    WrPhaseOffsetTuningWorddata(p1, ChannelPhaseOffsetTuningWord0data); 	// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(CPOW0_ADD, 2, ChannelPhaseOffsetTuningWord0data, 1); 	// 写入相位控制字
																			   
    WrPhaseOffsetTuningWorddata(p2, ChannelPhaseOffsetTuningWord1data); 	// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0a, 4, ChannelPhaseOffsetTuningWord1data, 1);    	// 写入相位控制字
																			   
    WrPhaseOffsetTuningWorddata(p3, ChannelPhaseOffsetTuningWord2data); 	// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0b, 4, ChannelPhaseOffsetTuningWord2data, 1); 		// 写入相位控制字
																			   
    WrPhaseOffsetTuningWorddata(p4, ChannelPhaseOffsetTuningWord3data); 	// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0c, 4, ChannelPhaseOffsetTuningWord3data, 1);    	// 写入相位控制字
																			   
    WrPhaseOffsetTuningWorddata(p5, ChannelPhaseOffsetTuningWord4data); 	// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0d, 4, ChannelPhaseOffsetTuningWord4data, 1); 		// 写入相位控制字
																			   
    WrPhaseOffsetTuningWorddata(p6, ChannelPhaseOffsetTuningWord5data); 	// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0e, 4, ChannelPhaseOffsetTuningWord5data, 1);    	// 写入相位控制字
																			   
    WrPhaseOffsetTuningWorddata(p7, ChannelPhaseOffsetTuningWord6data); 	// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0f, 4, ChannelPhaseOffsetTuningWord6data, 1); 		// 写入相位控制字
																			   
    WrPhaseOffsetTuningWorddata(p8, ChannelPhaseOffsetTuningWord7data); 	// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x10, 4, ChannelPhaseOffsetTuningWord7data, 1);    	// 写入相位控制字
    f = 200000;                                                                
    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);     		// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWord0data, 1); 	// 写入频率控制字
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

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);              	// 设置功能寄存器
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);        	// 设置通道功能寄存器

    WriteData_AD9959(ACR_ADD, 3, AmplitudeControldata, 0);                  // 设置幅度大小（S1）

    WriteData_AD9959(0x0a, 4, ChannelAmplitudeTuningWord1data, 0); 			// 设置幅度大小（S2）

    WriteData_AD9959(0x0b, 4, ChannelAmplitudeTuningWord2data, 0);

    WriteData_AD9959(0x0c, 4, ChannelAmplitudeTuningWord3data, 0);

    WriteData_AD9959(0x0d, 4, ChannelAmplitudeTuningWord4data, 0);           // 设置幅度大小（S1）

    WriteData_AD9959(0x0e, 4, ChannelAmplitudeTuningWord5data, 0); 			 // 设置幅度大小（S2）

    WriteData_AD9959(0x0f, 4, ChannelAmplitudeTuningWord6data, 0);

    WriteData_AD9959(0x10, 4, ChannelAmplitudeTuningWord7data, 0);

    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);     		 // 写频率控制字
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWord0data, 1);
}

/*******************************************************************************************/
/*	                          八阶调制 CH2                                                 */
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
    uint8_t ChannelFunctionRegisterdata[3] = {0x80, 0x23, 0x30};      	// 无RU/RD
    uint8_t FunctionRegister1data[3] = {0xD0, 0xe2, 0x00};

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);            	// 设置功能寄存器
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);    	// 设置通道功能寄存器

    WrFrequencyTuningWorddata(f1, ChannelFrequencyTuningWord0data);   	// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x04, 4, ChannelFrequencyTuningWord0data, 0); 		// 写入频率控制字

    WrFrequencyTuningWorddata(f2, ChannelFrequencyTuningWord1data);   	// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0A, 4, ChannelFrequencyTuningWord1data, 0); 		// 写入频率控制字

    WrFrequencyTuningWorddata(f3, ChannelFrequencyTuningWord2data);   	// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0B, 4, ChannelFrequencyTuningWord2data, 0); 		// 写入频率控制字

    WrFrequencyTuningWorddata(f4, ChannelFrequencyTuningWord3data);   	// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0C, 4, ChannelFrequencyTuningWord3data, 0); 		// 写入频率控制字

    WrFrequencyTuningWorddata(f5, ChannelFrequencyTuningWord4data);   	// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0D, 4, ChannelFrequencyTuningWord4data, 0); 		// 写入频率控制字

    WrFrequencyTuningWorddata(f6, ChannelFrequencyTuningWord5data);   	// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0E, 4, ChannelFrequencyTuningWord5data, 0); 		// 写入频率控制字

    WrFrequencyTuningWorddata(f7, ChannelFrequencyTuningWord6data);   	// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0F, 4, ChannelFrequencyTuningWord6data, 0); 		// 写入频率控制字

    WrFrequencyTuningWorddata(f8, ChannelFrequencyTuningWord7data);   	// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x10, 4, ChannelFrequencyTuningWord7data, 1); 		// 写入频率控制字
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

    uint8_t ChannelFunctionRegisterdata[3] = {0xc0, 0x03, 0x30};           // 相位调制模式启用（连续扫描可能开启）
//    uint8_t AmplitudeControldata[3] = {0x00, 0x03, 0xff};                // 八级调制时启用，默认输出最大
    uint8_t FunctionRegister1data[3] = {0xD0, 0xe2, 0x00};

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);                // 设置功能寄存器
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);          // 设置通道功能寄存器，即相位调制启动

    WrPhaseOffsetTuningWorddata(p1, ChannelPhaseOffsetTuningWord0data);    // 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(CPOW0_ADD, 2, ChannelPhaseOffsetTuningWord0data, 1);  // 写入相位控制字

    WrPhaseOffsetTuningWorddata(p2, ChannelPhaseOffsetTuningWord1data);    // 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0a, 4, ChannelPhaseOffsetTuningWord1data, 1);       // 写入相位控制字

    WrPhaseOffsetTuningWorddata(p3, ChannelPhaseOffsetTuningWord2data);    // 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0b, 4, ChannelPhaseOffsetTuningWord2data, 1);       // 写入相位控制字

    WrPhaseOffsetTuningWorddata(p4, ChannelPhaseOffsetTuningWord3data);    // 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0c, 4, ChannelPhaseOffsetTuningWord3data, 1);       // 写入相位控制字

    WrPhaseOffsetTuningWorddata(p5, ChannelPhaseOffsetTuningWord4data);    // 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0d, 4, ChannelPhaseOffsetTuningWord4data, 1);       // 写入相位控制字

    WrPhaseOffsetTuningWorddata(p6, ChannelPhaseOffsetTuningWord5data);    // 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0e, 4, ChannelPhaseOffsetTuningWord5data, 1);       // 写入相位控制字

    WrPhaseOffsetTuningWorddata(p7, ChannelPhaseOffsetTuningWord6data);    // 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0f, 4, ChannelPhaseOffsetTuningWord6data, 1);       // 写入相位控制字

    WrPhaseOffsetTuningWorddata(p8, ChannelPhaseOffsetTuningWord7data);    // 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x10, 4, ChannelPhaseOffsetTuningWord7data, 1);       // 写入相位控制字
    f = 200000;
    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);         // 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWord0data, 1);    // 写入频率控制字
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

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);              // 设置功能寄存器
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);        // 设置通道功能寄存器

    WriteData_AD9959(ACR_ADD, 3, AmplitudeControldata, 0);               // 设置幅度大小（S1）

    WriteData_AD9959(0x0a, 4, ChannelAmplitudeTuningWord1data, 0); 		 // 设置幅度大小（S2）

    WriteData_AD9959(0x0b, 4, ChannelAmplitudeTuningWord2data, 0);

    WriteData_AD9959(0x0c, 4, ChannelAmplitudeTuningWord3data, 0);

    WriteData_AD9959(0x0d, 4, ChannelAmplitudeTuningWord4data, 0);       // 设置幅度大小（S1）

    WriteData_AD9959(0x0e, 4, ChannelAmplitudeTuningWord5data, 0); 		 // 设置幅度大小（S2）

    WriteData_AD9959(0x0f, 4, ChannelAmplitudeTuningWord6data, 0);

    WriteData_AD9959(0x10, 4, ChannelAmplitudeTuningWord7data, 0);

    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);     	 // 写频率控制字
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWord0data, 1);
}

/*******************************************************************************************/
/*	                          八阶调制 CH3                                                 */
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
    uint8_t ChannelFunctionRegisterdata[3] = {0x80, 0x23, 0x30};      	// 无RU/RD
    uint8_t FunctionRegister1data[3] = {0xD0, 0xf2, 0x00};

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);            	// 设置功能寄存器
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);    	// 设置通道功能寄存器

    WrFrequencyTuningWorddata(f1, ChannelFrequencyTuningWord0data);   	// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x04, 4, ChannelFrequencyTuningWord0data, 0); 		// 写入频率控制字
		
    WrFrequencyTuningWorddata(f2, ChannelFrequencyTuningWord1data);		// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0A, 4, ChannelFrequencyTuningWord1data, 0); 		// 写入频率控制字
		
    WrFrequencyTuningWorddata(f3, ChannelFrequencyTuningWord2data);		// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0B, 4, ChannelFrequencyTuningWord2data, 0); 		// 写入频率控制字
		
    WrFrequencyTuningWorddata(f4, ChannelFrequencyTuningWord3data);		// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0C, 4, ChannelFrequencyTuningWord3data, 0); 		// 写入频率控制字
		
    WrFrequencyTuningWorddata(f5, ChannelFrequencyTuningWord4data);		// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0D, 4, ChannelFrequencyTuningWord4data, 0); 		// 写入频率控制字
		
    WrFrequencyTuningWorddata(f6, ChannelFrequencyTuningWord5data);		// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0E, 4, ChannelFrequencyTuningWord5data, 0); 		// 写入频率控制字
		
    WrFrequencyTuningWorddata(f7, ChannelFrequencyTuningWord6data);		// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0F, 4, ChannelFrequencyTuningWord6data, 0); 		// 写入频率控制字
		
    WrFrequencyTuningWorddata(f8, ChannelFrequencyTuningWord7data);		// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x10, 4, ChannelFrequencyTuningWord7data, 1); 		// 写入频率控制字
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

    uint8_t ChannelFunctionRegisterdata[3] = {0xc0, 0x03, 0x30};            // 相位调制模式启用（连续扫描可能开启）
//    uint8_t AmplitudeControldata[3] = {0x00, 0x03, 0xff};                 // 八级调制时启用，默认输出最大
    uint8_t FunctionRegister1data[3] = {0xD0, 0xf2, 0x00};

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);                 // 设置功能寄存器
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);           // 设置通道功能寄存器，即相位调制启动
																			   
    WrPhaseOffsetTuningWorddata(p1, ChannelPhaseOffsetTuningWord0data); 	// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(CPOW0_ADD, 2, ChannelPhaseOffsetTuningWord0data, 1); 	// 写入相位控制字
																			   
    WrPhaseOffsetTuningWorddata(p2, ChannelPhaseOffsetTuningWord1data); 	// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0a, 4, ChannelPhaseOffsetTuningWord1data, 1);    	// 写入相位控制字
																			   
    WrPhaseOffsetTuningWorddata(p3, ChannelPhaseOffsetTuningWord2data); 	// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0b, 4, ChannelPhaseOffsetTuningWord2data, 1); 		// 写入相位控制字
																			   
    WrPhaseOffsetTuningWorddata(p4, ChannelPhaseOffsetTuningWord3data); 	// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0c, 4, ChannelPhaseOffsetTuningWord3data, 1);    	// 写入相位控制字
																			   
    WrPhaseOffsetTuningWorddata(p5, ChannelPhaseOffsetTuningWord4data); 	// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0d, 4, ChannelPhaseOffsetTuningWord4data, 1); 		// 写入相位控制字
																			   
    WrPhaseOffsetTuningWorddata(p6, ChannelPhaseOffsetTuningWord5data); 	// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0e, 4, ChannelPhaseOffsetTuningWord5data, 1);    	// 写入相位控制字
																			   
    WrPhaseOffsetTuningWorddata(p7, ChannelPhaseOffsetTuningWord6data); 	// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0f, 4, ChannelPhaseOffsetTuningWord6data, 1); 		// 写入相位控制字
																			   
    WrPhaseOffsetTuningWorddata(p8, ChannelPhaseOffsetTuningWord7data); 	// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x10, 4, ChannelPhaseOffsetTuningWord7data, 1);    	// 写入相位控制字
    f = 200000;                                                                
    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);     		// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWord0data, 1);     // 写入频率控制字
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

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);         	// 设置功能寄存器
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);   	// 设置通道功能寄存器

    WriteData_AD9959(ACR_ADD, 3, AmplitudeControldata, 0);          	// 设置幅度大小（S1）

    WriteData_AD9959(0x0a, 4, ChannelAmplitudeTuningWord1data, 0);  	// 设置幅度大小（S2）

    WriteData_AD9959(0x0b, 4, ChannelAmplitudeTuningWord2data, 0);

    WriteData_AD9959(0x0c, 4, ChannelAmplitudeTuningWord3data, 0);

    WriteData_AD9959(0x0d, 4, ChannelAmplitudeTuningWord4data, 0);  	// 设置幅度大小（S1）

    WriteData_AD9959(0x0e, 4, ChannelAmplitudeTuningWord5data, 0);  	// 设置幅度大小（S2）

    WriteData_AD9959(0x0f, 4, ChannelAmplitudeTuningWord6data, 0);

    WriteData_AD9959(0x10, 4, ChannelAmplitudeTuningWord7data, 0);

    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);   	// 写频率控制字
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWord0data, 1);
}


/*******************************************************************************************/
/*	                          十六阶调制 CH0                                               */
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
    uint8_t ChannelFunctionRegisterdata[3] = {0x80, 0x23, 0x30};      // 无RU/RD
    uint8_t FunctionRegister1data[3] = {0xD0, 0xc3, 0x00};

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);           // 设置功能寄存器
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);     // 设置通道功能寄存器

    WrFrequencyTuningWorddata(f1, ChannelFrequencyTuningWord0data);   // 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x04, 4, ChannelFrequencyTuningWord0data, 0); 	  // 写入频率控制字

    WrFrequencyTuningWorddata(f2, ChannelFrequencyTuningWord1data);   // 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0A, 4, ChannelFrequencyTuningWord1data, 0); 	  // 写入频率控制字

    WrFrequencyTuningWorddata(f3, ChannelFrequencyTuningWord2data);   // 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0B, 4, ChannelFrequencyTuningWord2data, 0); 	  // 写入频率控制字

    WrFrequencyTuningWorddata(f4, ChannelFrequencyTuningWord3data);   // 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0C, 4, ChannelFrequencyTuningWord3data, 0); 	  // 写入频率控制字

    WrFrequencyTuningWorddata(f5, ChannelFrequencyTuningWord4data);   // 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0D, 4, ChannelFrequencyTuningWord4data, 0); 	  // 写入频率控制字

    WrFrequencyTuningWorddata(f6, ChannelFrequencyTuningWord5data);   // 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0E, 4, ChannelFrequencyTuningWord5data, 0);    // 写入频率控制字

    WrFrequencyTuningWorddata(f7, ChannelFrequencyTuningWord6data);   // 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0F, 4, ChannelFrequencyTuningWord6data, 0);    // 写入频率控制字

    WrFrequencyTuningWorddata(f8, ChannelFrequencyTuningWord7data);   // 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x10, 4, ChannelFrequencyTuningWord7data, 0);    // 写入频率控制字
 
    WrFrequencyTuningWorddata(f9, ChannelFrequencyTuningWord8data);   // 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x11, 4, ChannelFrequencyTuningWord8data, 0);    // 写入频率控制字

    WrFrequencyTuningWorddata(f10, ChannelFrequencyTuningWord9data);  // 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x12, 4, ChannelFrequencyTuningWord9data, 0);    // 写入频率控制字

    WrFrequencyTuningWorddata(f11, ChannelFrequencyTuningWord10data); // 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x13, 4, ChannelFrequencyTuningWord10data, 0);   // 写入频率控制字

    WrFrequencyTuningWorddata(f12, ChannelFrequencyTuningWord11data); // 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x14, 4, ChannelFrequencyTuningWord11data, 0);   // 写入频率控制字

    WrFrequencyTuningWorddata(f13, ChannelFrequencyTuningWord12data); // 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x15, 4, ChannelFrequencyTuningWord12data, 0);   // 写入频率控制字

    WrFrequencyTuningWorddata(f14, ChannelFrequencyTuningWord13data); // 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x16, 4, ChannelFrequencyTuningWord13data, 0);   // 写入频率控制字

    WrFrequencyTuningWorddata(f15, ChannelFrequencyTuningWord14data); // 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x017, 4, ChannelFrequencyTuningWord14data, 0);  // 写入频率控制字

    WrFrequencyTuningWorddata(f16, ChannelFrequencyTuningWord15data); // 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x18, 4, ChannelFrequencyTuningWord15data, 1);   // 写入频率控制字
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

    uint8_t ChannelFunctionRegisterdata[3] = {0xc0, 0x03, 0x30};                 	// 相位调制模式启用（连续扫描可能开启）
//    uint8_t AmplitudeControldata[3] = {0x00, 0x03, 0xff};                      	// 十六级调制时启用，默认输出最大
    uint8_t FunctionRegister1data[3] = {0xD0, 0xc3, 0x00};

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);                   		// 设置功能寄存器
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);           		// 设置通道功能寄存器，即相位调制启动

    WrPhaseOffsetTuningWorddata(p1, ChannelPhaseOffsetTuningWord0data); 			// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(CPOW0_ADD, 2, ChannelPhaseOffsetTuningWord0data, 1); 			// 写入相位控制字

    WrPhaseOffsetTuningWorddata(p2, ChannelPhaseOffsetTuningWord1data); 			// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0a, 4, ChannelPhaseOffsetTuningWord1data, 1);    			// 写入相位控制字

    WrPhaseOffsetTuningWorddata(p3, ChannelPhaseOffsetTuningWord2data); 			// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0b, 4, ChannelPhaseOffsetTuningWord2data, 1); 				// 写入相位控制字

    WrPhaseOffsetTuningWorddata(p4, ChannelPhaseOffsetTuningWord3data); 			// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0c, 4, ChannelPhaseOffsetTuningWord3data, 1);    			// 写入相位控制字

    WrPhaseOffsetTuningWorddata(p5, ChannelPhaseOffsetTuningWord4data); 			// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0d, 4, ChannelPhaseOffsetTuningWord4data, 1); 				// 写入相位控制字

    WrPhaseOffsetTuningWorddata(p6, ChannelPhaseOffsetTuningWord5data); 			// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0e, 4, ChannelPhaseOffsetTuningWord5data, 1);    			// 写入相位控制字

    WrPhaseOffsetTuningWorddata(p7, ChannelPhaseOffsetTuningWord6data); 			// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0f, 4, ChannelPhaseOffsetTuningWord6data, 1); 				// 写入相位控制字

    WrPhaseOffsetTuningWorddata(p8, ChannelPhaseOffsetTuningWord7data); 			// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x10, 4, ChannelPhaseOffsetTuningWord7data, 1);    			// 写入相位控制字

    WrPhaseOffsetTuningWorddata(p9, ChannelPhaseOffsetTuningWord8data); 			// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x11, 4, ChannelPhaseOffsetTuningWord8data, 1); 				// 写入相位控制字

    WrPhaseOffsetTuningWorddata(p10, ChannelPhaseOffsetTuningWord9data); 			// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x12, 4, ChannelPhaseOffsetTuningWord9data, 1);    			// 写入相位控制字

    WrPhaseOffsetTuningWorddata(p11, ChannelPhaseOffsetTuningWord10data); 			// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x13, 4, ChannelPhaseOffsetTuningWord10data, 1); 				// 写入相位控制字

    WrPhaseOffsetTuningWorddata(p12, ChannelPhaseOffsetTuningWord11data); 			// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x14, 4, ChannelPhaseOffsetTuningWord11data, 1);   			// 写入相位控制字

    WrPhaseOffsetTuningWorddata(p13, ChannelPhaseOffsetTuningWord12data); 			// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x15, 4, ChannelPhaseOffsetTuningWord12data, 1); 				// 写入相位控制字

    WrPhaseOffsetTuningWorddata(p14, ChannelPhaseOffsetTuningWord13data); 			// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x16, 4, ChannelPhaseOffsetTuningWord13data, 1);   			// 写入相位控制字

    WrPhaseOffsetTuningWorddata(p15, ChannelPhaseOffsetTuningWord14data); 			// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x17, 4, ChannelPhaseOffsetTuningWord14data, 1); 				// 写入相位控制字

    WrPhaseOffsetTuningWorddata(p16, ChannelPhaseOffsetTuningWord15data); 			// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x18, 4, ChannelPhaseOffsetTuningWord15data, 1);   			// 写入相位控制字

    f = 200000;
    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);     				// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWord0data, 1);     		// 写入频率控制字
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

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);               // 设置功能寄存器
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);         // 设置通道功能寄存器

    WriteData_AD9959(ACR_ADD, 3, AmplitudeControldata, 0);                // 设置幅度大小（S1）

    WriteData_AD9959(0x0a, 4, ChannelAmplitudeTuningWord1data, 0); 		  // 设置幅度大小（S2）

    WriteData_AD9959(0x0b, 4, ChannelAmplitudeTuningWord2data, 0);

    WriteData_AD9959(0x0c, 4, ChannelAmplitudeTuningWord3data, 0);

    WriteData_AD9959(0x0d, 4, ChannelAmplitudeTuningWord4data, 0);        // 设置幅度大小（S1）

    WriteData_AD9959(0x0e, 4, ChannelAmplitudeTuningWord5data, 0); 		  // 设置幅度大小（S2）

    WriteData_AD9959(0x0f, 4, ChannelAmplitudeTuningWord6data, 0);

    WriteData_AD9959(0x10, 4, ChannelAmplitudeTuningWord7data, 0);

    WriteData_AD9959(0x11, 4, ChannelAmplitudeTuningWord8data, 0);        // 设置幅度大小（S1）

    WriteData_AD9959(0x12, 4, ChannelAmplitudeTuningWord9data, 0); 		  // 设置幅度大小（S2）

    WriteData_AD9959(0x13, 4, ChannelAmplitudeTuningWord10data, 0);

    WriteData_AD9959(0x14, 4, ChannelAmplitudeTuningWord11data, 0);

    WriteData_AD9959(0x15, 4, ChannelAmplitudeTuningWord12data, 0);        // 设置幅度大小（S1）

    WriteData_AD9959(0x16, 4, ChannelAmplitudeTuningWord13data, 0); 	   // 设置幅度大小（S2）

    WriteData_AD9959(0x17, 4, ChannelAmplitudeTuningWord14data, 0);

    WriteData_AD9959(0x18, 4, ChannelAmplitudeTuningWord15data, 0);

    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);     	   // 写频率控制字
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWord0data, 1);
}

/*******************************************************************************************/
/*	                          十六阶调制 CH1                                               */
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
    uint8_t ChannelFunctionRegisterdata[3] = {0x80, 0x23, 0x30};      	// 无RU/RD
    uint8_t FunctionRegister1data[3] = {0xD0, 0xd3, 0x00};                 
																		   
    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);            	// 设置功能寄存器
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);    	// 设置通道功能寄存器
																		   
    WrFrequencyTuningWorddata(f1, ChannelFrequencyTuningWord0data);   	// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x04, 4, ChannelFrequencyTuningWord0data, 0); 		// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f2, ChannelFrequencyTuningWord1data);   	// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0A, 4, ChannelFrequencyTuningWord1data, 0); 		// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f3, ChannelFrequencyTuningWord2data);   	// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0B, 4, ChannelFrequencyTuningWord2data, 0); 		// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f4, ChannelFrequencyTuningWord3data);   	// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0C, 4, ChannelFrequencyTuningWord3data, 0); 		// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f5, ChannelFrequencyTuningWord4data);   	// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0D, 4, ChannelFrequencyTuningWord4data, 0); 		// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f6, ChannelFrequencyTuningWord5data);   	// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0E, 4, ChannelFrequencyTuningWord5data, 0); 		// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f7, ChannelFrequencyTuningWord6data);   	// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0F, 4, ChannelFrequencyTuningWord6data, 0); 		// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f8, ChannelFrequencyTuningWord7data);   	// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x10, 4, ChannelFrequencyTuningWord7data, 0); 		// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f9, ChannelFrequencyTuningWord8data);   	// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x11, 4, ChannelFrequencyTuningWord8data, 0); 		// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f10, ChannelFrequencyTuningWord9data);   	// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x12, 4, ChannelFrequencyTuningWord9data, 0);	 	// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f11, ChannelFrequencyTuningWord10data);   // 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x13, 4, ChannelFrequencyTuningWord10data, 0); 	// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f12, ChannelFrequencyTuningWord11data);   // 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x14, 4, ChannelFrequencyTuningWord11data, 0); 	// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f13, ChannelFrequencyTuningWord12data);   // 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x15, 4, ChannelFrequencyTuningWord12data, 0); 	// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f14, ChannelFrequencyTuningWord13data);   // 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x16, 4, ChannelFrequencyTuningWord13data, 0); 	// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f15, ChannelFrequencyTuningWord14data);   // 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x017, 4, ChannelFrequencyTuningWord14data, 0); 	// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f16, ChannelFrequencyTuningWord15data);   // 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x18, 4, ChannelFrequencyTuningWord15data, 1); 	// 写入频率控制字
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

    uint8_t ChannelFunctionRegisterdata[3] = {0xc0, 0x03, 0x30};                // 相位调制模式启用（连续扫描可能开启）
//    uint8_t AmplitudeControldata[3] = {0x00, 0x03, 0xff};                     // 十六级调制时启用，默认输出最大
    uint8_t FunctionRegister1data[3] = {0xD0, 0xd3, 0x00};

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);                  	// 设置功能寄存器
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);           	// 设置通道功能寄存器，即相位调制启动

    WrPhaseOffsetTuningWorddata(p1, ChannelPhaseOffsetTuningWord0data); 		// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(CPOW0_ADD, 2, ChannelPhaseOffsetTuningWord0data, 1); 		// 写入相位控制字
																				   
    WrPhaseOffsetTuningWorddata(p2, ChannelPhaseOffsetTuningWord1data); 		// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0a, 4, ChannelPhaseOffsetTuningWord1data, 1);    		// 写入相位控制字
																				   
    WrPhaseOffsetTuningWorddata(p3, ChannelPhaseOffsetTuningWord2data); 		// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0b, 4, ChannelPhaseOffsetTuningWord2data, 1); 			// 写入相位控制字
																				  
    WrPhaseOffsetTuningWorddata(p4, ChannelPhaseOffsetTuningWord3data); 		// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0c, 4, ChannelPhaseOffsetTuningWord3data, 1);   			// 写入相位控制字
																				   
    WrPhaseOffsetTuningWorddata(p5, ChannelPhaseOffsetTuningWord4data); 		// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0d, 4, ChannelPhaseOffsetTuningWord4data, 1); 			// 写入相位控制字
																				   
    WrPhaseOffsetTuningWorddata(p6, ChannelPhaseOffsetTuningWord5data); 		// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0e, 4, ChannelPhaseOffsetTuningWord5data, 1);    		// 写入相位控制字
																				   
    WrPhaseOffsetTuningWorddata(p7, ChannelPhaseOffsetTuningWord6data); 		// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0f, 4, ChannelPhaseOffsetTuningWord6data, 1); 			// 写入相位控制字
																				   
    WrPhaseOffsetTuningWorddata(p8, ChannelPhaseOffsetTuningWord7data); 		// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x10, 4, ChannelPhaseOffsetTuningWord7data, 1);    		// 写入相位控制字
																				   
    WrPhaseOffsetTuningWorddata(p9, ChannelPhaseOffsetTuningWord8data); 		// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x11, 4, ChannelPhaseOffsetTuningWord8data, 1); 			// 写入相位控制字
																				   
    WrPhaseOffsetTuningWorddata(p10, ChannelPhaseOffsetTuningWord9data); 		// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x12, 4, ChannelPhaseOffsetTuningWord9data, 1);    		// 写入相位控制字
																				   
    WrPhaseOffsetTuningWorddata(p11, ChannelPhaseOffsetTuningWord10data); 		// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x13, 4, ChannelPhaseOffsetTuningWord10data, 1); 			// 写入相位控制字
																				   
    WrPhaseOffsetTuningWorddata(p12, ChannelPhaseOffsetTuningWord11data); 		// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x14, 4, ChannelPhaseOffsetTuningWord11data, 1);   		// 写入相位控制字
																				   
    WrPhaseOffsetTuningWorddata(p13, ChannelPhaseOffsetTuningWord12data); 		// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x15, 4, ChannelPhaseOffsetTuningWord12data, 1); 			// 写入相位控制字
																				   
    WrPhaseOffsetTuningWorddata(p14, ChannelPhaseOffsetTuningWord13data); 		// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x16, 4, ChannelPhaseOffsetTuningWord13data, 1);   		// 写入相位控制字
																				   
    WrPhaseOffsetTuningWorddata(p15, ChannelPhaseOffsetTuningWord14data); 		// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x17, 4, ChannelPhaseOffsetTuningWord14data, 1); 			// 写入相位控制字
																				   
    WrPhaseOffsetTuningWorddata(p16, ChannelPhaseOffsetTuningWord15data); 		// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x18, 4, ChannelPhaseOffsetTuningWord15data, 1);   		// 写入相位控制字
																				   
    f = 200000;                                                                    
    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);     			// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWord0data, 1);     	// 写入频率控制字
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

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);              			// 设置功能寄存器
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);        			// 设置通道功能寄存器

    WriteData_AD9959(ACR_ADD, 3, AmplitudeControldata, 0);                      	//设置幅度大小（S1）

    WriteData_AD9959(0x0a, 4, ChannelAmplitudeTuningWord1data, 0); 					// 设置幅度大小（S2）

    WriteData_AD9959(0x0b, 4, ChannelAmplitudeTuningWord2data, 0);

    WriteData_AD9959(0x0c, 4, ChannelAmplitudeTuningWord3data, 0);

    WriteData_AD9959(0x0d, 4, ChannelAmplitudeTuningWord4data, 0);                  // 设置幅度大小（S1）

    WriteData_AD9959(0x0e, 4, ChannelAmplitudeTuningWord5data, 0); 					// 设置幅度大小（S2）

    WriteData_AD9959(0x0f, 4, ChannelAmplitudeTuningWord6data, 0);

    WriteData_AD9959(0x10, 4, ChannelAmplitudeTuningWord7data, 0);

    WriteData_AD9959(0x11, 4, ChannelAmplitudeTuningWord8data, 0);                  // 设置幅度大小（S1）

    WriteData_AD9959(0x12, 4, ChannelAmplitudeTuningWord9data, 0); 					// 设置幅度大小（S2）

    WriteData_AD9959(0x13, 4, ChannelAmplitudeTuningWord10data, 0);

    WriteData_AD9959(0x14, 4, ChannelAmplitudeTuningWord11data, 0);

    WriteData_AD9959(0x15, 4, ChannelAmplitudeTuningWord12data, 0);                  // 设置幅度大小（S1）

    WriteData_AD9959(0x16, 4, ChannelAmplitudeTuningWord13data, 0); 				 // 设置幅度大小（S2）

    WriteData_AD9959(0x17, 4, ChannelAmplitudeTuningWord14data, 0);

    WriteData_AD9959(0x18, 4, ChannelAmplitudeTuningWord15data, 0);

    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);     				 // 写频率控制字
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWord0data, 1);
}

/*******************************************************************************************/
/*	                          十六阶调制 CH2                                               */
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
    uint8_t ChannelFunctionRegisterdata[3] = {0x80, 0x23, 0x30};      	// 无RU/RD
    uint8_t FunctionRegister1data[3] = {0xD0, 0xe3, 0x00};

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);            	// 设置功能寄存器
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);    	// 设置通道功能寄存器
																		   
    WrFrequencyTuningWorddata(f1, ChannelFrequencyTuningWord0data);   	// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x04, 4, ChannelFrequencyTuningWord0data, 0); 		// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f2, ChannelFrequencyTuningWord1data);   	// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0A, 4, ChannelFrequencyTuningWord1data, 0); 		// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f3, ChannelFrequencyTuningWord2data);   	// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0B, 4, ChannelFrequencyTuningWord2data, 0); 		// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f4, ChannelFrequencyTuningWord3data);   	// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0C, 4, ChannelFrequencyTuningWord3data, 0); 		// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f5, ChannelFrequencyTuningWord4data);   	// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0D, 4, ChannelFrequencyTuningWord4data, 0); 		// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f6, ChannelFrequencyTuningWord5data);   	// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0E, 4, ChannelFrequencyTuningWord5data, 0); 		// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f7, ChannelFrequencyTuningWord6data);   	// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0F, 4, ChannelFrequencyTuningWord6data, 0); 		// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f8, ChannelFrequencyTuningWord7data);   	// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x10, 4, ChannelFrequencyTuningWord7data, 0); 		// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f9, ChannelFrequencyTuningWord8data);   	// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x11, 4, ChannelFrequencyTuningWord8data, 0); 		// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f10, ChannelFrequencyTuningWord9data);   	// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x12, 4, ChannelFrequencyTuningWord9data, 0); 		// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f11, ChannelFrequencyTuningWord10data);   // 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x13, 4, ChannelFrequencyTuningWord10data, 0); 	// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f12, ChannelFrequencyTuningWord11data);   // 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x14, 4, ChannelFrequencyTuningWord11data, 0); 	// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f13, ChannelFrequencyTuningWord12data);   // 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x15, 4, ChannelFrequencyTuningWord12data, 0); 	// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f14, ChannelFrequencyTuningWord13data);   // 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x16, 4, ChannelFrequencyTuningWord13data, 0); 	// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f15, ChannelFrequencyTuningWord14data);   // 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x017, 4, ChannelFrequencyTuningWord14data, 0); 	// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f16, ChannelFrequencyTuningWord15data);   // 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x18, 4, ChannelFrequencyTuningWord15data, 1); 	// 写入频率控制字
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
 
    uint8_t ChannelFunctionRegisterdata[3] = {0xc0, 0x03, 0x30};                 	// 相位调制模式启用（连续扫描可能开启）
//    uint8_t AmplitudeControldata[3] = {0x00, 0x03, 0xff};                         // 十六级调制时启用，默认输出最大
    uint8_t FunctionRegister1data[3] = {0xD0, 0xe3, 0x00};

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);                   		// 设置功能寄存器
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);           		// 设置通道功能寄存器，即相位调制启动

    WrPhaseOffsetTuningWorddata(p1, ChannelPhaseOffsetTuningWord0data); 			// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(CPOW0_ADD, 2, ChannelPhaseOffsetTuningWord0data, 1); 			// 写入相位控制字

    WrPhaseOffsetTuningWorddata(p2, ChannelPhaseOffsetTuningWord1data); 			// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0a, 4, ChannelPhaseOffsetTuningWord1data, 1);    			// 写入相位控制字

    WrPhaseOffsetTuningWorddata(p3, ChannelPhaseOffsetTuningWord2data); 			// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0b, 4, ChannelPhaseOffsetTuningWord2data, 1);	 			// 写入相位控制字

    WrPhaseOffsetTuningWorddata(p4, ChannelPhaseOffsetTuningWord3data); 			// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0c, 4, ChannelPhaseOffsetTuningWord3data, 1);    			// 写入相位控制字

    WrPhaseOffsetTuningWorddata(p5, ChannelPhaseOffsetTuningWord4data); 		    // 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0d, 4, ChannelPhaseOffsetTuningWord4data, 1); 			    // 写入相位控制字
 
    WrPhaseOffsetTuningWorddata(p6, ChannelPhaseOffsetTuningWord5data); 			// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0e, 4, ChannelPhaseOffsetTuningWord5data, 1);    			// 写入相位控制字
																					   
    WrPhaseOffsetTuningWorddata(p7, ChannelPhaseOffsetTuningWord6data); 			// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0f, 4, ChannelPhaseOffsetTuningWord6data, 1); 				// 写入相位控制字
																					   
    WrPhaseOffsetTuningWorddata(p8, ChannelPhaseOffsetTuningWord7data); 			// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x10, 4, ChannelPhaseOffsetTuningWord7data, 1);    			// 写入相位控制字
																					   
    WrPhaseOffsetTuningWorddata(p9, ChannelPhaseOffsetTuningWord8data); 			// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x11, 4, ChannelPhaseOffsetTuningWord8data, 1); 				// 写入相位控制字
																					   
    WrPhaseOffsetTuningWorddata(p10, ChannelPhaseOffsetTuningWord9data); 			// 将十进制相位数转换为2进制相位控制字?
    WriteData_AD9959(0x12, 4, ChannelPhaseOffsetTuningWord9data, 1);    			// 写入相位控制字

    WrPhaseOffsetTuningWorddata(p11, ChannelPhaseOffsetTuningWord10data); 			// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x13, 4, ChannelPhaseOffsetTuningWord10data, 1); 				// 写入相位控制字

    WrPhaseOffsetTuningWorddata(p12, ChannelPhaseOffsetTuningWord11data); 			// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x14, 4, ChannelPhaseOffsetTuningWord11data, 1);   			// 写入相位控制字

    WrPhaseOffsetTuningWorddata(p13, ChannelPhaseOffsetTuningWord12data);	 		// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x15, 4, ChannelPhaseOffsetTuningWord12data, 1); 			    // 写入相位控制字

    WrPhaseOffsetTuningWorddata(p14, ChannelPhaseOffsetTuningWord13data); 			// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x16, 4, ChannelPhaseOffsetTuningWord13data, 1);     			// 写入相位控制字

    WrPhaseOffsetTuningWorddata(p15, ChannelPhaseOffsetTuningWord14data); 			// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x17, 4, ChannelPhaseOffsetTuningWord14data, 1); 				// 写入相位控制字

    WrPhaseOffsetTuningWorddata(p16, ChannelPhaseOffsetTuningWord15data); 			// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x18, 4, ChannelPhaseOffsetTuningWord15data, 1);   			// 写入相位控制字

    f = 200000;
    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);     				// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWord0data, 1);     		// 写入频率控制字
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

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);              // 设置功能寄存器
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);        // 设置通道功能寄存器

    WriteData_AD9959(ACR_ADD, 3, AmplitudeControldata, 0);               // 设置幅度大小（S1）

    WriteData_AD9959(0x0a, 4, ChannelAmplitudeTuningWord1data, 0); 		 // 设置幅度大小（S2）

    WriteData_AD9959(0x0b, 4, ChannelAmplitudeTuningWord2data, 0);

    WriteData_AD9959(0x0c, 4, ChannelAmplitudeTuningWord3data, 0);

    WriteData_AD9959(0x0d, 4, ChannelAmplitudeTuningWord4data, 0);        // 设置幅度大小（S1）

    WriteData_AD9959(0x0e, 4, ChannelAmplitudeTuningWord5data, 0); 		  // 设置幅度大小（S2）

    WriteData_AD9959(0x0f, 4, ChannelAmplitudeTuningWord6data, 0);

    WriteData_AD9959(0x10, 4, ChannelAmplitudeTuningWord7data, 0);

    WriteData_AD9959(0x11, 4, ChannelAmplitudeTuningWord8data, 0);        // 设置幅度大小（S1）

    WriteData_AD9959(0x12, 4, ChannelAmplitudeTuningWord9data, 0); 		  // 设置幅度大小（S2）

    WriteData_AD9959(0x13, 4, ChannelAmplitudeTuningWord10data, 0);

    WriteData_AD9959(0x14, 4, ChannelAmplitudeTuningWord11data, 0);

    WriteData_AD9959(0x15, 4, ChannelAmplitudeTuningWord12data, 0);       // 设置幅度大小（S1）

    WriteData_AD9959(0x16, 4, ChannelAmplitudeTuningWord13data, 0); 	  // 设置幅度大小（S2）

    WriteData_AD9959(0x17, 4, ChannelAmplitudeTuningWord14data, 0);

    WriteData_AD9959(0x18, 4, ChannelAmplitudeTuningWord15data, 0);

    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);    	  // 写频率控制字
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWord0data, 1);
}

/*******************************************************************************************/
/*	                          十六阶调制 CH3                                               */
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
    uint8_t ChannelFunctionRegisterdata[3] = {0x80, 0x23, 0x30};      	// 无RU/RD
    uint8_t FunctionRegister1data[3] = {0xD0, 0xf3, 0x00};                 
																		   
    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);           	// 设置功能寄存器
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);    	// 设置通道功能寄存器
																		   
    WrFrequencyTuningWorddata(f1, ChannelFrequencyTuningWord0data);   	// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x04, 4, ChannelFrequencyTuningWord0data, 0); 		// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f2, ChannelFrequencyTuningWord1data);		// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0A, 4, ChannelFrequencyTuningWord1data, 0); 		// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f3, ChannelFrequencyTuningWord2data);		// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0B, 4, ChannelFrequencyTuningWord2data, 0); 		// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f4, ChannelFrequencyTuningWord3data);		// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0C, 4, ChannelFrequencyTuningWord3data, 0); 		// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f5, ChannelFrequencyTuningWord4data);		// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0D, 4, ChannelFrequencyTuningWord4data, 0); 		// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f6, ChannelFrequencyTuningWord5data);		// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0E, 4, ChannelFrequencyTuningWord5data, 0); 		// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f7, ChannelFrequencyTuningWord6data);		// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x0F, 4, ChannelFrequencyTuningWord6data, 0); 		// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f8, ChannelFrequencyTuningWord7data);		// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x10, 4, ChannelFrequencyTuningWord7data, 0); 		// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f9, ChannelFrequencyTuningWord8data);		// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x11, 4, ChannelFrequencyTuningWord8data, 0); 		// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f10, ChannelFrequencyTuningWord9data);   	// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x12, 4, ChannelFrequencyTuningWord9data, 0);     	// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f11, ChannelFrequencyTuningWord10data);   // 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x13, 4, ChannelFrequencyTuningWord10data, 0); 	// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f12, ChannelFrequencyTuningWord11data);   // 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x14, 4, ChannelFrequencyTuningWord11data, 0); 	// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f13, ChannelFrequencyTuningWord12data);   // 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x15, 4, ChannelFrequencyTuningWord12data, 0); 	// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f14, ChannelFrequencyTuningWord13data);   // 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x16, 4, ChannelFrequencyTuningWord13data, 0); 	// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f15, ChannelFrequencyTuningWord14data);   // 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x017, 4, ChannelFrequencyTuningWord14data, 0);	// 写入频率控制字
																		   
    WrFrequencyTuningWorddata(f16, ChannelFrequencyTuningWord15data);   // 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(0x18, 4, ChannelFrequencyTuningWord15data, 1); 	// 写入频率控制字
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

    uint8_t ChannelFunctionRegisterdata[3] = {0xc0, 0x03, 0x30};                 	// 相位调制模式启用（连续扫描可能开启）
//    uint8_t AmplitudeControldata[3] = {0x00, 0x03, 0xff};                         // 十六级调制时启用，默认输出最大
    uint8_t FunctionRegister1data[3] = {0xD0, 0xf3, 0x00};

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);                   		// 设置功能寄存器
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);           		// 设置通道功能寄存器，即相位调制启动

    WrPhaseOffsetTuningWorddata(p1, ChannelPhaseOffsetTuningWord0data); 			// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(CPOW0_ADD, 2, ChannelPhaseOffsetTuningWord0data, 1); 			// 写入相位控制字

    WrPhaseOffsetTuningWorddata(p2, ChannelPhaseOffsetTuningWord1data); 			// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0a, 4, ChannelPhaseOffsetTuningWord1data, 1);    			// 写入相位控制字
	
    WrPhaseOffsetTuningWorddata(p3, ChannelPhaseOffsetTuningWord2data); 			// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0b, 4, ChannelPhaseOffsetTuningWord2data, 1); 				// 写入相位控制字

    WrPhaseOffsetTuningWorddata(p4, ChannelPhaseOffsetTuningWord3data); 			// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0c, 4, ChannelPhaseOffsetTuningWord3data, 1);    			// 写入相位控制字

    WrPhaseOffsetTuningWorddata(p5, ChannelPhaseOffsetTuningWord4data); 			//将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0d, 4, ChannelPhaseOffsetTuningWord4data, 1); 				//写入相位控制字

    WrPhaseOffsetTuningWorddata(p6, ChannelPhaseOffsetTuningWord5data); 			// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0e, 4, ChannelPhaseOffsetTuningWord5data, 1);    			// 写入相位控制字

    WrPhaseOffsetTuningWorddata(p7, ChannelPhaseOffsetTuningWord6data); 			// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x0f, 4, ChannelPhaseOffsetTuningWord6data, 1); 				// 写入相位控制字

    WrPhaseOffsetTuningWorddata(p8, ChannelPhaseOffsetTuningWord7data); 			// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x10, 4, ChannelPhaseOffsetTuningWord7data, 1);    			// 写入相位控制字

    WrPhaseOffsetTuningWorddata(p9, ChannelPhaseOffsetTuningWord8data); 			// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x11, 4, ChannelPhaseOffsetTuningWord8data, 1);			 	// 写入相位控制字

    WrPhaseOffsetTuningWorddata(p10, ChannelPhaseOffsetTuningWord9data); 			// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x12, 4, ChannelPhaseOffsetTuningWord9data, 1);    			// 写入相位控制字

    WrPhaseOffsetTuningWorddata(p11, ChannelPhaseOffsetTuningWord10data); 			// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x13, 4, ChannelPhaseOffsetTuningWord10data, 1); 				// 写入相位控制字

    WrPhaseOffsetTuningWorddata(p12, ChannelPhaseOffsetTuningWord11data); 			// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x14, 4, ChannelPhaseOffsetTuningWord11data, 1);   			// 写入相位控制字

    WrPhaseOffsetTuningWorddata(p13, ChannelPhaseOffsetTuningWord12data); 			// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x15, 4, ChannelPhaseOffsetTuningWord12data, 1); 				// 写入相位控制字

    WrPhaseOffsetTuningWorddata(p14, ChannelPhaseOffsetTuningWord13data); 			// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x16, 4, ChannelPhaseOffsetTuningWord13data, 1);   			// 写入相位控制字

    WrPhaseOffsetTuningWorddata(p15, ChannelPhaseOffsetTuningWord14data); 			// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x17, 4, ChannelPhaseOffsetTuningWord14data, 1);			    // 写入相位控制字

    WrPhaseOffsetTuningWorddata(p16, ChannelPhaseOffsetTuningWord15data); 			// 将十进制相位数转换为2进制相位控制字
    WriteData_AD9959(0x18, 4, ChannelPhaseOffsetTuningWord15data, 1);   			// 写入相位控制字

    f = 200000;
    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);     				// 将十进制频率数转换为2进制频率控制字
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWord0data, 1);     		// 写入频率控制字
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

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);              			// 设置功能寄存器
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);        			// 设置通道功能寄存器

    WriteData_AD9959(ACR_ADD, 3, AmplitudeControldata, 0);                       	// 设置幅度大小（S1）

    WriteData_AD9959(0x0a, 4, ChannelAmplitudeTuningWord1data, 0); 					// 设置幅度大小（S2）

    WriteData_AD9959(0x0b, 4, ChannelAmplitudeTuningWord2data, 0);

    WriteData_AD9959(0x0c, 4, ChannelAmplitudeTuningWord3data, 0);

    WriteData_AD9959(0x0d, 4, ChannelAmplitudeTuningWord4data, 0);                  // 设置幅度大小（S1）

    WriteData_AD9959(0x0e, 4, ChannelAmplitudeTuningWord5data, 0); 					// 设置幅度大小（S2）

    WriteData_AD9959(0x0f, 4, ChannelAmplitudeTuningWord6data, 0);

    WriteData_AD9959(0x10, 4, ChannelAmplitudeTuningWord7data, 0);

    WriteData_AD9959(0x11, 4, ChannelAmplitudeTuningWord8data, 0);                  // 设置幅度大小（S1）

    WriteData_AD9959(0x12, 4, ChannelAmplitudeTuningWord9data, 0); 					// 设置幅度大小（S2）

    WriteData_AD9959(0x13, 4, ChannelAmplitudeTuningWord10data, 0);

    WriteData_AD9959(0x14, 4, ChannelAmplitudeTuningWord11data, 0);

    WriteData_AD9959(0x15, 4, ChannelAmplitudeTuningWord12data, 0);                 // 设置幅度大小（S1）

    WriteData_AD9959(0x16, 4, ChannelAmplitudeTuningWord13data, 0); 				// 设置幅度大小（S2）

    WriteData_AD9959(0x17, 4, ChannelAmplitudeTuningWord14data, 0);

    WriteData_AD9959(0x18, 4, ChannelAmplitudeTuningWord15data, 0);

    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);     				// 写频率控制字
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWord0data, 1);
}

/*******************************************************************************************/
/*	                          线性扫描模式相关函数                                         */
/*******************************************************************************************/

/**
 * @name	  AD9959_Frequency_Sweep
 * @brief     频率扫描模式
 * @param	  f1：起始频率，
			  f2：终止频率，
			  a1：上升δ，
			  a2：下降δ
 * @return    空
 * @DateTime  2019-7-19
 */
void AD9959_Frequency_Sweep(double f1, double f2, double a1, double a2) // f1起始频率，f2终止频率，a1上升δ，a2下降δ
{
    uint8_t ChannelFrequencyTuningWorddata1[4];
    uint8_t ChannelFrequencyTuningWorddata2[4];
    uint8_t ChannelFrequencyTuningWorddata3[4];
    uint8_t ChannelFrequencyTuningWorddata4[4];
    uint8_t FunctionRegister1data[3] = {0xD0, 0x00, 0x00};                         	// 默认情况下选择
    uint8_t ChannelFunctionRegisterdata[3] = {0x80, 0x43, 0x20};                   	// 扫频模式时选择;连续扫描没有启用，无RU/RD
    uint8_t SweepRampRatedata[2] = {0xff, 0xff};                                   	// 默认单位扫描时间最长
//    uint8_t FunctionRegister1data[3] = {0xD0,0x04,0x00};                          // 线性扫描时需要RU/RD时选择开启
//    uint8_t AmplitudeControldata[3] = {0xff,0x1f,0xff};                           // 开启RU/RD

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);                       	// 设置功能寄存器
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);                 	// 设置通道功能寄存器
//  WriteData_AD9959(ACR,3,AmplitudeControldata,1);                           		// 需输出幅度RU/RD模式才选择,且要修改功能寄存器FR1

	//  f=500000;    // 设置起始频率S0（0x04）
    WrFrequencyTuningWorddata(f1, ChannelFrequencyTuningWorddata1);         	 	// 写频率控制字
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWorddata1, 0);
	//  f=5000000; //设置终止频率(0x0A)
    WrFrequencyTuningWorddata(f2, ChannelFrequencyTuningWorddata2);    			  	// 写频率控制字
    WriteData_AD9959(0x0A, 4, ChannelFrequencyTuningWorddata2, 0);
	//  f=100;   //设置上升δ
    WrFrequencyTuningWorddata(a1, ChannelFrequencyTuningWorddata3);      		  	// 写频率控制字
    WriteData_AD9959(RDW_ADD, 4, ChannelFrequencyTuningWorddata3, 0);
	//   f=100;  //设置下降δ
    WrFrequencyTuningWorddata(a2, ChannelFrequencyTuningWorddata4);      		  	// 写频率控制字
    WriteData_AD9959(FDW_ADD, 4, ChannelFrequencyTuningWorddata4, 0);

    WriteData_AD9959(LSRR_ADD, 2, SweepRampRatedata, 1);                       		// 设置单位步进时间

}

/**
 * @name	  AD9959_Phase_Sweep
 * @brief     相位扫描模式
 * @param	  p1：起始相位，
			  p2：终止相位，
              a1：设置上升δ，
              a2：设置下降δ
 * @return    空
 * @DateTime  2019-7-19
 */
void AD9959_Phase_Sweep(int p1, int p2, int a1, int a2, double f)   				// p1起始相位，p2终止相位，a1设置上升δ，a2设置下降δ
{
    uint8_t ChannelPhaseOffsetTuningWorddata1[2];
    uint8_t ChannelPhaseOffsetTuningWorddata2[2];
    uint8_t ChannelPhaseOffsetTuningWorddata3[2];
    uint8_t ChannelPhaseOffsetTuningWorddata4[2];
    uint8_t ChannelFrequencyTuningWord0data[4];

    uint8_t FunctionRegister1data[3] = {0xD0, 0x00, 0x00};                          // 默认情况下选择
    uint8_t ChannelFunctionRegisterdata[3] = {0xc0, 0xC3, 0x30};                   	// 扫相模式时选择，连续扫描没有启用，无RU/RD
    uint8_t SweepRampRatedata[2] = {0xff, 0xff};                                    // 默认单位扫描时间最长
//uint8_t FunctionRegister1data[3] = {0xD0,0x04,0x00};                             	// 线性扫描时需要RU/RD时选择开启
// uint8_t AmplitudeControldata[3] = {0xff,0x1f,0xff};                             	// 开启RU/RD

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);                   		// 设置功能寄存器
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);            		// 设置通道功能寄存器
//   WriteData_AD9959(ACR_ADD,3,AmplitudeControldata,1);                          	// 需输出幅度RU/RD模式才选择,且要修改功能寄存器FR1

	//   p=0;// 设置起始相位S0（0x04）   数据类型与子函数里面不一样
    WrPhaseOffsetTuningWorddata(p1, ChannelPhaseOffsetTuningWorddata1);
    WriteData_AD9959(CPOW0_ADD, 2, ChannelPhaseOffsetTuningWorddata1, 0);
	//    p=360;// 设置终止相位E0(0x0A)
    WrPhaseOffsetTuningWorddata(p2, ChannelPhaseOffsetTuningWorddata2);
    WriteData_AD9959(0x0a, 4, ChannelPhaseOffsetTuningWorddata2, 0);
	//    p=10;// 设置上升δ
    WrPhaseOffsetTuningWorddata(a1, ChannelPhaseOffsetTuningWorddata3);
    WriteData_AD9959(RDW_ADD, 4, ChannelPhaseOffsetTuningWorddata3, 0);
	//    p=10;// 设置下降δ
    WrPhaseOffsetTuningWorddata(a2, ChannelPhaseOffsetTuningWorddata4);
    WriteData_AD9959(FDW_ADD, 4, ChannelPhaseOffsetTuningWorddata4, 0);
    WriteData_AD9959(LSRR_ADD, 2, SweepRampRatedata, 0);                            // 写单位步进时间
	//   f=400000;    // 设置输出频率大小
    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);       			// 写频率控制字
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWord0data, 1);
}

/**
 * @name	  AD9959_Amplitute_Sweep
 * @brief     幅度扫描模式
 * @param	  a：终止幅度，
			  a1：上升δ，
			  a2：下降δ，
			  f：输出频率
 * @return    空
 * @DateTime  2019-7-19
 */
void AD9959_Amplitute_Sweep(int a, int a1, int a2, double f)                    	// a终止幅度，a1上升δ，a2下降δ，f输出频率
{
    uint8_t ChannelAmplitudeTuningWorddata1[4];
    uint8_t ChannelAmplitudeTuningWorddata2[4];
    uint8_t ChannelAmplitudeTuningWorddata3[4];
    uint8_t ChannelFrequencyTuningWorddata4[4];

    uint8_t FunctionRegister1data[3] = {0xD0, 0x00, 0x00};                        	// 默认情况下选择
    uint8_t ChannelFunctionRegisterdata[3] = {0x40, 0x43, 0x20};                  	// 幅度扫描位启动
    uint8_t AmplitudeControldata[3] = {0x00, 0x0, 0x3f};                          	// 幅度扫描模式时启用
    uint8_t SweepRampRatedata[2] = {0xff, 0xff};                                  	// 默认单位扫描时间最长

    WriteData_AD9959(FR1_ADD, 3, FunctionRegister1data, 0);              		  	// 设置功能寄存器
    WriteData_AD9959(CFR_ADD, 3, ChannelFunctionRegisterdata, 0);
    //写起始幅度S0
    WriteData_AD9959(ACR_ADD, 3, AmplitudeControldata, 0);
    //写终止幅度E0
	//  a=1023; //写上升δ,不能写到1024，最大为1023
    WrAmplitudeTuningWorddata(a, ChannelAmplitudeTuningWorddata1);
    WriteData_AD9959(0x0a, 4, ChannelAmplitudeTuningWorddata1, 0);     				// CTW0 address 0x04.输出10MHZ频率
	//  a=10; // 写上升δ
    WrAmplitudeTuningWorddata(a1, ChannelAmplitudeTuningWorddata2);   				// 写频率控制字
    WriteData_AD9959(RDW_ADD, 4, ChannelAmplitudeTuningWorddata2, 0);
	//  a=10;  // 写下降δ
    WrAmplitudeTuningWorddata(a2, ChannelAmplitudeTuningWorddata3);   				// 写频率控制字
    WriteData_AD9959(FDW_ADD, 4, ChannelAmplitudeTuningWorddata3, 0);
    WriteData_AD9959(LSRR_ADD, 2, SweepRampRatedata, 0);                            // 写单位步进时间
	//  f=400000;
    WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWorddata4);      			// 写频率控制字
    WriteData_AD9959(CFTW0_ADD, 4, ChannelFrequencyTuningWorddata4, 1);
}

/**
 * @name	  AD9959_enablechannel0
 * @brief     使能通道0
 * @param	  空
 * @return    空
 * @DateTime  2019-7-19
 */
void AD9959_enablechannel0(void)                    	// 通道0使能
{
    uint8_t ChannelSelectRegisterdata0[1] =  {0x10};   	// 通道选择寄存器，3线传输，数据高位优先

    WriteData_AD9959(CSR_ADD, 1, ChannelSelectRegisterdata0, 0);
}

/**
 * @name	  AD9959_enablechannel1
 * @brief     使能通道1
 * @param	  空
 * @return    空
 * @DateTime  2019-7-19
 */
void AD9959_enablechannel1(void)                   		// 通道1使能
{
    uint8_t ChannelSelectRegisterdata1[1] = {0x20};   	// 通道选择寄存器，3线传输，数据高位优先
    // SDIO0 数据输入 and SDIO2 数据输出
    WriteData_AD9959(CSR_ADD, 1, ChannelSelectRegisterdata1, 0);
}

/**
 * @name	  AD9959_enablechannel2
 * @brief     使能通道2
 * @param	  空
 * @return    空
 * @DateTime  2019-7-19
 */
void AD9959_enablechannel2(void)                   		// 通道2使能
{
    uint8_t ChannelSelectRegisterdata1[1] = {0x40};   	// 通道选择寄存器，3线传输，数据高位优先
    // SDIO0 数据输入 and SDIO2 数据输出
    WriteData_AD9959(CSR_ADD, 1, ChannelSelectRegisterdata1, 0);
}

/**
 * @name	  AD9959_enablechannel3
 * @brief     使能通道3
 * @param	  空
 * @return    空
 * @DateTime  2019-7-19
 */
void AD9959_enablechannel3(void)                   		// 通道3使能
{
    uint8_t ChannelSelectRegisterdata1[1] = {0x80};   	// 通道选择寄存器，3线传输，数据高位优先
    // SDIO0 数据输入 and SDIO2 数据输出
    WriteData_AD9959(CSR_ADD, 1, ChannelSelectRegisterdata1, 0);
}

//////////////////////////////////////////////内部函数/////////////////////////////////////////////////////////

/**
 * @name	  WrPhaseOffsetTuningWorddata
 * @brief     设置频率转换字
 * @param	  f：输出频率大小
			  ChannelFrequencyTuningWorddata：待转换的十进制频率控制字指针
 * @return    空
 * @DateTime  2019-7-19
 */
void WrFrequencyTuningWorddata(double f, uint8_t *ChannelFrequencyTuningWorddata)
{
    unsigned char z;

    long int y;
    double x;
    x = 4294967296.0 / system_clk; 	// 频率控制字计算
    f = f * x;
    y = (long int)f;            	// 强制转换类型

    z = (uint8_t)(y >> 24); 		// 8位长整型数据强制转为8位字符型
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
 * @brief     设置相位转换字
 * @param	  f：输出频率大小
			  ChannelPhaseOffsetTuningWorddata：待转换的十进制相位控制字指针
 * @return    空
 * @DateTime  2019-7-19
 */
void WrPhaseOffsetTuningWorddata(double f, uint8_t *ChannelPhaseOffsetTuningWorddata)
{
    unsigned char z;

    long int y;
    double x;
    x = 16384.0 / 360;		 	// 相位控制字计算
    f = f * x;
    y = (long int)f;            // 强制转换类型

    z = (uint8_t)(y >> 8); 		// 8位长整型数据强制转为8位字符型
    ChannelPhaseOffsetTuningWorddata[0] = z;

    z = (uint8_t)(y >> 0);
    ChannelPhaseOffsetTuningWorddata[1] = z;
}

/**
 * @name	  WrAmplitudeTuningWorddata
 * @brief     幅度扫描时设置上升和下降的幅度
 * @param	  f：输出频率大小
			  ChannelAmplitudeTuningWorddata：待转换的十进制幅度控制字指针
 * @return    空
 * @DateTime  2019-7-19
 */
void WrAmplitudeTuningWorddata(double f, uint8_t *ChannelAmplitudeTuningWorddata)
{
    unsigned char z;

    unsigned long int y;
    double x;
    x = 1024.0 / 1024; 					// 幅度控制字计算
    f = f * x;
    y = (unsigned long int)f;           // 强制转换类型

    z = (uint8_t)(y >> 2); 				// 8位长整型数据强制转为8位字符型
    ChannelAmplitudeTuningWorddata[0] = z ;
    z = (uint8_t)(y << 6);    			// 取低2位
    ChannelAmplitudeTuningWorddata[1] = (z & 0xC0);
    ChannelAmplitudeTuningWorddata[2] = 0x00;
    ChannelAmplitudeTuningWorddata[3] = 0x00;
}

/**
 * @name	  WrAmplitudeTuningWorddata1
 * @brief     设置输出幅度的大小
 * @param	  f：输出频率大小
			  ChannelAmplitudeTuningWorddata：待转换的十进制幅度控制字指针
			  ASRAmplituteWordata：转换好的二进制幅度控制字指针
 * @return    空
 * @DateTime  2019-7-19
 */
void WrAmplitudeTuningWorddata1(double f, uint8_t *ChannelAmplitudeTuningWorddata, uint8_t *ASRAmplituteWordata)
{
    unsigned char z;

    unsigned long int y;
    double x;
    x = 1024.0 / 1024; 					// 幅度控制字计算
    f = f * x * 2.1;
    y = (unsigned long int)f;           // 强制转换类型

    ASRAmplituteWordata[0] = ChannelAmplitudeTuningWorddata[0];
	
    z = (uint8_t)(y >> 8); 				// 8位长整型数据强制转为8位字符型
    ChannelAmplitudeTuningWorddata[1] = (ChannelAmplitudeTuningWorddata[1] & 0xfc );
    ASRAmplituteWordata[1] = (ChannelAmplitudeTuningWorddata[1] | z );
    z = (uint8_t)(y << 0);
    ASRAmplituteWordata[2] = z;
}

#include <bsp_spi.h>

// ������SPIģ��ĳ�ʼ�����룬���ó�����ģʽ
// SPI�ڳ�ʼ��
// �������Ƕ�SPI1�ĳ�ʼ��
void SPI1_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;

    RCC_AHB1PeriphClockCmd(SPI_RCC_CLK, ENABLE);   // ʹ��GPIOAʱ��
    RCC_APB2PeriphClockCmd(SPI_AF_CLK, ENABLE);    // ʹ��SPI1ʱ��

    // GPIOF9,F10��ʼ������
    GPIO_InitStructure.GPIO_Pin = SCK_PIN | MISO_PIN | MOSI_PIN; // PB3~5���ù������
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                 // ���ù���
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;               // �������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;           // 100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                 // ����
    GPIO_Init(SPI_PORT, &GPIO_InitStructure);                    // ��ʼ��

    GPIO_PinAFConfig(SPI_PORT, SCK_SOURCE_PIN, GPIO_AF_SPI1);    // PB3����Ϊ SPI1
    GPIO_PinAFConfig(SPI_PORT, MISO_SOURCE_PIN, GPIO_AF_SPI1);   // PB4����Ϊ SPI1
    GPIO_PinAFConfig(SPI_PORT, MOSI_SOURCE_PIN, GPIO_AF_SPI1);   // PB5����Ϊ SPI1


    // ����ֻ���SPI�ڳ�ʼ��
    RCC_APB2PeriphResetCmd(SPI_AF_CLK, ENABLE);     // ��λSPI1
    RCC_APB2PeriphResetCmd(SPI_AF_CLK, DISABLE);    // ֹͣ��λSPI1

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  // ����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		                // ����SPI����ģʽ:����Ϊ��SPI
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		            // ����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		                    // ����ͬ��ʱ�ӵĿ���״̬Ϊ�͵�ƽ
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	                    // ����ͬ��ʱ�ӵĵ�һ�������أ��������½������ݱ�����
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		                    // NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;// ���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	                // ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
    SPI_InitStructure.SPI_CRCPolynomial = 7;	                        // CRCֵ����Ķ���ʽ
    SPI_Init(SPIX, &SPI_InitStructure);                                 // ����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���

    // ʹ��SPI����
    SPI_Cmd(SPIX, ENABLE);                         
    // ���ò�����Ԥ��Ƶϵ�� 84MHz / 16 = 5.25MHz��24L01֧�ֵ����SPIʱ��Ϊ10MHz
    SPI1_SetSpeed(SPI_BaudRatePrescaler_16);		
    // ��������
    SPI1_ReadWriteByte(0xff);
}

// SPI1�ٶ����ú���
// SpeedSet:0~7
// SPI�ٶ�=fAPB2/2^(SpeedSet+1)
// fAPB2ʱ��һ��Ϊ84Mhz
void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
    SPIX->CR1 &= 0XFFC7;
    SPIX->CR1 |= SPI_BaudRatePrescaler;	// ����SPI1�ٶ�
    SPI_Cmd(SPIX, ENABLE);
}


// SPI1 ��дһ���ֽ�
// TxData:Ҫд����ֽ�
// ����ֵ:��ȡ�����ֽ�
u8 SPI1_ReadWriteByte(u8 TxData)
{
    while (SPI_I2S_GetFlagStatus(SPIX, SPI_I2S_FLAG_TXE) == RESET) {}   // �ȴ���������

    SPI_I2S_SendData(SPIX, TxData);                                     // ͨ������SPIx����һ��byte ����

    while (SPI_I2S_GetFlagStatus(SPIX, SPI_I2S_FLAG_RXNE) == RESET) {}  // �ȴ�������һ��byte

    return SPI_I2S_ReceiveData(SPIX);                                   // ����ͨ��SPIx������յ�����
}

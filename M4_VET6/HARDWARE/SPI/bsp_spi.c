#include <bsp_spi.h>

// 以下是SPI模块的初始化代码，配置成主机模式
// SPI口初始化
// 这里针是对SPI1的初始化
void SPI1_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;

    RCC_AHB1PeriphClockCmd(SPI_RCC_CLK, ENABLE);   // 使能GPIOA时钟
    RCC_APB2PeriphClockCmd(SPI_AF_CLK, ENABLE);    // 使能SPI1时钟

    // GPIOF9,F10初始化设置
    GPIO_InitStructure.GPIO_Pin = SCK_PIN | MISO_PIN | MOSI_PIN; // PB3~5复用功能输出
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                 // 复用功能
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;               // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;           // 100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                 // 上拉
    GPIO_Init(SPI_PORT, &GPIO_InitStructure);                    // 初始化

    GPIO_PinAFConfig(SPI_PORT, SCK_SOURCE_PIN, GPIO_AF_SPI1);    // PB3复用为 SPI1
    GPIO_PinAFConfig(SPI_PORT, MISO_SOURCE_PIN, GPIO_AF_SPI1);   // PB4复用为 SPI1
    GPIO_PinAFConfig(SPI_PORT, MOSI_SOURCE_PIN, GPIO_AF_SPI1);   // PB5复用为 SPI1


    // 这里只针对SPI口初始化
    RCC_APB2PeriphResetCmd(SPI_AF_CLK, ENABLE);     // 复位SPI1
    RCC_APB2PeriphResetCmd(SPI_AF_CLK, DISABLE);    // 停止复位SPI1

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  // 设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		                // 设置SPI工作模式:设置为主SPI
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		            // 设置SPI的数据大小:SPI发送接收8位帧结构
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		                    // 串行同步时钟的空闲状态为低电平
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	                    // 串行同步时钟的第一个跳变沿（上升或下降）数据被采样
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		                    // NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;// 定义波特率预分频的值:波特率预分频值为256
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	                // 指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
    SPI_InitStructure.SPI_CRCPolynomial = 7;	                        // CRC值计算的多项式
    SPI_Init(SPIX, &SPI_InitStructure);                                 // 根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器

    // 使能SPI外设
    SPI_Cmd(SPIX, ENABLE);                         
    // 设置波特率预分频系数 84MHz / 16 = 5.25MHz，24L01支持的最大SPI时钟为10MHz
    SPI1_SetSpeed(SPI_BaudRatePrescaler_16);		
    // 启动传输
    SPI1_ReadWriteByte(0xff);
}

// SPI1速度设置函数
// SpeedSet:0~7
// SPI速度=fAPB2/2^(SpeedSet+1)
// fAPB2时钟一般为84Mhz
void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
    SPIX->CR1 &= 0XFFC7;
    SPIX->CR1 |= SPI_BaudRatePrescaler;	// 设置SPI1速度
    SPI_Cmd(SPIX, ENABLE);
}


// SPI1 读写一个字节
// TxData:要写入的字节
// 返回值:读取到的字节
u8 SPI1_ReadWriteByte(u8 TxData)
{
    while (SPI_I2S_GetFlagStatus(SPIX, SPI_I2S_FLAG_TXE) == RESET) {}   // 等待发送区空

    SPI_I2S_SendData(SPIX, TxData);                                     // 通过外设SPIx发送一个byte 数据

    while (SPI_I2S_GetFlagStatus(SPIX, SPI_I2S_FLAG_RXNE) == RESET) {}  // 等待接收完一个byte

    return SPI_I2S_ReceiveData(SPIX);                                   // 返回通过SPIx最近接收的数据
}

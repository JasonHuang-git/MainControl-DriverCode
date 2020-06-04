#ifndef __BSP_AD9959_H 
#define __BSP_AD9959_H 

#include <bsp.h>
// 简述：
//（1）AD9959有3个模式：单调谐模式，调制模式，线性扫描模式。
//（2）利用定时器3产生4个可调频率的方波用于单调谐模式和线性扫描模式。可以根据需要随意设置频率。
//（3）利用定时器2产生4路同步频率可调方波用于调制模式。频率可以根据公式 f = 12000000/CCR4_Val，	CCR4_Val见_TIM_TimeBase_H里面要设置的数，但是四个通道的频率一定是连续２倍关系

/*
AD9959与STM32连接，GPIO映射关系：
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

// 内部寄存器地址定义
#define CSR_ADD   	 0x00   //CSR  Channel Select Register(通道选择寄存器)                1 Bytes
#define FR1_ADD   	 0x01   //FR1  Function Register 1(功能寄存器1)                       3 Bytes
#define FR2_ADD   	 0x02   //FR2  Function Register 2(功能寄存器2)                       2 Bytes
#define CFR_ADD   	 0x03   //CFR  Channel Function Register(通道功能寄存器)              3 Bytes
#define CFTW0_ADD    0x04   //CTW0 Channel Frequency Tuning Word 0(通道频率转换字寄存器)  4 Bytes
#define CPOW0_ADD    0x05   //CPW0 Channel Phase Offset Word 0(通道相位转换字寄存器)      2 Bytes
#define ACR_ADD      0x06   //ACR  Amplitude Control Register(幅度控制寄存器)             3 Bytes
#define LSRR_ADD     0x07   //LSR  Linear Sweep Ramp Rate(通道线性扫描寄存器)             2 Bytes
#define RDW_ADD      0x08   //RDW  LSR Rising Delta Word(通道线性向上扫描寄存器)          4 Bytes
#define FDW_ADD      0x09   //FDW  LSR Falling Delta Word(通道线性向下扫描寄存器)         4 Bytes

// DDS系统时钟频率定义
#define system_clk 500000000L

// GPIO相关宏定义
#define DDS_CLK		 (RCC_AHB1Periph_GPIOF|RCC_AHB1Periph_GPIOG)
#define DDS_PORT1	 GPIOF
#define DDS_PORT2	 GPIOG
// GPIOG口
#define DDS_SCLK_PIN GPIO_Pin_11
#define DDS_CS_PIN	 GPIO_Pin_10
#define DDS_RESET    GPIO_Pin_9
#define DDS_PWR		 GPIO_Pin_8
// GPIOF口
#define DDS_PS0		 GPIO_Pin_0
#define DDS_PS1		 GPIO_Pin_1
#define DDS_PS2		 GPIO_Pin_2
#define DDS_PS3		 GPIO_Pin_3
#define DDS_SDIO0	 GPIO_Pin_4
#define DDS_SDIO1	 GPIO_Pin_5
#define DDS_SDIO2	 GPIO_Pin_6
#define DDS_SDIO3	 GPIO_Pin_7
#define DDS_UPDATE	 GPIO_Pin_8

// AD9959 引脚位带操作宏定义
#define SCLK         PGout(11)
#define CS           PGout(10)
#define RESET        PGout(9)
#define AD9959_PWR   PGout(8)

#define PS0          PFout(0)
#define PS1          PFout(1)
#define PS2          PFout(2)
#define PS3          PFout(3)

#define SDIO0        PFout(4)
#define SDIO1        PFout(5)
#define SDIO2        PFout(6)
#define SDIO3        PFout(7)
#define UPDATE       PFout(8)

// 函数定义区域
void Init_AD9959(void);
void delay_9959(uint32_t length);
void InitIO_9959(void);
void InitReset(void);
void IO_Update(void) ;
void WriteData_AD9959(uint8_t RegisterAddress, uint8_t NumberofRegisters, uint8_t *RegisterData,uint8_t temp);
void ReadData_9959(uint8_t RegisterAddress,uint8_t NumberofRegisters,uint8_t *RegisterData);

// 通道控制和幅频相三者调节整合函数
void Write_frequence(uint8_t Channel,uint32_t Freq);
void Write_Amplitude(uint8_t Channel, uint16_t Ampli);
void Write_Phase(uint8_t Channel,uint16_t Phase);

// 通道控制函数
void AD9959_enablechannel0(void);
void AD9959_enablechannel1(void);
void AD9959_enablechannel2(void);
void AD9959_enablechannel3(void);

// 内部函数
extern void WrPhaseOffsetTuningWorddata(double f,uint8_t *ChannelPhaseOffsetTuningWorddata);
extern void WrAmplitudeTuningWorddata(double f,uint8_t *ChannelAmplitudeTuningWorddata);	
extern void WrAmplitudeTuningWorddata1(double f,uint8_t *ChannelAmplitudeTuningWorddata,uint8_t *ASRAmplituteWordata);
extern void WrFrequencyTuningWorddata(double f, uint8_t *ChannelFrequencyTuningWorddata);

// 输出函数
void AD9959_Setwavefrequency (double f);
void AD9959_Setwavephase(double f, int p);
void AD9959_Setwaveamplitute(double f, int a);

// 2阶调制
void AD9959_SetFremodulation2(double f1, double f2);
void AD9959_SetPhamodulation2(double f, int p1, int p2);
void AD9959_SetAM2(double f);

// 4阶调制
void AD9959_SetFremodulation4(double f1,double f2,double f3,double f4);
void AD9959_SetPhamodulation4(double f,int p1,int p2,int p3,int p4);
void AD9959_SetAM4(double f);
void AD9959_SetFremodulation42(double f1,double f2,double f3,double f4);
void AD9959_SetPhamodulation42(double f,int p1,int p2,int p3,int p4);
void AD9959_SetAM42(double f);

// 8阶调制
void AD9959_SetFremodulation80(double f1,double f2,double f3,double f4,double f5,double f6,double f7,double f8);
void AD9959_SetPhamodulation80(double f,int p1,int p2,int p3,int p4,int p5,int p6,int p7,int p8);
void AD9959_SetAM80(double f);
void AD9959_SetFremodulation81(double f1,double f2,double f3,double f4,double f5,double f6,double f7,double f8);
void AD9959_SetPhamodulation81(double f,int p1,int p2,int p3,int p4,int p5,int p6,int p7,int p8);
void AD9959_SetAM81(double f);
void AD9959_SetFremodulation82(double f1,double f2,double f3,double f4,double f5,double f6,double f7,double f8);
void AD9959_SetPhamodulation82(double f,int p1,int p2,int p3,int p4,int p5,int p6,int p7,int p8);
void AD9959_SetAM82(double f);
void AD9959_SetFremodulation83(double f1,double f2,double f3,double f4,double f5,double f6,double f7,double f8);
void AD9959_SetPhamodulation83(double f,int p1,int p2,int p3,int p4,int p5,int p6,int p7,int p8);
void AD9959_SetAM83(double f);

// 16阶调制
void AD9959_SetFremodulation160(double f1,double f2,double f3,double f4,double f5,double f6,double f7,double f8,double f9,double f10,double f11,double f12,double f13,double f14,double f15,double f16);
void AD9959_SetPhamodulation160(double f,int p1,int p2,int p3,int p4,int p5,int p6,int p7,int p8,int p9,int p10,int p11,int p12,int p13,int p14,int p15,int p16);
void AD9959_SetAM160(double f);
void AD9959_SetFremodulation161(double f1,double f2,double f3,double f4,double f5,double f6,double f7,double f8,double f9,double f10,double f11,double f12,double f13,double f14,double f15,double f16);
void AD9959_SetPhamodulation161(double f,int p1,int p2,int p3,int p4,int p5,int p6,int p7,int p8,int p9,int p10,int p11,int p12,int p13,int p14,int p15,int p16);
void AD9959_SetAM161(double f);
void AD9959_SetFremodulation162(double f1,double f2,double f3,double f4,double f5,double f6,double f7,double f8,double f9,double f10,double f11,double f12,double f13,double f14,double f15,double f16);
void AD9959_SetPhamodulation162(double f,int p1,int p2,int p3,int p4,int p5,int p6,int p7,int p8,int p9,int p10,int p11,int p12,int p13,int p14,int p15,int p16);
void AD9959_SetAM162(double f);
void AD9959_SetFremodulation163(double f1,double f2,double f3,double f4,double f5,double f6,double f7,double f8,double f9,double f10,double f11,double f12,double f13,double f14,double f15,double f16);
void AD9959_SetPhamodulation163(double f,int p1,int p2,int p3,int p4,int p5,int p6,int p7,int p8,int p9,int p10,int p11,int p12,int p13,int p14,int p15,int p16);
void AD9959_SetAM163(double f);

// 扫描模式
void AD9959_Frequency_Sweep(double f1,double f2,double a1,double a2);
void AD9959_Phase_Sweep(int p1,int p2,int a1,int a2,double f);	
void AD9959_Amplitute_Sweep(int a,int a1,int a2,double f);

#endif  /* __BSP_AD9959_H */

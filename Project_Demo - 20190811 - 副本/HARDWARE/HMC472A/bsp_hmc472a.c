#include <bsp_hmc472a.h>

/**
 * @name	  hmc472_init
 * @brief     ��ʼ��HMC472��I/O�Ĺ�����ʽ
 * @param	  ��
 * @return    ��
 * @DateTime  2019-7-17
 */
void hmc472_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(A_HMC472A_CLK, ENABLE);
	
	// A���������ó�ʼ��
    GPIO_InitStructure.GPIO_Pin   = V5_Pin | V4_Pin | V3_Pin | V2_Pin | V1_Pin | V0_Pin;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;            // ��ͨ���ģʽ
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;            // �������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;        // 100MHz
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;             // ����
    GPIO_Init(A_HMC472A_PORT, &GPIO_InitStructure);           // ��ʼ��
	GPIO_WriteBit(A_HMC472A_PORT, V5_Pin | V4_Pin | V3_Pin
                        | V2_Pin | V1_Pin | V0_Pin, Bit_RESET);
	
#ifdef ATTENUATION	
	RCC_AHB1PeriphClockCmd(B_HMC472A_CLK, ENABLE);
	// B���������ó�ʼ��
	GPIO_InitStructure.GPIO_Pin   = V6_Pin | V7_Pin | V8_Pin | V9_Pin | V10_Pin | V11_Pin;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;            // ��ͨ���ģʽ
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;            // �������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;        // 100MHz
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;             // ����
    GPIO_Init(B_HMC472A_PORT, &GPIO_InitStructure);           // ��ʼ��
	GPIO_WriteBit(B_HMC472A_PORT, V6_Pin | V7_Pin | V8_Pin
                        | V9_Pin | V10_Pin | V11_Pin, Bit_RESET);
#endif
}

/**
 * @name	  hmc472_ctr
 * @brief     �����������HMC472˥������
			  Insertion Loss��
			  DC - 1.5 GHz     1.4 ~ 1.8 dB
		      1.5 - 3.0 GHz    1.7 ~ 2.3 dB
		      3.0 - 3.8 GHz    1.9 ~ 3   dB
 * @param	  ��dBΪ��λ����˥��������0.5dB��������Χ0~31.5dB/63dB�����Ʋ������)
 * @return    �������Ʒ���0�������������̷�Χ����1��
 * @DateTime  2019-7-17
 */
uint8_t hmc472_ctr(float _db)
{
    __IO uint8_t db = ( uint8_t ) ( _db * 2 );
	
#ifdef ATTENUATION
	__IO uint8_t db2 = 0;
#endif
	// �ж��Ƿ񳬳�����
    if((_db < MIN_MULTIPLE) || (db > MAX_MULTIPLE))
    {
        printf("---------------Out of range!---------------\r\n");
        return 1;
    }
#ifdef ATTENUATION
	if(db > A_MAX_MULTIPLE)
	{
		db2 = db - A_MAX_MULTIPLE;
		db = A_MAX_MULTIPLE;
	}
	else
	{
		db2 = 0;
	}
#endif
	// 0.5dB����λ
	if(1 == (db & 0x01))
		V5_GPIO_PIN = GPIO_PIN_RESET;
	else
		V5_GPIO_PIN = GPIO_PIN_SET;
	// 1dB����λ
	db = db >> 1;
	if(1 == (db & 0x01))
		 V4_GPIO_PIN = GPIO_PIN_RESET;
	else
		 V4_GPIO_PIN = GPIO_PIN_SET;
	// 2dB����λ
	db = db >> 1;
	if(1 == (db & 0x01))
		V3_GPIO_PIN = GPIO_PIN_RESET;
	else
		V3_GPIO_PIN = GPIO_PIN_SET;
	// 4dB����λ
	db = db >> 1;
	if(1 == ( db & 0x01))
		V2_GPIO_PIN = GPIO_PIN_RESET;
	else
		V2_GPIO_PIN = GPIO_PIN_SET;
	// 8dB����λ
	db = db >> 1;
	if(1 == ( db & 0x01))
		V1_GPIO_PIN = GPIO_PIN_RESET;
	else
		V1_GPIO_PIN = GPIO_PIN_SET;
	// 16dB����λ
	db = db >> 1;
	if(1 == (db & 0x01)) 
		V0_GPIO_PIN = GPIO_PIN_RESET;
	else 
		V0_GPIO_PIN = GPIO_PIN_SET;
#ifdef ATTENUATION
	// ����32~63dB
	// 0.5db����λ
	if(1 == (db2 & 0x01))
		V11_GPIO_PIN = GPIO_PIN_RESET;
	else
		V11_GPIO_PIN = GPIO_PIN_SET;
	// 1dB����λ
	db2 = db2 >> 1;
	if(1 == (db2 & 0x01))
		 V10_GPIO_PIN = GPIO_PIN_RESET;
	else
		 V10_GPIO_PIN = GPIO_PIN_SET;
	// 2dB����λ
	db2 = db2 >> 1;
	if(1 == (db2 & 0x01))
		V9_GPIO_PIN = GPIO_PIN_RESET;
	else
		V9_GPIO_PIN = GPIO_PIN_SET;
	// 4dB����λ
	db2 = db2 >> 1;
	if(1 == (db2 & 0x01))
		V8_GPIO_PIN = GPIO_PIN_RESET;
	else
		V8_GPIO_PIN = GPIO_PIN_SET;
	// 8dB����λ
	db2 = db2 >> 1;
	if(1 == (db2 & 0x01))
		V7_GPIO_PIN = GPIO_PIN_RESET;
	else
		V7_GPIO_PIN = GPIO_PIN_SET;
	// 16dB����λ
	db2 = db2 >> 1;
	if(1 == (db2 & 0x01)) 
		V6_GPIO_PIN = GPIO_PIN_RESET;
	else 
		V6_GPIO_PIN = GPIO_PIN_SET;
#endif

    return 0;
}


#include <bsp.h>

/* ����˺ͽ��ն˵�ַ */
uint8_t TX_ADDRESS[TX_ADR_WIDTH] = {0x34, 0x43, 0x10, 0x10, 0x01};
uint8_t RX_ADDRESS[RX_ADR_WIDTH] = {0x34, 0x43, 0x10, 0x10, 0x01};

/*
*********************************************************************************************************
*	�� �� ��: nRF24L01_InitHard
*	����˵��: SPI����
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void nRF24L01_InitHard(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/*ʹ��GPIOʱ�� */
	RCC_AHB1PeriphClockCmd(nRF24L01_RCC_CLK, ENABLE);

	/* ����nRF24L01��CE���� */
	/* ����Ϊ�������ģʽ */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = nRF24L01_PIN_CE;
	GPIO_Init(nRF24L01_PORT_CE, &GPIO_InitStructure);

	/* ����nRF24L01�� CSN ���� */
	/* ����Ϊ�������ģʽ */
	GPIO_InitStructure.GPIO_Pin = nRF24L01_PIN_CSN;
	GPIO_Init(nRF24L01_PORT_CSN, &GPIO_InitStructure);

	/* ����nRF24L01�� IRQ ���� */
	/* ����Ϊ��������� */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = nRF24L01_PIN_IRQ;
	GPIO_Init(nRF24L01_PORT_IRQ, &GPIO_InitStructure);

	/* ��������csn���ţ�NRF�������״̬ */
	nRF24L01_CSN_1();
}

/*
*********************************************************************************************************
*	�� �� ��: nRF24L01_WriteReg
*	����˵��: ������nRF24L01�ض��ļĴ���д������
*	��    ��: _ucRegAddr: nRF24L01������+�Ĵ�����ַ��
_usValue:   ��Ҫ��Ĵ���д�������
*	�� �� ֵ: nRF24L01��status�Ĵ�����״̬
*********************************************************************************************************
*/
static uint8_t nRF24L01_WriteReg(uint8_t _ucRegAddr, uint8_t _usValue)
{
	uint8_t ucStatus;

	nRF24L01_CE_0();

	/* �õ�CSN��ʹ��SPI���� */
	nRF24L01_CSN_0();

	/* ��������Ĵ����� */
	ucStatus = SPI1_ReadWriteByte(_ucRegAddr);

	/* ��Ĵ���д������ */
	SPI1_ReadWriteByte(_usValue);

	/* CSN���ߣ���� */
	nRF24L01_CSN_1();

	/* ����״̬�Ĵ�����ֵ */
	return (ucStatus);
}

/*
*********************************************************************************************************
*	�� �� ��: nRF24L01_ReadReg
*	����˵��: ���ڴ�nRF24L01�ض��ļĴ�����������
*	��    ��: reg: nRF24L01������+�Ĵ�����ַ
*	�� �� ֵ: �Ĵ����е�����
*********************************************************************************************************
*/
static uint8_t nRF24L01_ReadReg(uint8_t _ucRegAddr)
{
	uint8_t usValue;

	nRF24L01_CE_0();

	/* �õ�CSN��ʹ��SPI���� */
	nRF24L01_CSN_0();

	/* ���ͼĴ����� */
	SPI1_ReadWriteByte(_ucRegAddr);

	/* ��ȡ�Ĵ�����ֵ */
	usValue = SPI1_ReadWriteByte(NOP);

	/*CSN���ߣ����*/
	nRF24L01_CSN_1();

	return usValue;
}

/*
*********************************************************************************************************
*	�� �� ��: nRF24L01_ReadBuf
*	����˵��: ���ڴ�nRF24L01�ļĴ����ж���һ������
*	��    ��: _ucRegAddr: nRF24L01������+�Ĵ�����ַ��
_pBuf��     ���ݻ����ַ��
_ucLen:     _pBuf�ݳ���
*	�� �� ֵ: nRF24L01��status�Ĵ�����״̬
*********************************************************************************************************
*/
static uint8_t nRF24L01_ReadBuf(uint8_t _ucRegAddr, uint8_t *_pBuf, uint8_t _ucLen)
{
	uint8_t ucStatus, i;

	nRF24L01_CE_0();

	/* �õ�CSN��ʹ��SPI���� */
	nRF24L01_CSN_0();

	/*���ͼĴ�����*/
	ucStatus = SPI1_ReadWriteByte(_ucRegAddr);

	/*��ȡ����������*/
	for (i = 0; i < _ucLen; i++)
	{
		_pBuf[i] = SPI1_ReadWriteByte(NOP);
	}

	/* CSN���ߣ���� */
	nRF24L01_CSN_1();

	return ucStatus;
}

/*
*********************************************************************************************************
*	�� �� ��: nRF24L01_WriteBuf
*	����˵��: ������nRF24L01�ļĴ�����д��һ������
*	��    �Σ�_ucRegAddr: nRF24L01������+�Ĵ�����ַ��
_pBuf��     ���ݻ����ַ��
_ucLen:     _pBuf�����ݳ��ȡ�
*	�� �� ֵ: NRF��status�Ĵ�����״̬��
*********************************************************************************************************
*/
static uint8_t nRF24L01_WriteBuf(uint8_t _ucRegAddr, uint8_t *_pBuf, uint8_t _ucLen)
{
	uint8_t ucStatus, i;

	nRF24L01_CE_0();

	/*�õ�CSN��ʹ��SPI����*/
	nRF24L01_CSN_0();

	/*���ͼĴ�����*/
	ucStatus = SPI1_ReadWriteByte(_ucRegAddr);

	/*�򻺳���д������*/
	for (i = 0; i < _ucLen; i++)
	{
		SPI1_ReadWriteByte(*_pBuf++);
	}

	/*CSN���ߣ����*/
	nRF24L01_CSN_1();

	return (ucStatus);
}

/*
*********************************************************************************************************
*	�� �� ��: nRF24L01_RxMode
*	����˵��: ����nRF24L01�����ڽ���ģʽ
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void nRF24L01_RxMode(void)
{
	nRF24L01_CE_0();
	nRF24L01_WriteBuf(NRF_WRITE_REG + RX_ADDR_P0, RX_ADDRESS, RX_ADR_WIDTH); /* дRX�ڵ��ַ */
	nRF24L01_WriteReg(NRF_WRITE_REG + EN_AA, 0x01);               			 /* ʹ��ͨ��0���Զ�Ӧ�� */
	nRF24L01_WriteReg(NRF_WRITE_REG + EN_RXADDR, 0x01);           			 /* ʹ��ͨ��0�Ľ��յ�ַ */
	nRF24L01_WriteReg(NRF_WRITE_REG + RF_CH, CHANAL);             			 /* ����RFͨ��Ƶ�� */
	nRF24L01_WriteReg(NRF_WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH);  			 /* ѡ��ͨ��0����Ч���ݿ�� */
	nRF24L01_WriteReg(NRF_WRITE_REG + RF_SETUP, 0x0f);                       /* ����TX�������,0db����,2Mbps,���������濪�� */
	nRF24L01_WriteReg(NRF_WRITE_REG + CONFIG, 0x0f);                         /* ���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ */

	/*CE���ߣ��������ģʽ*/
	nRF24L01_CE_1();
}

/*
*********************************************************************************************************
*	�� �� ��: nRF24L01_TxMode
*	����˵��: ����nRF24L01�����ڷ���ģʽ
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void nRF24L01_TxMode(void)
{
	nRF24L01_CE_0();
	nRF24L01_WriteBuf(NRF_WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);    /* дTX�ڵ��ַ */
	nRF24L01_WriteBuf(NRF_WRITE_REG + RX_ADDR_P0, RX_ADDRESS, RX_ADR_WIDTH); /* ����TX�ڵ��ַ,��ҪΪ��ʹ��ACK */
	nRF24L01_WriteReg(NRF_WRITE_REG + EN_AA, 0x01);                          /* ʹ��ͨ��0���Զ�Ӧ�� */
	nRF24L01_WriteReg(NRF_WRITE_REG + EN_RXADDR, 0x01);                      /* ʹ��ͨ��0�Ľ��յ�ַ */
	nRF24L01_WriteReg(NRF_WRITE_REG + SETUP_RETR, 0x0f);                     /* �����Զ��ط����ʱ��:250us + 86us;����Զ��ط�����:10�� */
	nRF24L01_WriteReg(NRF_WRITE_REG + RF_CH, CHANAL);                        /* ����RFͨ��ΪCHANAL */
	nRF24L01_WriteReg(NRF_WRITE_REG + RF_SETUP, 0x0f);                       /* ����TX�������,0db����,2Mbps,���������濪�� */
	nRF24L01_WriteReg(NRF_WRITE_REG + CONFIG, 0x0e);                         /* ���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ,���������ж� */

	/* CE���ߣ����뷢��ģʽ */
	nRF24L01_CE_1();
	delay_us(100);  /* CEҪ����һ��ʱ��Ž��뷢��ģʽ��ʱ�����10us */
}

/*
*********************************************************************************************************
*	�� �� ��: nRF24L01_ConnectCheck
*	����˵��: ��Ҫ����nRF24L01_��MCU�Ƿ���������
*	��    ��: ��
*	�� �� ֵ: 1 ��ʾ����������0 ��ʾ���Ӳ�����
*********************************************************************************************************
*/
uint8_t nRF24L01_ConnectCheck(void)
{
	uint8_t ucBuf[TX_ADR_WIDTH];

	/*д��TX_ADR_WIDTH���ֽڵĵ�ַ. */
	nRF24L01_WriteBuf(NRF_WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);

	/*����д��ĵ�ַ */
	nRF24L01_ReadBuf(TX_ADDR, ucBuf, TX_ADR_WIDTH);

	/* ���ڵ���Ŀ�� */
	printf("ucBuf[0] = %d, TX_ADDRESS[0] = %d\r\n", ucBuf[0], TX_ADDRESS[0]);
	printf("ucBuf[1] = %d, TX_ADDRESS[1] = %d\r\n", ucBuf[1], TX_ADDRESS[1]);
	printf("ucBuf[2] = %d, TX_ADDRESS[2] = %d\r\n", ucBuf[2], TX_ADDRESS[2]);
	printf("ucBuf[3] = %d, TX_ADDRESS[3] = %d\r\n", ucBuf[3], TX_ADDRESS[3]);
	printf("ucBuf[4] = %d, TX_ADDRESS[4] = %d\r\n", ucBuf[4], TX_ADDRESS[4]);

	/* �Ƚ�д������ݺͶ������Ƿ���ͬ */
	if (strncmp((char *)TX_ADDRESS, (char *)ucBuf, TX_ADR_WIDTH) == 0)
	{
		return 1;  /* ��ͬ */
	}
	else
	{
		return 0;  /* ����ͬ */
	}
}

/*
*********************************************************************************************************
*	�� �� ��: nRF24L01_TxData
*	����˵��: ������nRF24L01�ķ��ͻ�������д������
*	��    ��: _pTxBuf ���ݻ����ַ
*	�� �� ֵ: ���ͽ�����ɹ�����TX_DS, ʧ�ܷ���MAX_RT��0
*********************************************************************************************************
*/
uint8_t nRF24L01_TxData(uint8_t *_pTxBuf)
{
	uint8_t ucState;
	uint32_t ulCount = 0;

	/*CEΪ�ͣ��������ģʽ1*/
	nRF24L01_CE_0();

	/*д���ݵ�_pTxBuf���32���ֽ�*/
	nRF24L01_WriteBuf(WR_TX_PLOAD, _pTxBuf, TX_PLOAD_WIDTH);

	/*CEΪ�ߣ�_pTxBuf�ǿգ��������ݰ� */
	nRF24L01_CE_1();

	/*
	�ȴ���������ж�
	while(nRF24L01_IRQ_Read()!=0);
	ѡ���������ַ�ʽ������Ч��ֹ������������ʱ���������״̬
	*/
	while ((nRF24L01_IRQ_Read() != 0) && (ulCount < 65535))
	{
		ulCount++;
	}

	if (ulCount >= 65535)
	{
		return 0;
	}

	/* ��ȡ״̬�Ĵ�����ֵ */
	ucState = nRF24L01_ReadReg(STATUS);

	/*���TX_DS��MAX_RT�жϱ�־*/
	nRF24L01_WriteReg(NRF_WRITE_REG + STATUS, ucState);

	nRF24L01_WriteReg(FLUSH_TX, NOP);     /* ���TX FIFO�Ĵ��� */

	/*�ж��ж�����*/
	/* �ﵽ����ط����� */
	if (ucState & MAX_RT)
	{
		return MAX_RT;
	}
	/* ������� */
	else if (ucState & TX_DS)
	{
		return TX_DS;
	}
	/* ����ԭ����ʧ�� */
	else
	{
		return 0;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: nRF24L01_RxData
*	����˵��: ���ڴ�nRF24L01�Ľ��ջ������ж�������
*	��    ��: _pRxBuf ���ݻ����ַ
*	�� �� ֵ: ���ս�����ɹ�����RX_DS, ʧ�ܷ���0
*********************************************************************************************************
*/
uint8_t nRF24L01_RxData(uint8_t *_pRxBuf)
{
	uint8_t ucState;
	uint32_t ulCount = 0;

	nRF24L01_CE_1();	  /* �������״̬ */

	/*
	�ȴ������ж�
	while(nRF24L01_IRQ_Read()!=0);
	ѡ���������ַ�ʽ������Ч��ֹ������������ʱ���������״̬
	*/
	while ((nRF24L01_IRQ_Read() != 0) && (ulCount < 65535))
	{
		ulCount++;
	}

	if (ulCount >= 65535)
	{
		return 0;
	}

	nRF24L01_CE_0();  	 /* �������״̬ */

	/*��ȡstatus�Ĵ�����ֵ  */
	ucState = nRF24L01_ReadReg(STATUS);

	/* ����жϱ�־*/
	nRF24L01_WriteReg(NRF_WRITE_REG + STATUS, ucState);

	/*�ж��Ƿ���յ�����*/
	if (ucState & RX_DR)   /* ���յ����� */
	{
		nRF24L01_ReadBuf(RD_RX_PLOAD, _pRxBuf, RX_PLOAD_WIDTH); /* ��ȡ���� */
		nRF24L01_WriteReg(FLUSH_RX, NOP);                       /* ���RX FIFO�Ĵ��� */
		return RX_DR;
	}
	else
	{
		return 0;   /* û�յ��κ����� */
	}
}

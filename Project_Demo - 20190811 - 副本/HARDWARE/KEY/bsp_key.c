#include <bsp_key.h>

/**
 * @name	  init_key
 * @brief     独立按键初始化
 * @param	  空
 * @return    空
 * @DateTime  2019-7-18
 */
void init_key(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	// 使能对应GPIO外设时钟
	RCC_AHB1PeriphClockCmd(S2_CLK | S3_CLK, ENABLE);

	// 设置对应S2引脚为输入模式
	GPIO_InitStructure.GPIO_Pin   = S2_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
	GPIO_Init(S2_PORT, &GPIO_InitStructure);

	// 设置对应S3引脚为输入模式
	GPIO_InitStructure.GPIO_Pin   = S3_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
	GPIO_Init(S3_PORT, &GPIO_InitStructure);

}
extern uint8_t key_old;
/**
 * @name	  key_scan
 * @brief     独立按键扫描程序
 * @param	  mode
			  0：表示不支持连按
			  1：表示支持连按
 * @return    按键键值
 * @DateTime  2019-7-18
 */
uint8_t key_scan(uint8_t mode)
{
	static uint8_t flag = 1;

	if(mode) flag = 1;

	if(flag && (S2 || S3))
	{
		delay_ms(10);	// 去抖动
		if(flag && (S2 || S3))
		{
			flag = 0;
			if(S2)
            {
                key_old = S2_Val;
                return S2_Val;
            }
			else if(S3)
            {
                key_old = S3_Val;
                return S3_Val;
            }
		}
	}
	else if(!S2 && !S3) flag = 1;
	return key_old;
}


#ifdef KEYBOARD

//The KeyBoard Connect(4X4)
//ROW(PG0 PG1 PG2 PG3)    COL(PG4 PG4 PG5 PG6)
/**
 * @name	  init_keyboard
 * @brief     4X4矩阵键盘引脚初始化函数
 * @param	  空
 * @return    空
 * @DateTime  2019-7-18
 */
void init_keyboard(void)
{
	GPIO_InitTypeDef GPIO_Structure;

	RCC_AHB1PeriphClockCmd(KEY_BOARD_CLK, ENABLE);

	GPIO_Structure.GPIO_Pin = ROW_PIN_0 | ROW_PIN_1 | ROW_PIN_2 | ROW_PIN_3 
							| COL_PIN_0 | COL_PIN_1 | COL_PIN_2 | COL_PIN_3;
	GPIO_Structure.GPIO_OType = GPIO_OType_OD;
	GPIO_Structure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Structure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(KEY_BOARD_PORT, &GPIO_Structure);

	// 确保所有按键保持拉高状态
	GPIO_SetBits(KEY_BOARD_PORT, ROW_PIN_0 | ROW_PIN_1 | ROW_PIN_2 | ROW_PIN_3 
							| COL_PIN_0 | COL_PIN_1 | COL_PIN_2 | COL_PIN_3);
}

/**
 * @name	  ResetRow
 * @brief     拉低行线
 * @param	  空
 * @return    空
 * @DateTime  2019-7-18
 */
void ResetRow(void)
{
	GPIO_InitTypeDef GPIO_Structure;

	// 配置行为输出模式
	GPIO_Structure.GPIO_Pin = ROW_PIN_0 | ROW_PIN_1 | ROW_PIN_2 | ROW_PIN_3;
	GPIO_Structure.GPIO_OType = GPIO_OType_OD;
	GPIO_Structure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Structure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(KEY_BOARD_PORT, &GPIO_Structure);


	// 配置列为输入模式
	GPIO_Structure.GPIO_Pin = COL_PIN_0 | COL_PIN_1 | COL_PIN_2 | COL_PIN_3;
	GPIO_Structure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Structure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(KEY_BOARD_PORT, &GPIO_Structure);

	// 配置行输出低电平
	GPIO_ResetBits(KEY_BOARD_PORT, ROW_PIN_0 | ROW_PIN_1 | ROW_PIN_2 | ROW_PIN_3);
}

/**
 * @name	  ResetCol
 * @brief     拉低列线
 * @param	  空
 * @return    空
 * @DateTime  2019-7-18
 */
void ResetCol()
{
	GPIO_InitTypeDef GPIO_Structure;

	// 配置行为输入模式
	GPIO_Structure.GPIO_Pin = ROW_PIN_0 | ROW_PIN_1 | ROW_PIN_2 | ROW_PIN_3;
	GPIO_Structure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Structure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(KEY_BOARD_PORT, &GPIO_Structure);

	// 配置列为输出模式
	GPIO_Structure.GPIO_Pin = COL_PIN_0 | COL_PIN_1 | COL_PIN_2 | COL_PIN_3;
	GPIO_Structure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Structure.GPIO_OType = GPIO_OType_OD;
	GPIO_Structure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(KEY_BOARD_PORT, &GPIO_Structure);

	// 配置列输出低电平
	GPIO_ResetBits(KEY_BOARD_PORT, COL_PIN_0 | COL_PIN_1 | COL_PIN_2 | COL_PIN_3);
}

/**
 * @name	  keyboard_scan
 * @brief     4x4矩阵键盘扫描
 * @param	  mode：0（不支持连按）
			        1（支持连按）
 * @return    成功检测到按键返回对应键值，如果没检测到按键则返回0
 * @DateTime  2019-7-18
 */
uint8_t keyboard_scan(uint8_t mode)
{
	static uint8_t flag = 1;
	uint8_t temp = 0;
	uint8_t row_val = 0, col_val = 0;

	if(mode) flag = 1;

//	init_keyboard();	// 先把行列线设置为输出口，用在实验室STM32F407VET6板子需要添加此句
	ResetRow();			// 把行线拉低，列线设置为输入口，读取按键状态
	
	if(COL0_IN && COL1_IN && COL2_IN && COL3_IN)
	{
		flag = 1;
	}
	else if(flag && (COL0_IN == 0 || COL1_IN == 0 || COL2_IN == 0 || COL3_IN == 0))
	{
		delay_ms(50);
		temp = (COL0_IN << 0) | (COL1_IN << 1) | (COL2_IN << 2) | (COL3_IN << 3);
		temp &= 0x0f;
		flag = 0;
		if(temp != 0x0f)
		{
			switch(temp)
			{
			case 0x0e:
				col_val = 0x0e;
				break;
			case 0x0d:
				col_val = 0x0d;
				break;
			case 0x0b:
				col_val = 0x0b;
				break;
			case 0x07:
				col_val = 0x07;
				break;
			}
	//		init_keyboard();	// 用在实验室STM32F407VET6板子需要添加此句
			ResetCol();
			if(ROW0_IN == 0 || ROW1_IN == 0 || ROW2_IN == 0 || ROW3_IN == 0)
			{
				delay_ms(50);
				temp = (ROW0_IN << 4) | (ROW1_IN << 5) | (ROW2_IN << 6) | (ROW3_IN << 7);
				temp &= 0xf0;
				if(temp != 0xf0)
				{
					switch(temp)
					{
					case 0xe0:
						row_val = 0xe0;
						return (row_val | col_val);
					case 0xd0:
						row_val = 0xd0;
						return (row_val | col_val);
					case 0xb0:
						row_val = 0xb0;
						return (row_val | col_val);
					case 0x70:
						row_val = 0x70;
						return (row_val | col_val);
					}
					while((temp & 0xf0) != 0xf0)
					{
						temp = (ROW0_IN << 4) | (ROW1_IN << 5) | (ROW2_IN << 6) | (ROW3_IN << 7);
					}
				}
			}
		}
	}
	
	return 0;
}

#endif

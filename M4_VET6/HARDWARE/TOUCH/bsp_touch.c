#include <bsp.h>

_scan_dir scan_dir = 
{
    480,
	320,
};

_m_tp_dev tp_dev=
{
	TP_Init,
	FT6236_Scan,
	0,
	0, 
	0,
	0,	  	 		
};					

u8 TP_Init(void)
{
    if(FT6236_Init())
    {
        return 1;               // ��ʼ��ʧ��
    }
	tp_dev.touchtype = LCD_DIR;	// ����(1)��������(0)     
    return 0;
}


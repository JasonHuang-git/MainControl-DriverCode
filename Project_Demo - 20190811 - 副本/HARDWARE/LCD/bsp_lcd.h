#ifndef __BSP_LCD_H
#define __BSP_LCD_H		

#include <bsp.h>

// LCD��Ҫ������
typedef struct  
{										    
	uint16_t width;			 // LCD ���
	uint16_t height;		 // LCD �߶�
	uint16_t id;			 // LCD ID
	uint8_t  dir;			 // ���������������ƣ�0��������1��������	 
}_lcd_dev; 	  

// LCD����
extern _lcd_dev lcddev;  	  // ����LCD��Ҫ����

// LCD�Ļ�����ɫ�ͱ���ɫ	   
extern uint16_t  POINT_COLOR; // Ĭ�Ϻ�ɫ    
extern uint16_t  BACK_COLOR;  // ������ɫ.Ĭ��Ϊ��ɫ

//////////////////////////////////////////////////////////////////////////////////	 
//-----------------LCD�˿ڶ���---------------- 
//LCD��ַ�ṹ��
typedef struct
{
	vu16 LCD_REG;
	vu16 LCD_RAM;
} LCD_TypeDef;

#define LCD_BASE        ((uint32_t)(0x60000000 | 0x0001FFFE))
#define LCD             ((LCD_TypeDef *) LCD_BASE)
//////////////////////////////////////////////////////////////////////////////////

// ��Ļ����
#define VERTICAL   0  	  // ����
#define HORIZONTAL 1  	  // ����

// �л��������ĺ�
#define LCD_DIR HORIZONTAL  // Ĭ������

// ɨ�跽���� B5 B6 B7
//B5 = 0
#define L2R_U2D  0 // ������,���ϵ���
#define L2R_D2U  1 // ������,���µ���
#define R2L_U2D  2 // ���ҵ���,���ϵ���
#define R2L_D2U  3 // ���ҵ���,���µ���

//B5 = 1
#define U2D_L2R  4 // ���ϵ���,������
#define D2U_L2R  5 // ���µ���,������
#define U2D_R2L  6 // ���ϵ���,���ҵ���
#define D2U_R2L  7 // ���µ���,���ҵ���	 

// ע�⣺ɨ�跽��ֻ֧��B5 = 0 �����������������ʾ����ѡ��0��3��
// ������ʾ����ѡ��1��2��
// ʣ��������������ں�����ת���ɳ���ʵ�֣�����Ҫ�û�ѡ��
#define DFT_SCAN_DIR  L2R_U2D  // Ĭ�ϵ�ɨ�跽��

// �����������
#define LCD_BL_OFF       {PDout(6) = 0;}
#define LCD_BL_ON        {PDout(6) = 1;}


// ������ɫ
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE         	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 // ��ɫ
#define BRRED 			 0XFC07 // �غ�ɫ
#define GRAY  			 0X8430 // ��ɫ
// GUI��ɫ

#define DARKBLUE      	 0X01CF	// ����ɫ
#define LIGHTBLUE      	 0X7D7C	// ǳ��ɫ  
#define GRAYBLUE       	 0X5458 // ����ɫ
// ������ɫΪPANEL����ɫ 

#define LIGHTGREEN     	 0X841F // ǳ��ɫ
#define LGRAY 			 0XC618 // ǳ��ɫ(PANNEL),���屳��ɫ

#define LGRAYBLUE        0XA651 // ǳ����ɫ(�м����ɫ)
#define LBBLUE           0X2B12 // ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)

// �ⲿ���ú���
void LCD_Init(void);													   							// ��ʼ��
void LCD_DisplayOn(void);																			// ����ʾ
void LCD_DisplayOff(void);																			// ����ʾ
void LCD_Clear(uint16_t Color);	 																	// ����
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos);													// ���ù��
void LCD_DrawPoint(uint16_t x,uint16_t y);															// ����
void LCD_Fast_DrawPoint(uint16_t x,uint16_t y,uint16_t color);										// ���ٻ���
uint16_t  LCD_ReadPoint(uint16_t x,uint16_t y); 													// ���� 
void LCD_Draw_Circle(uint16_t x0,uint16_t y0,uint8_t r);						 					// ��Բ
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);								// ����
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);		   					// ������
void LCD_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t color);		   				// ��䵥ɫ
void LCD_Color_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t *color);				// ���ָ����ɫ
void LCD_ShowChar(uint16_t x,uint16_t y,uint8_t num,uint8_t size,uint8_t mode);						// ��ʾһ���ַ�
void LCD_ShowNum(uint16_t x,uint16_t y,uint32_t num,uint8_t len,uint8_t size);  					// ��ʾһ������
void LCD_ShowxNum(uint16_t x,uint16_t y,uint32_t num,uint8_t len,uint8_t size,uint8_t mode);		// ��ʾ ����
void LCD_ShowString(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t size,uint8_t *p);	// ��ʾһ���ַ���,12/16����
void DrawRGBImage(uint16_t Xpos, uint16_t Ypos, uint16_t Xsize, uint16_t Ysize, uint8_t *pData);
void LCD_DrawLine_color(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
// �ڲ�����
uint16_t  Read_ID(void);
void LCD_WriteReg(uint16_t LCD_Reg, uint16_t LCD_RegValue);
uint16_t  LCD_ReadReg(uint16_t LCD_Reg);
void LCD_WriteRAM_Prepare(void);       // ��ʼ�����ڴ�С
void LCD_WriteREG_Prepare(void);       // д����0x2c
void LCD_WriteRAM(uint16_t RGB_Code);
void LCD_Display_Dir(uint8_t dir);								    // ������Ļ��ʾ����
void LCD_Scan_Dir(uint8_t dir);
void LCD_Set_Window(uint16_t Xstart,uint16_t Xend,uint16_t Ystart,uint16_t Yend);	// ���ô���					   						   																			 

// LCD�ֱ�������
#define SSD_HOR_RESOLUTION		480		// LCDˮƽ�ֱ���
#define SSD_VER_RESOLUTION		320		// LCD��ֱ�ֱ���

// LCD������������
#define SSD_HOR_PULSE_WIDTH		1		// ˮƽ����
#define SSD_HOR_BACK_PORCH		46		// ˮƽǰ��
#define SSD_HOR_FRONT_PORCH		210		// ˮƽ����

#define SSD_VER_PULSE_WIDTH		1		// ��ֱ����
#define SSD_VER_BACK_PORCH		23		// ��ֱǰ��
#define SSD_VER_FRONT_PORCH		22		// ��ֱǰ��
// ���¼����������Զ�����
#define SSD_HT	(SSD_HOR_RESOLUTION+SSD_HOR_BACK_PORCH+SSD_HOR_FRONT_PORCH)
#define SSD_HPS	(SSD_HOR_BACK_PORCH)
#define SSD_VT 	(SSD_VER_RESOLUTION+SSD_VER_BACK_PORCH+SSD_VER_FRONT_PORCH)
#define SSD_VPS (SSD_VER_BACK_PORCH)

#endif  /* __BSP_LCD_H */

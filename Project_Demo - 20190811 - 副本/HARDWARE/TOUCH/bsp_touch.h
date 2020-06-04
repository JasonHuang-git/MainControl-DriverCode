#ifndef __BSP_TOUCH_H
#define __BSP_TOUCH_H
#include <bsp.h>   
#include <bsp_ft6236.h>


// ʹ�õ�����ʱ����ú꣬ʹ�õ�����ʱ��Ҫע�͵�
#define CT_TOUCH


#define TP_PRES_DOWN 0x80  // ����������	  
#define TP_CATH_PRES 0x40  // �а��������� 
#define CT_MAX_TOUCH  5    // ������֧�ֵĵ���,�̶�Ϊ5��


//���ݴ���������꣬�������㣬�������
typedef struct
{
    vu16 xs;
    vu16 ys;
}_scan_dir;

extern _scan_dir scan_dir;

//������������
typedef struct
{
	uint8_t (*init)(void);			// ��ʼ��������������
	uint8_t (*scan)(uint8_t);		// ɨ�败����.0,��Ļɨ��;1,��������;	 
	uint8_t (*adjust)(void);		// ������У׼ 
	uint16_t x[CT_MAX_TOUCH]; 		// ��ǰ����
	uint16_t y[CT_MAX_TOUCH];		// �����������5������,����������x[0],y[0]����:�˴�ɨ��ʱ,����������,��
	// x[4],y[4]�洢��һ�ΰ���ʱ������. 
	uint8_t  sta;					// �ʵ�״̬ 
	// b7:����1/�ɿ�0; 
	// b6:0,û�а�������;1,�а�������. 
	// b5:����
	// b4~b0:���ݴ��������µĵ���(0,��ʾδ����,1��ʾ����)
    /////////////////////������У׼����(����������ҪУ׼)//////////////////////								
	float xfac;					
	float yfac;
	short xoff;
	short yoff;	
	// b7:����1/�ɿ�0; 
	// b6:0,û�а�������;1,�а�������. 
	// b5:����
	// b4~b0:���ݴ��������µĵ���(0,��ʾδ����,1��ʾ����) 
	// �����Ĳ���,��������������������ȫ�ߵ�ʱ��Ҫ�õ�.
    // b7:0,������
    //    1,������ 
	uint8_t touchtype;
}_m_tp_dev;

extern _m_tp_dev tp_dev;	 	// ������������touch.c���涨��

//������оƬ��������	   
#define PEN  		PCin(0)  	// T_PEN
#define DOUT 		PCin(2)   	// T_MISO
#define TDIN 		PCout(3)  	// T_MOSI
#define TCLK 		PBout(10)  	// T_SCK
#define TCS  		PBout(9)  	// T_CS  

//����������
void TP_Write_Byte(uint8_t num);								// �����оƬд��һ������
uint16_t TP_Read_AD(uint8_t CMD);								// ��ȡADת��ֵ
uint16_t TP_Read_XOY(uint8_t xy);								// ���˲��������ȡ(X/Y)
uint8_t TP_Read_XY(uint16_t *x,uint16_t *y);					// ˫�����ȡ(X+Y)
uint8_t TP_Read_XY2(uint16_t *x,uint16_t *y);					// ����ǿ�˲���˫���������ȡ
void TP_Drow_Touch_Point(uint16_t x,uint16_t y,uint16_t color); // ��һ������У׼��
void TP_Draw_Big_Point(uint16_t x,uint16_t y,uint16_t color);	// ��һ�����
void TP_Save_Adjdata(void);										// ����У׼����
uint8_t TP_Get_Adjdata(void);									// ��ȡУ׼����
uint8_t TP_Adjust(void);										// ������У׼
void TP_Adj_Info_Show(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t x3,uint16_t y3,uint16_t fac);//��ʾУ׼��Ϣ

// ����/����������
uint8_t TP_Scan(uint8_t tp);	// ɨ��
uint8_t TP_Init(void);			// ��ʼ��

#endif


















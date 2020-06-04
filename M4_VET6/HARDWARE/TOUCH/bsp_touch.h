#ifndef __TOUCH_H__
#define __TOUCH_H__

#include <bsp.h>  

#define TP_PRES_DOWN 0x80  // 触屏被按下	  
#define TP_CATH_PRES 0x40  // 有按键按下了 
#define CT_MAX_TOUCH  5    // 电容屏支持的点数,固定为5点

//触摸相对坐标，用于运算，不需更改
typedef struct
{
    vu16 xs;
    vu16 ys;
}_scan_dir;

extern _scan_dir scan_dir;

//触摸屏控制器
typedef struct
{
	u8 (*init)(void);			// 初始化触摸屏控制器
	u8 (*scan)(u8);				// 扫描触摸屏.0,屏幕扫描;1,物理坐标;	 
	u16 x[CT_MAX_TOUCH]; 		// 当前坐标
	u16 y[CT_MAX_TOUCH];		// 电容屏有最多5组坐标,电阻屏则用x[0],y[0]代表:此次扫描时,触屏的坐标,用
	// x[4],y[4]存储第一次按下时的坐标. 
	u8  sta;					// 笔的状态 
	// b7:按下1/松开0; 
	// b6:0,没有按键按下;1,有按键按下. 
	// b5:保留
	// b4~b0:电容触摸屏按下的点数(0,表示未按下,1表示按下) 
	// 新增的参数,当触摸屏的左右上下完全颠倒时需要用到.
	// b0:0,竖屏(适合左右为X坐标,上下为Y坐标的TP)
	//    1,横屏(适合左右为Y坐标,上下为X坐标的TP) 
	u8 touchtype;
}_m_tp_dev;

extern _m_tp_dev tp_dev;	 	// 触屏控制器在touch.c里面定义

// 电容屏函数
u8 TP_Scan(u8 tp);	// 扫描
u8 TP_Init(void);	// 初始化

#endif


















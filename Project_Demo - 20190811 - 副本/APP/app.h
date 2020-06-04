#ifndef __APP_H__
#define __APP_H__

#include <bsp.h>

#define AMP_FACTOR    100
#define AMP2_FACTOR   30
#define MAX_POINTS    171
#define REF_RES       10000

#define OUTREF_RES    3000

typedef enum _error_case 
{
    r1_nc,
    r1_shortcut,
    r2_nc,
    r2_shortcut,
    r3_nc,
    r3_shortcut,
    r4_nc,
    r4_shortcut,
    normals
    
} error_case;

typedef enum cases 
{
    nc,
    shortcut,
    normal
    
} cases_cc;              

struct component 
{
    cases_cc status;
};

extern error_case error_code;
// 应用层函数声明区域
void DAC_Test_app(void);
void FFT_Test_app(void);
void AD9833_Set_Fre_app(uint32_t fre);
void display_menu(void);
void update_display(void);
float input_res_calc(void);
void measure_select(uint8_t mode);
void amp_freq_curve(void);
void get_amp(void);
void get_ADC2_amp(void);
float * Get_adc1_ch7_amp(void);
int res_judge(void);
uint8_t cut_cap(void);
float output_res_calc(void);
void display_menu_B(void);
uint8_t c1_fres(void);
uint8_t c2_fres(void);
uint8_t c3_fres(void);
uint16_t amp_freq_curve_new(void);
extern uint16_t ref_gain;
extern uint16_t ref_bw;

#endif

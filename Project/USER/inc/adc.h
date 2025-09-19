#ifndef __ADC_H
#define __ADC_H

#include "headfile.h"

//归一化
void normalize_data (void);

//电感采样，一阶低通滤波
void inductor_low_pass_filter();

//#define ADC_CHANNELS 6   // 6个ADC通道
//#define WINDOW_SIZE 4    // 滑动窗口大小


//// 初始化ADC滑动平均滤波器
//void adc_filter_init(void);

//// 滑动平均滤波函数
//uint16 adc_sliding_filter(uint8 channel, uint16 new_sample);


// 采样
//void sample_and_filter_all_channels();

//int16 I_Median_Average_Filter(int16 *ADC);
//void AD_Date_Fitier();

#endif

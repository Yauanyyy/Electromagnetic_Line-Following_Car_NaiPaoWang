#ifndef __ADC_H
#define __ADC_H

#include "headfile.h"

//��һ��
void normalize_data (void);

//��в�����һ�׵�ͨ�˲�
void inductor_low_pass_filter();

//#define ADC_CHANNELS 6   // 6��ADCͨ��
//#define WINDOW_SIZE 4    // �������ڴ�С


//// ��ʼ��ADC����ƽ���˲���
//void adc_filter_init(void);

//// ����ƽ���˲�����
//uint16 adc_sliding_filter(uint8 channel, uint16 new_sample);


// ����
//void sample_and_filter_all_channels();

//int16 I_Median_Average_Filter(int16 *ADC);
//void AD_Date_Fitier();

#endif

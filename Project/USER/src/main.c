/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2020,��ɿƼ�
 * All rights reserved.
 * ��������QQȺ��һȺ��179029047(����)  ��Ⱥ��244861897(����)  ��Ⱥ��824575535
 *
 * �����������ݰ�Ȩ������ɿƼ����У�δ��������������ҵ��;��
 * ��ӭ��λʹ�ò������������޸�����ʱ���뱣����ɿƼ��İ�Ȩ������
 *
 * @file       		main
 * @company	   		�ɶ���ɿƼ����޹�˾
 * @author     		��ɿƼ�(QQ790875685)
 * @version    		�鿴doc��version�ļ� �汾˵��
 * @Software 		MDK FOR C251 V5.60
 * @Target core		STC32G12K128
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2020-12-18
 ********************************************************************************************************************/

#include "headfile.h"
/*
 * ϵͳƵ�ʣ��ɲ鿴board.h�е� FOSC �궨���޸ġ�
 * board.h�ļ���FOSC��ֵ����Ϊ0,������Զ�����ϵͳƵ��Ϊ33.1776MHZ
 * ��board_init��,�Ѿ���P54��������Ϊ��λ
 * �����Ҫʹ��P54����,������board.c�ļ��е�board_init()������ɾ��SET_P54_RESRT����
 */
 
#define TIMER_0_DURATION 999
#define TIMER_1_DURATION 3
#define TIMER_2_DURATION 999
#define TIMER_3_DURATION 999
#define TIMER_4_DURATION 6
#define PWM_FREQUENCY_17k 17000


#if(TIMER_1_DURATION != 3)
#error "TIMER_1_DURATION!!!!!!!"               
#endif

#if(TIMER_4_DURATION != 6)
#error "TIMER_4_DURATION!!!!!!!"
#endif

#if(PWM_FREQUENCY_17k != 17000)
#error "PWM_FREQUENCY_17k!!!!!!!"
#endif


int CNT999 = 0;

void main()
{
	DisableGlobalIRQ();
	board_init();			// ��ʼ���Ĵ���,��ɾ���˾���롣
	
	// ��������ʼ��
	buzzer_init();
	
	// ��Ļ��ʼ��
	ips114_init();
	
	// ���ȳ�ʼ��
	//pwm_init(PWMB_CH3_P33, 50, 700);
	//pwm_init(PWMB_CH4_P77, 50, 700);
	
	// �����ʼ��
	pwm_init (PWMA_CH2P_P62, PWM_FREQUENCY_17k, 0);
	pwm_init (PWMA_CH4P_P66, PWM_FREQUENCY_17k, 0);
	pwm_init (PWMA_CH1P_P60, PWM_FREQUENCY_17k, 0);
	pwm_init (PWMA_CH3P_P64, PWM_FREQUENCY_17k, 0);
	
	// ��ͨ��ADC��ʼ������ʱֻ������  // ͨ����Ƶ��
	adc_init (ADC_P00, ADC_SYSclk_DIV_2); // ��1��б�ĺ�
	adc_init (ADC_P01, ADC_SYSclk_DIV_2); // ��2����
	adc_init (ADC_P05, ADC_SYSclk_DIV_2); // ��3����
	//adc_init(ADC_P06, ADC_SYSclk_DIV_2); // A
	adc_init (ADC_P13, ADC_SYSclk_DIV_2); // ��3����
	adc_init (ADC_P14, ADC_SYSclk_DIV_2); // ��2����
	adc_init (ADC_P16, ADC_SYSclk_DIV_2); // ��1��б�ĺ�
	//adc_init(ADC_P17, ADC_SYSclk_DIV_2); // B
	
	// ��ѹ���adc��ʼ��
	adc_init(ADC_P15, ADC_SYSclk_DIV_32);
	
	// ����ת���ڳ�ʼ��
	wireless_uart_init();
	
	// ������ʼ��
	//dl1a_init();
	
	// ��������ʼ��
	ctimer_count_init (CTIM0_P34);
	ctimer_count_init (CTIM3_P04);
	
	// imu963ra  ��ʼ��
	imu963ra_init();
	
	// imu963ra  offset
	IMU_offset();
	//imu963ra_offset_flag = 1;
	
	buzzer_beep(1);
	
	// �����ݵ�ѹ
	waitForFullCharge(1400);
	
	// ��ʱ��1��ʼ��
	pit_timer_ms (TIM_1, TIMER_1_DURATION); // 3ms
	
	// �������� ����Ϊֱ����ʻ����
	exitChargingMode(8000);
	
	// ��ʱ��4��ʼ��
	//pit_timer_ms (TIM_0, TIMER_0_DURATION);
	//pit_timer_ms (TIM_1, TIMER_1_DURATION); // 3ms
	//pit_timer_ms(TIM_2, TIMER_2_DURATION);
	//pit_timer_ms(TIM_3, TIMER_3_DURATION);// 1000ms timer 3 ��ʱ����ֱ����
	pit_timer_ms (TIM_4, TIMER_4_DURATION); // 6ms
	
	// ���ж������
	EnableGlobalIRQ();
	
	
	while (1)
	{
		WorkStatusSwitch();	
	}
}




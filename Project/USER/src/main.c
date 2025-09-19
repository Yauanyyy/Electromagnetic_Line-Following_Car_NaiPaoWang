/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2020,逐飞科技
 * All rights reserved.
 * 技术讨论QQ群：一群：179029047(已满)  二群：244861897(已满)  三群：824575535
 *
 * 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
 * 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
 *
 * @file       		main
 * @company	   		成都逐飞科技有限公司
 * @author     		逐飞科技(QQ790875685)
 * @version    		查看doc内version文件 版本说明
 * @Software 		MDK FOR C251 V5.60
 * @Target core		STC32G12K128
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2020-12-18
 ********************************************************************************************************************/

#include "headfile.h"
/*
 * 系统频率，可查看board.h中的 FOSC 宏定义修改。
 * board.h文件中FOSC的值设置为0,则程序自动设置系统频率为33.1776MHZ
 * 在board_init中,已经将P54引脚设置为复位
 * 如果需要使用P54引脚,可以在board.c文件中的board_init()函数中删除SET_P54_RESRT即可
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
	board_init();			// 初始化寄存器,勿删除此句代码。
	
	// 蜂鸣器初始化
	buzzer_init();
	
	// 屏幕初始化
	ips114_init();
	
	// 风扇初始化
	//pwm_init(PWMB_CH3_P33, 50, 700);
	//pwm_init(PWMB_CH4_P77, 50, 700);
	
	// 电机初始化
	pwm_init (PWMA_CH2P_P62, PWM_FREQUENCY_17k, 0);
	pwm_init (PWMA_CH4P_P66, PWM_FREQUENCY_17k, 0);
	pwm_init (PWMA_CH1P_P60, PWM_FREQUENCY_17k, 0);
	pwm_init (PWMA_CH3P_P64, PWM_FREQUENCY_17k, 0);
	
	// 八通道ADC初始化，暂时只用六个  // 通道，频率
	adc_init (ADC_P00, ADC_SYSclk_DIV_2); // 左1，斜改横
	adc_init (ADC_P01, ADC_SYSclk_DIV_2); // 左2，竖
	adc_init (ADC_P05, ADC_SYSclk_DIV_2); // 左3，横
	//adc_init(ADC_P06, ADC_SYSclk_DIV_2); // A
	adc_init (ADC_P13, ADC_SYSclk_DIV_2); // 右3，横
	adc_init (ADC_P14, ADC_SYSclk_DIV_2); // 右2，竖
	adc_init (ADC_P16, ADC_SYSclk_DIV_2); // 右1，斜改横
	//adc_init(ADC_P17, ADC_SYSclk_DIV_2); // B
	
	// 电压检测adc初始化
	adc_init(ADC_P15, ADC_SYSclk_DIV_32);
	
	// 无线转串口初始化
	wireless_uart_init();
	
	// 超声初始化
	//dl1a_init();
	
	// 编码器初始化
	ctimer_count_init (CTIM0_P34);
	ctimer_count_init (CTIM3_P04);
	
	// imu963ra  初始化
	imu963ra_init();
	
	// imu963ra  offset
	IMU_offset();
	//imu963ra_offset_flag = 1;
	
	buzzer_beep(1);
	
	// 检测电容电压
	waitForFullCharge(1400);
	
	// 定时器1初始化
	pit_timer_ms (TIM_1, TIMER_1_DURATION); // 3ms
	
	// 进入赛道 参数为直线行驶距离
	exitChargingMode(8000);
	
	// 定时器4初始化
	//pit_timer_ms (TIM_0, TIMER_0_DURATION);
	//pit_timer_ms (TIM_1, TIMER_1_DURATION); // 3ms
	//pit_timer_ms(TIM_2, TIMER_2_DURATION);
	//pit_timer_ms(TIM_3, TIMER_3_DURATION);// 1000ms timer 3 临时测试直线用
	pit_timer_ms (TIM_4, TIMER_4_DURATION); // 6ms
	
	// 总中断最后开启
	EnableGlobalIRQ();
	
	
	while (1)
	{
		WorkStatusSwitch();	
	}
}




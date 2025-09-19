#ifndef __HEADFILE_H_
#define __HEADFILE_H_


#include <stdlib.h>


#include "isr.h"
#include <string.h>
#include <stdio.h>
#include "intrins.h"
//------STC32G SDK等
#include "STC32Gxx.h"
#include "board.h"
#include "common.h"

//------逐飞科技单片机外设驱动头文件
#include "zf_uart.h"
#include "zf_gpio.h"
//#include "zf_iic.h"
#include "zf_adc.h"
#include "zf_spi.h"
#include "zf_tim.h"
#include "zf_pwm.h"
#include "zf_nvic.h"
#include "zf_exti.h"
#include "zf_delay.h"
#include "zf_eeprom.h"
#include "zf_fifo.h"

//------逐飞科技产品驱动头文件

// 显示模块相关头文件
#include "SEEKFREE_FONT.h"
#include "SEEKFREE_CONFIG.h"
#include "SEEKFREE_FUNCTION.h"
#include "SEEKFREE_OLED.h"
#include "SEEKFREE_18TFT.h"
#include "SEEKFREE_IPS114_SPI.h"
#include "SEEKFREE_IPS200_SPI.h"

// 传感器模块相关头文件

#include "SEEKFREE_ICM20602.h"
#include "SEEKFREE_IMU660RA.h"
#include "SEEKFREE_IMU963RA.h"
#include "SEEKFREE_TSL1401.h"
#include "SEEKFREE_DL1B.h"
#include "SEEKFREE_GPS_TAU1201.h"

// 通信模块相关头文件
#include "SEEKFREE_PRINTF.h"
#include "SEEKFREE_WIRELESS.h"
#include "SEEKFREE_BLE6A20.h"

// 注释掉暂时不用的头文件，可根据需要启用
// #include "SEEKFREE_MPU6050.h"
// #include "SEEKFREE_ABSOLUTE_ENCODER.h"
// #include "SEEKFREE_AT24C02.h"
// #include "SEEKFREE_BLUETOOTH_CH9141.h"
// #include "SEEKFREE_WIRELESS_CH573.h"
// #include "SEEKFREE_VIRSCO.h"
// #include "SEEKFREE_DL1A.h"

//------逐飞科技产品组件层文件

//自定义
#include "params.h"
#include "road.h"
#include "math.h"
#include "adc.h"
#include "speed.h"
#include "pid.h"
#include "isr.h"
#include "params.h"
#include "gyroscope.h"
#include "buzzer.h"
#include "charge.h"
#include "globals_ex.h"
#endif

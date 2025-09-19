#ifndef __GLOBALS_EX_H
#define __GLOBALS_EX_H

#include "headfile.h" // 包含项目的基础头文件

/*
 * ============================================================================
 *                              全局变量声明
 * ============================================================================
 * @note   此文件用于声明项目中使用的所有全局变量。
 *         所有变量的实体定义位于 "globals.c" 文件中。
 * @author yanyyy
 * @date   2025-09-19
 * ============================================================================
 */


//-----------------------------------------------------------------------------
// 1. 小车速度与行驶控制 (Speed & Drive Control)
//-----------------------------------------------------------------------------
extern uint8 setSpeed;
extern uint8 lowSpeed;
extern uint8 normalSpeed;
extern uint8 botSpeed;
extern int32 distance_set;
extern int32 distance_accu;
extern int16 angle_set;
extern int16 angle_accu;


//-----------------------------------------------------------------------------
// 2. 电池与充电管理 (Battery & Charging)
//-----------------------------------------------------------------------------
extern uint16 ad_result;
extern uint16 battery_voltage;
extern uint16 battery_voltage_temp;
extern uint8  charge_flag;


//-----------------------------------------------------------------------------
// 3. 电机与编码器 (Motor & Encoder)
//-----------------------------------------------------------------------------
extern int   encoderL;
extern int   encoderR;
extern int16 Lputout;
extern int16 Rputout;
extern float errorL;
extern float errorR;
extern float errorL_Before;
extern float errorR_Before;
extern float errorL_Before_Before;
extern float errorR_Before_Before;


//-----------------------------------------------------------------------------
// 4. PID 控制参数 (PID Control Parameters)
//-----------------------------------------------------------------------------
extern int16 motor_kp;
extern int16 motor_ki;
extern int16 motor_kd;
extern float adc_kp1;
extern float adc_kp2;
extern float adc_ki;
extern float adc_kd;
extern float gyro_kp;
extern float gyro_kd;
extern float fix;


//-----------------------------------------------------------------------------
// 5. 电磁传感器数据处理 (ADC Sensor Data)
//-----------------------------------------------------------------------------
extern uint16 adc_buffer_user[6][4];
extern uint8  adc_index_user[6];
extern uint32 adc_total_user[6];
extern uint16 adc_pre[6];
extern uint16 adc_guiyi[6];
extern uint16 adc_sum;
extern uint16 adc_max[6];
extern uint16 adc_min[6];


//-----------------------------------------------------------------------------
// 6. 循迹控制中间变量 (Steering Control Variables)
//-----------------------------------------------------------------------------
extern float adc_target;
extern float adcError;
extern float prevAdcError;
extern float adcIntegral;
extern float adcDerivative;
extern float adc_actual;
extern float adcError_abs;
extern int   adcError_int;
extern int   adc_actual_int;


//-----------------------------------------------------------------------------
// 7. IMU 姿态数据 (IMU Attitude Data)
//-----------------------------------------------------------------------------
extern int16 gyro_x_raw;
extern int16 gyro_y_raw;
extern int16 gyro_z_raw;
extern int16 acc_x_raw;
extern int16 acc_y_raw;
extern int16 acc_z_raw;
extern int gyroX;
extern int gyroY;
extern int gyroZ;
extern int accelX;
extern int accelY;
extern int accelZ;
extern float angleX_gyro;
extern float angleY_gyro;
extern float angleZ_gyro;
extern uint8 imu963ra_offset_flag;
extern int   offset_gx;
extern int   offset_gy;
extern int   offset_gz;
extern int   offset_accx;
extern int   offset_accy;
extern int   offset_accz;
extern int32 offset_acc_temp;
extern int gyroZ_before;
extern int gyroZ_Derivative;
extern int gyroZ_min;


//-----------------------------------------------------------------------------
// 8. 赛道元素处理 (Road Element Handling)
//-----------------------------------------------------------------------------
extern RoadItems currItem;
extern RoadItems lastItem;
extern uint8 get_in_island_FLAG;
extern uint8 before_get_in_island;
extern uint8 circle_direc_FLAG;
extern uint8 jumpIsland;
extern uint8 block_FLAG;
extern uint8 ramp_FLAG;
extern uint8 order_FLAG;
extern uint16 outIslandTurn;
extern uint16 outIslandStraight;
extern uint16 blockLeft;
extern uint16 blockRight;
extern uint16 blockBack;
extern uint16 rampset;


//-----------------------------------------------------------------------------
// 9. UI界面与通信 (UI & Communication)
//-----------------------------------------------------------------------------
extern char  text[24];
extern uint8 screenItem;
extern uint8 oscilloscopeItem;
extern uint8 paramChoice;
extern UARTN_enum UART_Using;

#endif

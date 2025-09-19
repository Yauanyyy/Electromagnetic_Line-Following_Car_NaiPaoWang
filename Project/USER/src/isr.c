///*********************************************************************************************************************
// * COPYRIGHT NOTICE
// * Copyright (c) 2020,逐飞科技
// * All rights reserved.
// * 技术讨论QQ群：一群：179029047(已满)  二群：244861897(已满)  三群：824575535
// *
// * 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
// * 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
// *
// * @file       		isr
// * @company	   		成都逐飞科技有限公司
// * @author     		逐飞科技(QQ790875685)
// * @version    		查看doc内version文件 版本说明
// * @Software 			MDK FOR C251 V5.60
// * @Target core		STC32G12K128
// * @Taobao   			https://seekfree.taobao.com/
// * @date       		2020-4-14
// ********************************************************************************************************************/
#include "headfile.h"
#include "globals_ex.h"

//UART1中断
void UART1_Isr() interrupt 4
{
    uint8 res;
	static uint8 dwon_count;
    if(UART1_GET_TX_FLAG)
    {
        UART1_CLEAR_TX_FLAG;
        busy[1] = 0;
    }
    if(UART1_GET_RX_FLAG)
    {
        UART1_CLEAR_RX_FLAG;
        res = SBUF;
        //程序自动下载
        if(res == 0x7F)
        {
            if(dwon_count++ > 20)
                IAP_CONTR = 0x60;
        }
        else
        {
            dwon_count = 0;
        }
    }
}

//UART2中断
void UART2_Isr() interrupt 8
{
    if(UART2_GET_TX_FLAG)
	{
        UART2_CLEAR_TX_FLAG;
		busy[2] = 0;
	}
    if(UART2_GET_RX_FLAG)
	{
        UART2_CLEAR_RX_FLAG;
		//接收数据寄存器为：S2BUF
		gps_uart_callback(S2BUF);
	}
}


//UART3中断
void UART3_Isr() interrupt 17
{
    if(UART3_GET_TX_FLAG)
	{
        UART3_CLEAR_TX_FLAG;
		busy[3] = 0;
	}
    if(UART3_GET_RX_FLAG)
	{
        UART3_CLEAR_RX_FLAG;
		//接收数据寄存器为：S3BUF

	}
}


//UART4中断
void UART4_Isr() interrupt 18
{
    if(UART4_GET_TX_FLAG)
	{
        UART4_CLEAR_TX_FLAG;
		busy[4] = 0;
	}
    if(UART4_GET_RX_FLAG)
	{
        UART4_CLEAR_RX_FLAG;

		//接收数据寄存器为：S4BUF;
		if(wireless_module_uart_handler != NULL)
		{
			// 该函数为函数指针
			// 再初始化无线模块的时候，设置该函数地址
			wireless_module_uart_handler(S4BUF);
		}
	}
}

#define LED P52
void INT0_Isr() interrupt 0
{
	LED = 0;	//点亮LED
}
void INT1_Isr() interrupt 2
{

}
void INT2_Isr() interrupt 10
{
	INT2_CLEAR_FLAG;  //清除中断标志
}
void INT3_Isr() interrupt 11
{
	INT3_CLEAR_FLAG;  //清除中断标志
}

void INT4_Isr() interrupt 16
{
	INT4_CLEAR_FLAG;  //清除中断标志
}

void TM0_Isr() interrupt 1
{

}

/**
 * @brief  Timer1 中断服务程序 (高频数据采集)
 * @param  None
 * @return void
 * @note   此中断负责执行以下任务：
 *         1. 读取左右电机的编码器值。
 *         2. 触发并滤波电感的ADC采样。
 *         3. 滤波陀螺仪数据。
 *         4. 当 `distance_set` 或 `angle_set` 不为零时，累加行驶距离和转弯角度。
 * @author yanyyy
 * @date   2025-09-19
 */
void TM1_Isr() interrupt 3
{

	// 超声测距;

//	    dl1a_get_distance();
//	    if (dl1a_distance_mm > 8000)
//	    {
//	        dl1a_distance_mm = 8000;
//	    }

	// 编码器读取带方向
	// 左侧
	encoderL = (P35 == 1) ? (ctimer_count_read (CTIM0_P34) ) : (ctimer_count_read (CTIM0_P34) * -1);
	ctimer_count_clean (CTIM0_P34);

	// 右侧
	encoderR = (P53 == 1) ? (ctimer_count_read (CTIM3_P04) * -1) : (ctimer_count_read (CTIM3_P04) );
	ctimer_count_clean (CTIM3_P04);


//	// 编码器读取不带方向
//    // 左侧
//    encoderL = ctimer_count_read(CTIM0_P34);
//    ctimer_count_clean(CTIM0_P34);
//
//    // 右侧
//    encoderR = ctimer_count_read(CTIM3_P04);
//    ctimer_count_clean(CTIM3_P04);



	//电感采样，均值滤波
//	AD_Date_Fitier();
	
//	//电感采样，滑动平均滤波
//	sample_and_filter_all_channels();
	
	//电感采样，一阶低通滤波
	inductor_low_pass_filter();

	// 距离累计
	if (distance_set != 0)
	{
		distance_accu += encoderL + encoderR;
		
		if(distance_accu < 0) distance_accu = 0;
		
		if (distance_accu >= distance_set)
		{
			distance_accu = 0;
			distance_set = 0;
		}
	}
	
//	if(imu963ra_offset_flag == 1)
//	{
		//陀螺仪采样，一阶低通滤波 + 缩小
		gyro_low_pass_filter();
	
		//加速计采样，一阶低通滤波
		//acc_low_pass_filter();
	
//	}
//	else
//	{
//		imu963ra_get_gyro();
//		//imu963ra_get_acc();
//	}


	// 角度累计
	if (angle_set != 0)
	{
		//gyroY = imu963ra_gyro_transition(gyro_y_raw);//换算角速度 单位为°/s
		gyroZ_min = gyroZ/128;
		angle_accu += gyroZ_min;
		
		if ((angle_set > 0 && angle_accu > angle_set) || 
			(angle_set < 0 && angle_accu < angle_set))
		{
			angle_accu = 0;
			angle_set = 0;
		}
	}

}
void TM2_Isr() interrupt 12
{
	TIM2_CLEAR_FLAG;  //清除中断标志
	
}
void TM3_Isr() interrupt 19
{
	TIM3_CLEAR_FLAG; //清除中断标志
	
}

/**
 * @brief  Timer4 中断服务程序 (主控制逻辑)
 * @param  None
 * @return void
 * @note   此中断以较低频率运行，
 *         执行完整的 "感知-决策-执行" 循环：
 *         1. 感知: 对采集到的原始数据进行归一化处理。
 *         2. 决策: 调用 `RoadItemsManager` 判断赛道元素，并通过 `switch` 语句选择当前的运动策略。
 *         3. 执行: 根据策略计算出的 `setSpeed` 和 `fix`，调用 `putOut` 函数驱动电机。
 *         4. 保护: 包含一个脱轨保护机制，在信号丢失时停止电机。
 * @author yanyyy
 * @date   2025-09-19
 */
void TM4_Isr() interrupt 20
{
	TIM4_CLEAR_FLAG; //清除中断标志
	//ccd_collect();	 //CCD采集数据
	
	// --- 1. 感知: 数据处理 ---
	normalize_data();
	adc_sum = adc_guiyi[1] + adc_guiyi[2] + adc_guiyi[3] + adc_guiyi[4] + adc_guiyi[5];
	
	 // --- 2. 决策: 状态判断与策略选择 ---
	
	RoadItemsManager(); // 根据传感器状态，更新当前赛道元素 `currItem`
	
	// 根据当前赛道元素，选择对应的运动策略函数
	switch (currItem)   // 不同的方式处理fix
    {
    case stop:
        finalStop();
        break;
	
	case forward:
        Gostraight();
        break;

    case Lisland:
        Lget_in_island();
        break;
    case Risland:
        Rget_in_island();
        break;

    case L_at_island:
        Lat_island();
        break;
    case R_at_island:
        Rat_island();
        break;
	
    case L_out_island:
        Lget_out_island();
        break;
    case R_out_island:
        Rget_out_island();
        break;	
	
	case cross:
        Cross();
        break;

	case line:
		Follow();
	    break;
	
	case line_CBH:
		Follow_CBH();
		break;

	case left90:
		Left90();
		break;

	case right90:
		Right90();
		break;
	
    default:
        Follow();
        break;
    }

	//脱轨保护
	//if ((adc_pre[0]+adc_pre[1]+adc_pre[2]+adc_pre[3]+adc_pre[4]+adc_pre[5] < 200) && (block_FLAG == 0))
	
	// --- 3. 执行 & 4. 保护 ---
    // 脱轨保护：如果所有电感的信号总和过低，则认为已脱轨，停止电机
	if (adc_pre[1] + adc_pre[2] + adc_pre[3] + adc_pre[4] + adc_pre[5] < 300)
	{
		motorPWMOut (LEFT, 0);
		motorPWMOut (RIGHT, 0);
	}

	else // 正常情况下，执行速度闭环控制
	{
        // setSpeed 是基础速度，fix 是方向环PID计算出的差速修正值
		putOut (LEFT, setSpeed + fix);
		putOut (RIGHT, setSpeed - fix);
	}

}

//void  INT0_Isr()  interrupt 0;
//void  TM0_Isr()   interrupt 1;
//void  INT1_Isr()  interrupt 2;
//void  TM1_Isr()   interrupt 3;
//void  UART1_Isr() interrupt 4;
//void  ADC_Isr()   interrupt 5;
//void  LVD_Isr()   interrupt 6;
//void  PCA_Isr()   interrupt 7;
//void  UART2_Isr() interrupt 8;
//void  SPI_Isr()   interrupt 9;
//void  INT2_Isr()  interrupt 10;
//void  INT3_Isr()  interrupt 11;
//void  TM2_Isr()   interrupt 12;
//void  INT4_Isr()  interrupt 16;
//void  UART3_Isr() interrupt 17;
//void  UART4_Isr() interrupt 18;
//void  TM3_Isr()   interrupt 19;
//void  TM4_Isr()   interrupt 20;
//void  CMP_Isr()   interrupt 21;
//void  I2C_Isr()   interrupt 24;
//void  USB_Isr()   interrupt 25;
//void  PWM1_Isr()  interrupt 26;
//void  PWM2_Isr()  interrupt 27;
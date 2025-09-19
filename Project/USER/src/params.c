#include "headfile.h"

// ============================================================================
//                          主工作状态切换
// ============================================================================

/**
 * @brief  根据拨码开关(P75, P76)的状态，切换小车的工作模式
 * @param  None
 * @return void
 * @note   此函数是顶层的工作模式分发器。
 *         - P76 P75 = 00: 无操作
 *         - P76 P75 = 01: 虚拟示波器模式 (发送位置数据)
 *         - P76 P75 = 10: 虚拟示波器模式 (发送电机数据)
 *         - P76 P75 = 11: 屏幕UI显示模式
 * @author yanyyy
 * @date   2025-09-19
 */
void WorkStatusSwitch()
{
    if (!P75 && !P76)
    {
        // 模式 00: 保留，无操作
    }
    else if (P75 && !P76)
    {
        // 模式 01: 发送位置相关数据到上位机
        Oscilloscope_Position();
    }
    else if (!P75 && P76)
    {
        // 模式 10: 发送电机相关数据到上位机
        Oscilloscope_Motor();
    }
    else
    {
        // 模式 11: 启用屏幕UI显示与参数调节
        Screen_All();
    }
    
    /*
     * // --- 以下为一种备选的、更复杂的示波器切换逻辑 ---
     * // 通过按键 P70 在不同的示波器界面间切换
     * if (!P75 && !P76)
     * {
     *     // 00
     * }
     * else if ((p75 ^ p76))
     * {
     *     switch (screenItem)
     *     {
     *         case 0:
     *             Oscilloscope_Motor();
     *             if (!P70)
     *             {
     *                 ips114_clear(WHITE);
     *                 sprintf(text, "Oscilloscope_Position");
     *                 ips114_showstr(0, 0, text);
     *                 screenItem = 1;
     *                 while (!P70);
     *             }
     *             break;
     *         // ... 其他 case ...
     *     }
     * }
     * else
     * {
     *     Screen_All();
     * }
     */
}


// ============================================================================
//                          虚拟示波器数据发送
// ============================================================================

/**
 * @brief  通过无线串口发送电机相关数据
 * @param  None
 * @return void
 * @note   发送格式为 ",目标速度,左编码器值,右编码器值\r\n"。
 *         用于上位机软件实时绘图。
 * @author yanyyy
 * @date   2025-09-19
 */
void Oscilloscope_Motor()
{
    sprintf(text, ",%d,%d,%d\r\n", setSpeed, encoderL, encoderR);
    wireless_uart_send_buff(text, sizeof(text) - 1);
}

/**
 * @brief  通过无线串口发送位置与姿态控制数据
 * @param  None
 * @return void
 * @note   发送格式为 ",循迹偏差,矫正,Z轴角加速度\r\n"。
 * @author yanyyy
 * @date   2025-09-19
 */
void Oscilloscope_Position()
{
    adc_actual_int = adc_actual;
    adcError_int = adcError * 100;
    sprintf(text, ",%d,%d,%d\r\n", adcError_int, adc_actual_int, gyroZ_Derivative);
    wireless_uart_send_buff(text, sizeof(text) - 1);
}

/**
 * @brief  通过无线串口发送陀螺仪原始数据和累计角度
 * @param  None
 * @return void
 * @note   发送格式为 ",gyro_x,gyro_y,gyro_z,累计角度\r\n"。
 * @author yanyyy
 * @date   2025-09-19
 */
void Oscilloscope_Gryo()
{
    sprintf(text, ",%d,%d,%d,%d\r\n", imu963ra_gyro_x, imu963ra_gyro_y, imu963ra_gyro_z, angle_accu);
    wireless_uart_send_buff(text, sizeof(text) - 1);
}


// ============================================================================
//                          屏幕UI显示与参数调节
// ============================================================================

/**
 * @brief  屏幕UI主状态机，负责在不同显示界面间切换
 * @param  None
 * @return void
 * @note   使用全局变量 `screenItem` 作为状态。
 *         按下 P70 键会切换到下一个界面。
 * @author yanyyy
 * @date   2025-09-19
 */
void Screen_All()
{
    switch (screenItem)
    {
        case 0:
            adcScreen();
            if (!P70)
            {
                screenItem = 1;
                paramChoice = 0;
                ips114_clear(WHITE);
                while (!P70);
            }
            break;
        case 1:
            preScreen();
            if (!P70)
            {
                screenItem = 2;
                paramChoice = 0;
                ips114_clear(WHITE);
                while (!P70);
            }
            break;
        case 2:
            pidScreen();
            if (!P70)
            {
                screenItem = 3;
                paramChoice = 0;
                ips114_clear(WHITE);
                while (!P70);
            }
            break;
        case 3:
            gyroScreen();
            if (!P70)
            {
                screenItem = 0;
                paramChoice = 0;
                ips114_clear(WHITE);
                while (!P70);
            }
            break;
        default:
            break;
    }
}

/**
 * @brief  显示ADC、速度和编码器数据的界面
 * @param  None
 * @return void
 * @note   使用 `paramChoice` 切换子界面。
 *         - P73: 在'电感'和'编码器'子界面间切换。
 *         - P71/P72: 在'电感'子界面下，可以减/加 `setSpeed`。
 * @author yanyyy
 * @date   2025-09-19
 */
void adcScreen()
{
    switch (paramChoice)
    {
        case 0: // 显示电感原始值和归一化值
            sprintf(text, "pre0 %04d    gy0 %03d", adc_pre[0], adc_guiyi[0]);
            ips114_showstr(0, 0, text);
            sprintf(text, "pre1 %04d    gy1 %03d", adc_pre[1], adc_guiyi[1]);
            ips114_showstr(0, 1, text);
            sprintf(text, "pre2 %04d    gy2 %03d", adc_pre[2], adc_guiyi[2]);
            ips114_showstr(0, 2, text);
            sprintf(text, "pre3 %04d    gy3 %03d", adc_pre[3], adc_guiyi[3]);
            ips114_showstr(0, 3, text);
            sprintf(text, "pre4 %04d    gy4 %03d", adc_pre[4], adc_guiyi[4]);
            ips114_showstr(0, 4, text);
            sprintf(text, "pre5 %04d    gy5 %03d", adc_pre[5], adc_guiyi[5]);
            ips114_showstr(0, 5, text);
            sprintf(text, "setSpeed:%d", setSpeed);
            ips114_showstr(0, 6, text);
            sprintf(text, "currItem:%d", currItem);
            ips114_showstr(0, 7, text);

            if (!P73) { paramChoice = 1; ips114_clear(WHITE); while (!P73); } // 切换到下一参数页
            if (!P71) { setSpeed -= 1; while (!P71); }                        // 减速
            if (!P72) { setSpeed += 1; while (!P72); }                        // 加速
            break;

        case 1: // 显示编码器值
            sprintf(text, "encoderR:%d  ", encoderR);
            ips114_showstr(0, 1, text);
            sprintf(text, "encoderL:%d  ", encoderL);
            ips114_showstr(0, 2, text);
            sprintf(text, "setSpeed:%d", setSpeed);
            ips114_showstr(0, 4, text);

            if (!P73) { paramChoice = 0; ips114_clear(WHITE); while (!P73); } // 切换回上一参数页
            break;

        default:
            break;
    }
}

/**
 * @brief  显示传感器最大/最小标定值的界面
 * @param  None
 * @return void
 * @note   使用 `paramChoice` 在最小值和最大值显示页之间切换。
 *         - P73: 在 `adc_min` 和 `adc_max` 显示之间切换。
 * @author yanyyy
 * @date   2025-09-19
 */
void preScreen()
{
    switch (paramChoice)
    {
        case 0: // 显示最小值
            sprintf(text, "adc_min[0]:%d  ", adc_min[0]);
			ips114_showstr(0, 0, text);
			sprintf(text, "adc_min[1]:%d  ", adc_min[1]);
			ips114_showstr(0, 1, text);
			sprintf(text, "adc_min[2]:%d  ", adc_min[2]);
			ips114_showstr(0, 2, text);
			sprintf(text, "adc_min[3]:%d  ", adc_min[3]);
			ips114_showstr(0, 3, text);
			sprintf(text, "adc_min[4]:%d  ", adc_min[4]);
			ips114_showstr(0, 4, text);
			sprintf(text, "adc_min[5]:%d  ", adc_min[5]);
			ips114_showstr(0, 5, text);

            if (!P73) { paramChoice = 1; ips114_clear(WHITE); while (!P73); }
            break;

        case 1: // 显示最大值
            sprintf(text, "adc_max[0]:%d  ", adc_max[0]);
			ips114_showstr(0, 0, text);
			sprintf(text, "adc_max[1]:%d  ", adc_max[1]);
			ips114_showstr(0, 1, text);
			sprintf(text, "adc_max[2]:%d  ", adc_max[2]);
			ips114_showstr(0, 2, text);
			sprintf(text, "adc_max[3]:%d  ", adc_max[3]);
			ips114_showstr(0, 3, text);
			sprintf(text, "adc_max[4]:%d  ", adc_max[4]);
			ips114_showstr(0, 4, text);
			sprintf(text, "adc_max[5]:%d  ", adc_max[5]);
			ips114_showstr(0, 5, text);

            if (!P73) { paramChoice = 0; ips114_clear(WHITE); while (!P73); }
            break;
    }
}

/**
 * @brief  显示并调节循迹PID参数的界面
 * @param  None
 * @return void
 * @note   使用 `paramChoice` 切换要调节的PID参数 (kp1, ki, kd, kp2)。
 *         - P73: 循环切换要调节的参数。
 *         - P71/P72: 减/加当前选中的参数值。
 * @author yanyyy
 * @date   2025-09-19
 */
void pidScreen()
{
	switch (paramChoice)
	{
		case 0: // 调节 adc_kp1
			sprintf (text, "adcError:%f  ", adcError );
			ips114_showstr (0, 0, text);
			sprintf (text, "adcIntegral:%f  ", adcIntegral );
			ips114_showstr (0, 1, text);
			sprintf (text, "adcDerivative:%f  ", adcDerivative );
			ips114_showstr (0, 2, text);
			sprintf (text, "adc_actual:%f  ", adc_actual );
			ips114_showstr (0, 3, text);
			sprintf (text, "adc_kp1:%f", adc_kp1 );
			ips114_showstr (0, 5, text);
		
			if (!P73) { paramChoice = 1; ips114_clear(WHITE); while (!P73); }
            if (!P71) { adc_kp1 -= 1; while (!P71); }
            if (!P72) { adc_kp1 += 1; while (!P72); }
			break;
			
		case 1: // 调节 adc_ki
			sprintf (text, "adcError:%f  ", adcError );
			ips114_showstr (0, 0, text);
			sprintf (text, "adcIntegral:%f  ", adcIntegral );
			ips114_showstr (0, 1, text);
			sprintf (text, "adcDerivative:%f  ", adcDerivative );
			ips114_showstr (0, 2, text);
			sprintf (text, "adc_actual:%f  ", adc_actual );
			ips114_showstr (0, 3, text);
			sprintf (text, "adc_ki:%f", adc_ki );
			ips114_showstr (0, 5, text);
		
			if (!P73) { paramChoice = 2; ips114_clear(WHITE); while (!P73); }
            if (!P71) { adc_ki -= 0.1; while (!P71); }
            if (!P72) { adc_ki += 0.1; while (!P72); }
			break;
			
		case 2: // 调节 adc_kd
			sprintf (text, "adcError:%f  ", adcError );
			ips114_showstr (0, 0, text);
			sprintf (text, "adcIntegral:%f  ", adcIntegral );
			ips114_showstr (0, 1, text);
			sprintf (text, "adcDerivative:%f  ", adcDerivative );
			ips114_showstr (0, 2, text);
			sprintf (text, "adc_actual:%f  ", adc_actual );
			ips114_showstr (0, 3, text);
			sprintf (text, "adc_kd:%f  x50", adc_kd );
			ips114_showstr (0, 5, text);
		
			if (!P73) { paramChoice = 3; ips114_clear(WHITE); while (!P73); }
            if (!P71) { adc_kd -= 50; while (!P71); }
            if (!P72) { adc_kd += 50; while (!P72); }
			break;
			
		case 3: // 调节 adc_kp2
			sprintf (text, "adcError:%f  ", adcError );
			ips114_showstr (0, 0, text);
			sprintf (text, "adcIntegral:%f  ", adcIntegral );
			ips114_showstr (0, 1, text);
			sprintf (text, "adcDerivative:%f  ", adcDerivative );
			ips114_showstr (0, 2, text);
			sprintf (text, "adc_actual:%f  ", adc_actual );
			ips114_showstr (0, 3, text);
			sprintf (text, "adc_kp2:%f  x0.1", adc_kp2 );
			ips114_showstr (0, 5, text);
		
			if (!P73) { paramChoice = 0; ips114_clear(WHITE); while (!P73); }
            if (!P71) { adc_kp2 -= 0.1; while (!P71); }
            if (!P72) { adc_kp2 += 0.1; while (!P72); }
			break;	
	}
}

/**
 * @brief  显示陀螺仪和加速度计数据的界面
 * @param  None
 * @return void
 * @note   使用 `paramChoice` 在陀螺仪和加速度计显示页之间切换。
 *         - P73: 在两页之间切换。
 * @author yanyyy
 * @date   2025-09-19
 */
void gyroScreen()
{
	switch (paramChoice)
	{
		case 0: // 显示陀螺仪数据
            ips114_showstr(0, 0, "gyro.x=");
            ips114_showstr(0, 1, "gyro.y=");
            ips114_showstr(0, 2, "gyro.z=");
			ips114_showstr(0, 3, "angle_accu=");
            ips114_showint16(6*10, 0, imu963ra_gyro_x);
            ips114_showint16(6*10, 1, imu963ra_gyro_y);
            ips114_showint16(6*10, 2, imu963ra_gyro_z);
			ips114_showint16(90, 3, angle_accu);
		
			if (!P73) { paramChoice = 1; ips114_clear(WHITE); while (!P73); }
            break;
			
		case 1: // 显示加速度计数据
			ips114_showstr(0, 0, "acc.x=");
            ips114_showstr(0, 1, "acc.y=");
            ips114_showstr(0, 2, "acc.z=");
            ips114_showint16(6*10, 0, imu963ra_acc_x);
            ips114_showint16(6*10, 1, imu963ra_acc_y);
            ips114_showint16(6*10, 2, imu963ra_acc_z);
			ips114_showstr(0, 3, "accelX=");
            ips114_showstr(0, 4, "accelY=");
            ips114_showstr(0, 5, "accelZ=");
            ips114_showint16(6*10, 3, accelX);
            ips114_showint16(6*10, 4, accelY);
            ips114_showint16(6*10, 5, accelZ);			
			
			if (!P73) { paramChoice = 0; ips114_clear(WHITE); while (!P73); }
            break;
	}
}


// ============================================================================
//                       无线数据发送 (屏幕内容替代)
// ============================================================================

/**
 * @brief  通过无线串口发送PID相关数据
 * @param  None
 * @return void
 * @note   是 `pidScreen` 的无线替代版本。
 *         依赖 `Z_change()` 函数。
 * @author yanyyy
 * @date   2025-09-19
 */
void wireless_pidScreen()
{
	sprintf (text, "adcError:%d  adcIntegral:%d  \n", adcError, adcIntegral);
	wireless_uart_send_buff (text, sizeof (text) - 1);

	sprintf (text, "adcDerivative:%d  adc_actual:%d  \n", adcDerivative, adc_actual);
	wireless_uart_send_buff (text, sizeof (text) - 1);

	sprintf (text, "adc_kp1:%d  change:adc_kd:%d  \n", Z_change (adc_kp1), Z_change (adc_kd) );
	wireless_uart_send_buff (text, sizeof (text) - 1);
}

/**
 * @brief  通过无线串口发送归一化后的电感数据
 * @param  None
 * @return void
 * @note   依赖 `Z_change()` 函数。
 * @author yanyyy
 * @date   2025-09-19
 */
void wireless_guiyiScreen()
{
	sprintf (text, "adc_guiyi[0]:%d  adc_guiyi[1]:%d  \n", adc_guiyi[0], adc_guiyi[1]);
	wireless_uart_send_buff (text, sizeof (text) - 1);

	sprintf (text, "adc_guiyi[2]:%d  adc_guiyi[3]:%d  \n", adc_guiyi[2], adc_guiyi[3]);
	wireless_uart_send_buff (text, sizeof (text) - 1);

	sprintf (text, "adc_guiyi[4]:%d  adc_guiyi[5]:%d  \n", adc_guiyi[4], adc_guiyi[5]);
	wireless_uart_send_buff (text, sizeof (text) - 1);

	sprintf (text, "change:adc_kp1:%d  \n", Z_change (adc_kp1) );
	wireless_uart_send_buff (text, sizeof (text) - 1);
}

/**
 * @brief  通过无线串口发送电机和编码器数据
 * @param  None
 * @return void
 * @note   是 `adcScreen` 部分功能的无线替代版本。
 * @author yanyyy
 * @date   2025-09-19
 */
void wireless_adcScreen()
{
	sprintf (text, "Rputout:%d   \n", Rputout);
	wireless_uart_send_buff (text, sizeof (text) - 1);

	sprintf (text, "Lputout:%d   \n", Lputout);
	wireless_uart_send_buff (text, sizeof (text) - 1);


	sprintf (text, "encoderR:%d   \n", encoderR);
	wireless_uart_send_buff (text, sizeof (text) - 1);

	sprintf (text, "encoderL:%d   \n", encoderL);
	wireless_uart_send_buff (text, sizeof (text) - 1);


	sprintf (text, "  \n", setSpeed);
	wireless_uart_send_buff (text, sizeof (text) - 1);
}









































//#include "headfile.h"

//void WorkStatusSwitch()
//{
//	 if (!P75 && !P76) {
//        // 00
//    }
//    else if (P75 && !P76) {
//        Oscilloscope_Position();
//    }
//    else if (!P75 && P76) {
//        Oscilloscope_Motor();
//    }
//    else {
//        Screen_All();
//    }
//}


////    if (!P75 && !P76) {
////        // 00
////    }
////	
////	
////    else if ((p75 ^ p76)) {
////        switch (screenItem)
////		{
////			case 0:
////				Oscilloscope_Motor();
////				if (!P70)
////				{
////					ips114_clear(WHITE);
////					sprintf(text, "Oscilloscope_Position");
////					ips114_showstr(0, 0, text);
////					
////					screenItem = 1;
////					while (!P70);
////				}
////				break;
////			case 1:
////				Oscilloscope_Position();
////				if (!P70)
////				{
////					ips114_clear(WHITE);
////					sprintf(text, "Oscilloscope_Gryo");
////					ips114_showstr(0, 0, text);
////					
////					screenItem = 2;
////					while (!P70);
////				}
////				break;
////			case 2:
////				Oscilloscope_Gryo();
////				if (!P70)
////				{
////					ips114_clear(WHITE);
////					sprintf(text, "Oscilloscope_Motor");
////					ips114_showstr(0, 0, text);
////					
////					screenItem = 0;
////					while (!P70);
////				}
////				break;
////			default:
////				break;
////		}
////    }

////	
////	
////    else {
////        Screen_All();
////    }
////}

//	
//void Oscilloscope_Motor()
//{

//	sprintf (text, ",%d,%d,%d\r\n", setSpeed, encoderL, encoderR);
//	wireless_uart_send_buff (text, sizeof (text) - 1);

//}



//void Oscilloscope_Position()
//{
//	adc_actual_int = adc_actual;
//	adcError_int = adcError * 100;
//	sprintf(text, ",%d,%d,%d\r\n", adcError_int, adc_actual_int, gyroZ_Derivative);

//	wireless_uart_send_buff (text, sizeof(text) - 1);

//}


//void Oscilloscope_Gryo()
//{
//	sprintf(text, ",%d,%d,%d,%d\r\n", imu963ra_gyro_x, imu963ra_gyro_y, imu963ra_gyro_z,angle_accu);

//	wireless_uart_send_buff (text, sizeof(text) - 1);

//}


//void Screen_All()
//{
//	switch (screenItem)
//	{
//		case 0:
//			adcScreen();
//			//wireless_adcScreen();
//			if (!P70)
//			{
//				screenItem = 1;
//				paramChoice = 0;
//				ips114_clear (WHITE);
//				while (!P70);
//			}
//			break;
//		case 1:
//			preScreen();
//			//wireless_guiyiScreen();
//			if (!P70)
//			{
//				screenItem = 2;
//				paramChoice = 0;
//				ips114_clear (WHITE);
//				while (!P70);
//			}
//			break;
//		case 2:
//			pidScreen();
//			//wireless_pidScreen();
//			if (!P70)
//			{
//				screenItem = 3;
//				paramChoice = 0;
//				ips114_clear (WHITE);
//				while (!P70);
//			}
//			break;
//		case 3:
//			gyroScreen();
//			if (!P70)
//			{
//				screenItem = 0;
//				paramChoice = 0;
//				ips114_clear (WHITE);
//				while (!P70);
//			}
//			break;
//		default:
//			break;
//	}
//}



//void adcScreen()
//{
//	switch (paramChoice)
//	{
//		case 0:
//			//SHOW
//			sprintf (text, "pre0 %04d    gy0 %03d", adc_pre[0], adc_guiyi[0]);
//			ips114_showstr (0, 0, text);             
//			sprintf (text, "pre1 %04d    gy1 %03d", adc_pre[1], adc_guiyi[1]);
//			ips114_showstr (0, 1, text);                
//			sprintf (text, "pre2 %04d    gy2 %03d", adc_pre[2], adc_guiyi[2]);
//			ips114_showstr (0, 2, text);
//			sprintf (text, "pre3 %04d    gy3 %03d", adc_pre[3], adc_guiyi[3]);
//			ips114_showstr (0, 3, text);
//			sprintf (text, "pre4 %04d    gy4 %03d", adc_pre[4], adc_guiyi[4]);
//			ips114_showstr (0, 4, text);
//			sprintf (text, "pre5 %04d    gy5 %03d", adc_pre[5], adc_guiyi[5]);
//			ips114_showstr (0, 5, text);
//			
//			sprintf (text, "setSpeed:%d", setSpeed);
//			ips114_showstr (0, 6, text);
//			sprintf (text, "currItem:%d", currItem);
//			ips114_showstr (0, 7, text);
//		
//			if (!P73)
//			{
//				paramChoice = 1;
//				ips114_clear (WHITE);
//				while (!P73);
//			}
//			if (!P71)
//			{
//				setSpeed -= 1;
//				while (!P71);
//			}
//			if (!P72)
//			{
//				setSpeed += 1;
//				while (!P72);
//			}
//			break;
//		
//		case 1:
//			//SHOW
//			sprintf(text, "encoderR:%d  ", encoderR);
//			ips114_showstr(0, 1, text);
//			sprintf(text, "encoderL:%d  ", encoderL);
//			ips114_showstr(0, 2, text);
//			
//			sprintf (text, "setSpeed:%d", setSpeed);
//			ips114_showstr (0, 4, text);
//		
//			if (!P73)
//			{
//				paramChoice = 0;
//				ips114_clear (WHITE);
//				while (!P73);
//			}
//			break;
//			
//			
//		default:
//			break;
//	}
//}

//void preScreen()
//{
//	switch (paramChoice)
//	{
//		case 0:
//			//SHOW
//			sprintf(text, "adc_min[0]:%d  ", adc_min[0]);
//			ips114_showstr(0, 0, text);
//			sprintf(text, "adc_min[1]:%d  ", adc_min[1]);
//			ips114_showstr(0, 1, text);
//			sprintf(text, "adc_min[2]:%d  ", adc_min[2]);
//			ips114_showstr(0, 2, text);
//			sprintf(text, "adc_min[3]:%d  ", adc_min[3]);
//			ips114_showstr(0, 3, text);
//			sprintf(text, "adc_min[4]:%d  ", adc_min[4]);
//			ips114_showstr(0, 4, text);
//			sprintf(text, "adc_min[5]:%d  ", adc_min[5]);
//			ips114_showstr(0, 5, text);
//		
//			if (!P73)
//			{
//				paramChoice = 1;
//				ips114_clear (WHITE);
//				while (!P73);
//			}

//			break;
//			
//		case 1:
//			//SHOW
//			sprintf(text, "adc_max[0]:%d  ", adc_max[0]);
//			ips114_showstr(0, 0, text);
//			sprintf(text, "adc_max[1]:%d  ", adc_max[1]);
//			ips114_showstr(0, 1, text);
//			sprintf(text, "adc_max[2]:%d  ", adc_max[2]);
//			ips114_showstr(0, 2, text);
//			sprintf(text, "adc_max[3]:%d  ", adc_max[3]);
//			ips114_showstr(0, 3, text);
//			sprintf(text, "adc_max[4]:%d  ", adc_max[4]);
//			ips114_showstr(0, 4, text);
//			sprintf(text, "adc_max[5]:%d  ", adc_max[5]);
//			ips114_showstr(0, 5, text);
//		
//			if (!P73)
//			{
//				paramChoice = 0;
//				ips114_clear (WHITE);
//				while (!P73);
//			}
//			break;
//	}
//}

//void pidScreen()
//{
//	switch (paramChoice)
//	{
//		case 0: // 调节 adc_kp1
//			sprintf (text, "adcError:%f  ", adcError );
//			ips114_showstr (0, 0, text);
//			sprintf (text, "adcIntegral:%f  ", adcIntegral );
//			ips114_showstr (0, 1, text);
//			sprintf (text, "adcDerivative:%f  ", adcDerivative );
//			ips114_showstr (0, 2, text);
//			sprintf (text, "adc_actual:%f  ", adc_actual );
//			ips114_showstr (0, 3, text);
//			sprintf (text, "adc_kp1:%f", adc_kp1 );
//			ips114_showstr (0, 5, text);
//		
//			if (!P73) { paramChoice = 1; ips114_clear(WHITE); while (!P73); }
//            if (!P71) { adc_kp1 -= 1; while (!P71); }
//            if (!P72) { adc_kp1 += 1; while (!P72); }
//			break;
//			
//		case 1: // 调节 adc_ki
//			sprintf (text, "adcError:%f  ", adcError );
//			ips114_showstr (0, 0, text);
//			sprintf (text, "adcIntegral:%f  ", adcIntegral );
//			ips114_showstr (0, 1, text);
//			sprintf (text, "adcDerivative:%f  ", adcDerivative );
//			ips114_showstr (0, 2, text);
//			sprintf (text, "adc_actual:%f  ", adc_actual );
//			ips114_showstr (0, 3, text);
//			sprintf (text, "adc_ki:%f", adc_ki );
//			ips114_showstr (0, 5, text);
//		
//			if (!P73) { paramChoice = 2; ips114_clear(WHITE); while (!P73); }
//            if (!P71) { adc_ki -= 0.1; while (!P71); }
//            if (!P72) { adc_ki += 0.1; while (!P72); }
//			break;
//			
//		case 2: // 调节 adc_kd
//			sprintf (text, "adcError:%f  ", adcError );
//			ips114_showstr (0, 0, text);
//			sprintf (text, "adcIntegral:%f  ", adcIntegral );
//			ips114_showstr (0, 1, text);
//			sprintf (text, "adcDerivative:%f  ", adcDerivative );
//			ips114_showstr (0, 2, text);
//			sprintf (text, "adc_actual:%f  ", adc_actual );
//			ips114_showstr (0, 3, text);
//			sprintf (text, "adc_kd:%f  x50", adc_kd );
//			ips114_showstr (0, 5, text);
//		
//			if (!P73) { paramChoice = 3; ips114_clear(WHITE); while (!P73); }
//            if (!P71) { adc_kd -= 50; while (!P71); }
//            if (!P72) { adc_kd += 50; while (!P72); }
//			break;
//			
//		case 3: // 调节 adc_kp2
//			sprintf (text, "adcError:%f  ", adcError );
//			ips114_showstr (0, 0, text);
//			sprintf (text, "adcIntegral:%f  ", adcIntegral );
//			ips114_showstr (0, 1, text);
//			sprintf (text, "adcDerivative:%f  ", adcDerivative );
//			ips114_showstr (0, 2, text);
//			sprintf (text, "adc_actual:%f  ", adc_actual );
//			ips114_showstr (0, 3, text);
//			sprintf (text, "adc_kp2:%f  x0.1", adc_kp2 );
//			ips114_showstr (0, 5, text);
//		
//			if (!P73) { paramChoice = 0; ips114_clear(WHITE); while (!P73); }
//            if (!P71) { adc_kp2 -= 0.1; while (!P71); }
//            if (!P72) { adc_kp2 += 0.1; while (!P72); }
//			break;	
//	}
//}

//void gyroScreen()
//{
//	switch (paramChoice)
//	{
//		case 0: // 显示陀螺仪数据
//            ips114_showstr(0, 0, "gyro.x=");
//            ips114_showstr(0, 1, "gyro.y=");
//            ips114_showstr(0, 2, "gyro.z=");
//			ips114_showstr(0, 3, "angle_accu=");
//            ips114_showint16(6*10, 0, imu963ra_gyro_x);
//            ips114_showint16(6*10, 1, imu963ra_gyro_y);
//            ips114_showint16(6*10, 2, imu963ra_gyro_z);
//			ips114_showint16(90, 3, angle_accu);
//		
//			if (!P73) { paramChoice = 1; ips114_clear(WHITE); while (!P73); }
//            break;
//			
//		case 1: // 显示加速度计数据
//			ips114_showstr(0, 0, "acc.x=");
//            ips114_showstr(0, 1, "acc.y=");
//            ips114_showstr(0, 2, "acc.z=");
//            ips114_showint16(6*10, 0, imu963ra_acc_x);
//            ips114_showint16(6*10, 1, imu963ra_acc_y);
//            ips114_showint16(6*10, 2, imu963ra_acc_z);
//			ips114_showstr(0, 3, "accelX=");
//            ips114_showstr(0, 4, "accelY=");
//            ips114_showstr(0, 5, "accelZ=");
//            ips114_showint16(6*10, 3, accelX);
//            ips114_showint16(6*10, 4, accelY);
//            ips114_showint16(6*10, 5, accelZ);			
//			
//			if (!P73) { paramChoice = 0; ips114_clear(WHITE); while (!P73); }
//            break;
//	}
//}

//void wireless_pidScreen()
//{
//	sprintf (text, "adcError:%d  adcIntegral:%d  \n", adcError, adcIntegral);
//	wireless_uart_send_buff (text, sizeof (text) - 1);

//	sprintf (text, "adcDerivative:%d  adc_actual:%d  \n", adcDerivative, adc_actual);
//	wireless_uart_send_buff (text, sizeof (text) - 1);

//	sprintf (text, "adc_kp1:%d  change:adc_kd:%d  \n", Z_change (adc_kp1), Z_change (adc_kd) );
//	wireless_uart_send_buff (text, sizeof (text) - 1);
//}

//void wireless_guiyiScreen()
//{
//	sprintf (text, "adc_guiyi[0]:%d  adc_guiyi[1]:%d  \n", adc_guiyi[0], adc_guiyi[1]);
//	wireless_uart_send_buff (text, sizeof (text) - 1);

//	sprintf (text, "adc_guiyi[2]:%d  adc_guiyi[3]:%d  \n", adc_guiyi[2], adc_guiyi[3]);
//	wireless_uart_send_buff (text, sizeof (text) - 1);

//	sprintf (text, "adc_guiyi[4]:%d  adc_guiyi[5]:%d  \n", adc_guiyi[4], adc_guiyi[5]);
//	wireless_uart_send_buff (text, sizeof (text) - 1);

//	sprintf (text, "change:adc_kp1:%d  \n", Z_change (adc_kp1) );
//	wireless_uart_send_buff (text, sizeof (text) - 1);
//}

//void wireless_adcScreen()
//{
//	sprintf (text, "Rputout:%d   \n", Rputout);
//	wireless_uart_send_buff (text, sizeof (text) - 1);

//	sprintf (text, "Lputout:%d   \n", Lputout);
//	wireless_uart_send_buff (text, sizeof (text) - 1);


//	sprintf (text, "encoderR:%d   \n", encoderR);
//	wireless_uart_send_buff (text, sizeof (text) - 1);

//	sprintf (text, "encoderL:%d   \n", encoderL);
//	wireless_uart_send_buff (text, sizeof (text) - 1);


//	sprintf (text, "  \n", setSpeed);
//	wireless_uart_send_buff (text, sizeof (text) - 1);
//}

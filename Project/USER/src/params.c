#include "headfile.h"

// ============================================================================
//                          ������״̬�л�
// ============================================================================

/**
 * @brief  ���ݲ��뿪��(P75, P76)��״̬���л�С���Ĺ���ģʽ
 * @param  None
 * @return void
 * @note   �˺����Ƕ���Ĺ���ģʽ�ַ�����
 *         - P76 P75 = 00: �޲���
 *         - P76 P75 = 01: ����ʾ����ģʽ (����λ������)
 *         - P76 P75 = 10: ����ʾ����ģʽ (���͵������)
 *         - P76 P75 = 11: ��ĻUI��ʾģʽ
 * @author yanyyy
 * @date   2025-09-19
 */
void WorkStatusSwitch()
{
    if (!P75 && !P76)
    {
        // ģʽ 00: �������޲���
    }
    else if (P75 && !P76)
    {
        // ģʽ 01: ����λ��������ݵ���λ��
        Oscilloscope_Position();
    }
    else if (!P75 && P76)
    {
        // ģʽ 10: ���͵��������ݵ���λ��
        Oscilloscope_Motor();
    }
    else
    {
        // ģʽ 11: ������ĻUI��ʾ���������
        Screen_All();
    }
    
    /*
     * // --- ����Ϊһ�ֱ�ѡ�ġ������ӵ�ʾ�����л��߼� ---
     * // ͨ������ P70 �ڲ�ͬ��ʾ����������л�
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
     *         // ... ���� case ...
     *     }
     * }
     * else
     * {
     *     Screen_All();
     * }
     */
}


// ============================================================================
//                          ����ʾ�������ݷ���
// ============================================================================

/**
 * @brief  ͨ�����ߴ��ڷ��͵���������
 * @param  None
 * @return void
 * @note   ���͸�ʽΪ ",Ŀ���ٶ�,�������ֵ,�ұ�����ֵ\r\n"��
 *         ������λ�����ʵʱ��ͼ��
 * @author yanyyy
 * @date   2025-09-19
 */
void Oscilloscope_Motor()
{
    sprintf(text, ",%d,%d,%d\r\n", setSpeed, encoderL, encoderR);
    wireless_uart_send_buff(text, sizeof(text) - 1);
}

/**
 * @brief  ͨ�����ߴ��ڷ���λ������̬��������
 * @param  None
 * @return void
 * @note   ���͸�ʽΪ ",ѭ��ƫ��,����,Z��Ǽ��ٶ�\r\n"��
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
 * @brief  ͨ�����ߴ��ڷ���������ԭʼ���ݺ��ۼƽǶ�
 * @param  None
 * @return void
 * @note   ���͸�ʽΪ ",gyro_x,gyro_y,gyro_z,�ۼƽǶ�\r\n"��
 * @author yanyyy
 * @date   2025-09-19
 */
void Oscilloscope_Gryo()
{
    sprintf(text, ",%d,%d,%d,%d\r\n", imu963ra_gyro_x, imu963ra_gyro_y, imu963ra_gyro_z, angle_accu);
    wireless_uart_send_buff(text, sizeof(text) - 1);
}


// ============================================================================
//                          ��ĻUI��ʾ���������
// ============================================================================

/**
 * @brief  ��ĻUI��״̬���������ڲ�ͬ��ʾ������л�
 * @param  None
 * @return void
 * @note   ʹ��ȫ�ֱ��� `screenItem` ��Ϊ״̬��
 *         ���� P70 �����л�����һ�����档
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
 * @brief  ��ʾADC���ٶȺͱ��������ݵĽ���
 * @param  None
 * @return void
 * @note   ʹ�� `paramChoice` �л��ӽ��档
 *         - P73: ��'���'��'������'�ӽ�����л���
 *         - P71/P72: ��'���'�ӽ����£����Լ�/�� `setSpeed`��
 * @author yanyyy
 * @date   2025-09-19
 */
void adcScreen()
{
    switch (paramChoice)
    {
        case 0: // ��ʾ���ԭʼֵ�͹�һ��ֵ
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

            if (!P73) { paramChoice = 1; ips114_clear(WHITE); while (!P73); } // �л�����һ����ҳ
            if (!P71) { setSpeed -= 1; while (!P71); }                        // ����
            if (!P72) { setSpeed += 1; while (!P72); }                        // ����
            break;

        case 1: // ��ʾ������ֵ
            sprintf(text, "encoderR:%d  ", encoderR);
            ips114_showstr(0, 1, text);
            sprintf(text, "encoderL:%d  ", encoderL);
            ips114_showstr(0, 2, text);
            sprintf(text, "setSpeed:%d", setSpeed);
            ips114_showstr(0, 4, text);

            if (!P73) { paramChoice = 0; ips114_clear(WHITE); while (!P73); } // �л�����һ����ҳ
            break;

        default:
            break;
    }
}

/**
 * @brief  ��ʾ���������/��С�궨ֵ�Ľ���
 * @param  None
 * @return void
 * @note   ʹ�� `paramChoice` ����Сֵ�����ֵ��ʾҳ֮���л���
 *         - P73: �� `adc_min` �� `adc_max` ��ʾ֮���л���
 * @author yanyyy
 * @date   2025-09-19
 */
void preScreen()
{
    switch (paramChoice)
    {
        case 0: // ��ʾ��Сֵ
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

        case 1: // ��ʾ���ֵ
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
 * @brief  ��ʾ������ѭ��PID�����Ľ���
 * @param  None
 * @return void
 * @note   ʹ�� `paramChoice` �л�Ҫ���ڵ�PID���� (kp1, ki, kd, kp2)��
 *         - P73: ѭ���л�Ҫ���ڵĲ�����
 *         - P71/P72: ��/�ӵ�ǰѡ�еĲ���ֵ��
 * @author yanyyy
 * @date   2025-09-19
 */
void pidScreen()
{
	switch (paramChoice)
	{
		case 0: // ���� adc_kp1
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
			
		case 1: // ���� adc_ki
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
			
		case 2: // ���� adc_kd
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
			
		case 3: // ���� adc_kp2
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
 * @brief  ��ʾ�����Ǻͼ��ٶȼ����ݵĽ���
 * @param  None
 * @return void
 * @note   ʹ�� `paramChoice` �������Ǻͼ��ٶȼ���ʾҳ֮���л���
 *         - P73: ����ҳ֮���л���
 * @author yanyyy
 * @date   2025-09-19
 */
void gyroScreen()
{
	switch (paramChoice)
	{
		case 0: // ��ʾ����������
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
			
		case 1: // ��ʾ���ٶȼ�����
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
//                       �������ݷ��� (��Ļ�������)
// ============================================================================

/**
 * @brief  ͨ�����ߴ��ڷ���PID�������
 * @param  None
 * @return void
 * @note   �� `pidScreen` ����������汾��
 *         ���� `Z_change()` ������
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
 * @brief  ͨ�����ߴ��ڷ��͹�һ����ĵ������
 * @param  None
 * @return void
 * @note   ���� `Z_change()` ������
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
 * @brief  ͨ�����ߴ��ڷ��͵���ͱ���������
 * @param  None
 * @return void
 * @note   �� `adcScreen` ���ֹ��ܵ���������汾��
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
//		case 0: // ���� adc_kp1
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
//		case 1: // ���� adc_ki
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
//		case 2: // ���� adc_kd
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
//		case 3: // ���� adc_kp2
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
//		case 0: // ��ʾ����������
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
//		case 1: // ��ʾ���ٶȼ�����
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

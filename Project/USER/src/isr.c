///*********************************************************************************************************************
// * COPYRIGHT NOTICE
// * Copyright (c) 2020,��ɿƼ�
// * All rights reserved.
// * ��������QQȺ��һȺ��179029047(����)  ��Ⱥ��244861897(����)  ��Ⱥ��824575535
// *
// * �����������ݰ�Ȩ������ɿƼ����У�δ��������������ҵ��;��
// * ��ӭ��λʹ�ò������������޸�����ʱ���뱣����ɿƼ��İ�Ȩ������
// *
// * @file       		isr
// * @company	   		�ɶ���ɿƼ����޹�˾
// * @author     		��ɿƼ�(QQ790875685)
// * @version    		�鿴doc��version�ļ� �汾˵��
// * @Software 			MDK FOR C251 V5.60
// * @Target core		STC32G12K128
// * @Taobao   			https://seekfree.taobao.com/
// * @date       		2020-4-14
// ********************************************************************************************************************/
#include "headfile.h"
#include "globals_ex.h"

//UART1�ж�
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
        //�����Զ�����
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

//UART2�ж�
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
		//�������ݼĴ���Ϊ��S2BUF
		gps_uart_callback(S2BUF);
	}
}


//UART3�ж�
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
		//�������ݼĴ���Ϊ��S3BUF

	}
}


//UART4�ж�
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

		//�������ݼĴ���Ϊ��S4BUF;
		if(wireless_module_uart_handler != NULL)
		{
			// �ú���Ϊ����ָ��
			// �ٳ�ʼ������ģ���ʱ�����øú�����ַ
			wireless_module_uart_handler(S4BUF);
		}
	}
}

#define LED P52
void INT0_Isr() interrupt 0
{
	LED = 0;	//����LED
}
void INT1_Isr() interrupt 2
{

}
void INT2_Isr() interrupt 10
{
	INT2_CLEAR_FLAG;  //����жϱ�־
}
void INT3_Isr() interrupt 11
{
	INT3_CLEAR_FLAG;  //����жϱ�־
}

void INT4_Isr() interrupt 16
{
	INT4_CLEAR_FLAG;  //����жϱ�־
}

void TM0_Isr() interrupt 1
{

}

/**
 * @brief  Timer1 �жϷ������ (��Ƶ���ݲɼ�)
 * @param  None
 * @return void
 * @note   ���жϸ���ִ����������
 *         1. ��ȡ���ҵ���ı�����ֵ��
 *         2. �������˲���е�ADC������
 *         3. �˲����������ݡ�
 *         4. �� `distance_set` �� `angle_set` ��Ϊ��ʱ���ۼ���ʻ�����ת��Ƕȡ�
 * @author yanyyy
 * @date   2025-09-19
 */
void TM1_Isr() interrupt 3
{

	// �������;

//	    dl1a_get_distance();
//	    if (dl1a_distance_mm > 8000)
//	    {
//	        dl1a_distance_mm = 8000;
//	    }

	// ��������ȡ������
	// ���
	encoderL = (P35 == 1) ? (ctimer_count_read (CTIM0_P34) ) : (ctimer_count_read (CTIM0_P34) * -1);
	ctimer_count_clean (CTIM0_P34);

	// �Ҳ�
	encoderR = (P53 == 1) ? (ctimer_count_read (CTIM3_P04) * -1) : (ctimer_count_read (CTIM3_P04) );
	ctimer_count_clean (CTIM3_P04);


//	// ��������ȡ��������
//    // ���
//    encoderL = ctimer_count_read(CTIM0_P34);
//    ctimer_count_clean(CTIM0_P34);
//
//    // �Ҳ�
//    encoderR = ctimer_count_read(CTIM3_P04);
//    ctimer_count_clean(CTIM3_P04);



	//��в�������ֵ�˲�
//	AD_Date_Fitier();
	
//	//��в���������ƽ���˲�
//	sample_and_filter_all_channels();
	
	//��в�����һ�׵�ͨ�˲�
	inductor_low_pass_filter();

	// �����ۼ�
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
		//�����ǲ�����һ�׵�ͨ�˲� + ��С
		gyro_low_pass_filter();
	
		//���ټƲ�����һ�׵�ͨ�˲�
		//acc_low_pass_filter();
	
//	}
//	else
//	{
//		imu963ra_get_gyro();
//		//imu963ra_get_acc();
//	}


	// �Ƕ��ۼ�
	if (angle_set != 0)
	{
		//gyroY = imu963ra_gyro_transition(gyro_y_raw);//������ٶ� ��λΪ��/s
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
	TIM2_CLEAR_FLAG;  //����жϱ�־
	
}
void TM3_Isr() interrupt 19
{
	TIM3_CLEAR_FLAG; //����жϱ�־
	
}

/**
 * @brief  Timer4 �жϷ������ (�������߼�)
 * @param  None
 * @return void
 * @note   ���ж��Խϵ�Ƶ�����У�
 *         ִ�������� "��֪-����-ִ��" ѭ����
 *         1. ��֪: �Բɼ�����ԭʼ���ݽ��й�һ������
 *         2. ����: ���� `RoadItemsManager` �ж�����Ԫ�أ���ͨ�� `switch` ���ѡ��ǰ���˶����ԡ�
 *         3. ִ��: ���ݲ��Լ������ `setSpeed` �� `fix`������ `putOut` �������������
 *         4. ����: ����һ���ѹ챣�����ƣ����źŶ�ʧʱֹͣ�����
 * @author yanyyy
 * @date   2025-09-19
 */
void TM4_Isr() interrupt 20
{
	TIM4_CLEAR_FLAG; //����жϱ�־
	//ccd_collect();	 //CCD�ɼ�����
	
	// --- 1. ��֪: ���ݴ��� ---
	normalize_data();
	adc_sum = adc_guiyi[1] + adc_guiyi[2] + adc_guiyi[3] + adc_guiyi[4] + adc_guiyi[5];
	
	 // --- 2. ����: ״̬�ж������ѡ�� ---
	
	RoadItemsManager(); // ���ݴ�����״̬�����µ�ǰ����Ԫ�� `currItem`
	
	// ���ݵ�ǰ����Ԫ�أ�ѡ���Ӧ���˶����Ժ���
	switch (currItem)   // ��ͬ�ķ�ʽ����fix
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

	//�ѹ챣��
	//if ((adc_pre[0]+adc_pre[1]+adc_pre[2]+adc_pre[3]+adc_pre[4]+adc_pre[5] < 200) && (block_FLAG == 0))
	
	// --- 3. ִ�� & 4. ���� ---
    // �ѹ챣����������е�е��ź��ܺ͹��ͣ�����Ϊ���ѹ죬ֹͣ���
	if (adc_pre[1] + adc_pre[2] + adc_pre[3] + adc_pre[4] + adc_pre[5] < 300)
	{
		motorPWMOut (LEFT, 0);
		motorPWMOut (RIGHT, 0);
	}

	else // ��������£�ִ���ٶȱջ�����
	{
        // setSpeed �ǻ����ٶȣ�fix �Ƿ���PID������Ĳ�������ֵ
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
#include "headfile.h"

// 宏定义区域
// #define FILTER_N 4       // 滤波深度
// #define WINDOW_SIZE 4    // 窗口大小，建议设置为2的整数次幂
#define ADC_CHANNELS 6       // ADC通道数量


/**
 * @brief  对各路电磁传感器的ADC采样值进行一阶低通滤波
 * @param  None
 * @return void
 * @note   此函数直接读取ADC硬件端口并更新全局数组`adc_pre`。
 *         滤波系数为 0.3，即新采样值权重占30%，历史值权重占70%。
 * @author yanyyy
 * @date   2025-09-19
 */
void inductor_low_pass_filter()
{
    // 对每个传感器通道进行滤波处理
    adc_pre[1] = (adc_pre[1] * 7 + adc_once(ADC_P00, ADC_12BIT) * 3) / 10;
    // adc_pre[0] = (adc_pre[0] * 7 + adc_once(ADC_P15, ADC_12BIT) * 3) / 10; // 此通道被注释
    adc_pre[2] = (adc_pre[2] * 7 + adc_once(ADC_P01, ADC_12BIT) * 3) / 10;
    adc_pre[3] = (adc_pre[3] * 7 + adc_once(ADC_P13, ADC_12BIT) * 3) / 10;
    adc_pre[4] = (adc_pre[4] * 7 + adc_once(ADC_P14, ADC_12BIT) * 3) / 10;
    adc_pre[5] = (adc_pre[5] * 7 + adc_once(ADC_P16, ADC_12BIT) * 3) / 10;
}


/**
 * @brief  对滤波后的传感器数据进行归一化处理
 * @param  None
 * @return void
 * @note   该函数将 `adc_pre` 数组中的值，根据预设的 `adc_max` 和 `adc_min`
 *         线性映射到 0-100 的范围，并存入 `adc_guiyi` 数组。
 *         请确保 `adc_max` 和 `adc_min` 数组在使用前已经过正确的校准。
 * @author yanyyy
 * @date   2025-09-19
 */
void normalize_data()
{
    uint8 i;
    uint32 current_adc, max_val, min_val;

    // 遍历所有传感器通道进行归一化处理 (注意循环从1开始)
    for (i = 1; i < ADC_CHANNELS; i++)
    {
        current_adc = adc_pre[i];
        max_val = adc_max[i];     // 手动标定的该通道最大值
        min_val = adc_min[i];     // 手动标定的该通道最小值

        // 限制当前值在最大最小范围内
        if (current_adc < min_val)
        {
            current_adc = min_val;
        }
        else if (current_adc > max_val)
        {
            current_adc = max_val;
        }

        // 归一化公式，将当前值映射到 0-100
        // 为防止除零错误，应确保 max_val > min_val
        if (max_val > min_val)
        {
            adc_guiyi[i] = (current_adc - min_val) * 100 / (max_val - min_val);
        }
        else
        {
            adc_guiyi[i] = 0; // 如果最大最小值相等，则归一化值为0
        }
        
        /*
         * // 以下为自动刷新最大最小值的代码，通常在标定阶段使用
         * if(adc_pre[i] < adc_min[i])
         * {
         *     adc_min[i] = adc_pre[i]; // 刷新最小值
         * }
         * if(adc_pre[i] > adc_max[i])
         * {
         *     adc_max[i] = adc_pre[i]; // 刷新最大值
         * }
         */
    }
}


//void normalize_data() //电感归一化
//{
//	uint8 i;

//	for (i = 0; i < ADC_CHANNELS; i++)            //归一化处理
//	{
//		if (adc_pre[i] < adc_min[i])
//		{
//			adc_pre[i] = adc_min[i];
//		}
//		else if (adc_pre[i] > adc_max[i])
//		{
//			adc_pre[i] = adc_max[i];
//		}
//		
////		if(adc_pre[i]<adc_min[i])
////			adc_min[i]=adc_pre[i];//刷新最小值
////		if(adc_pre[i]>adc_max[i])
////			adc_max[i]=adc_pre[i];//刷新最大值

//		adc_guiyi[i] = (adc_pre[i] - adc_min[i]) * 100  / (adc_max[i] - adc_min[i]); 		//归一化处理
//	}

//}

//void adc_filter_init()
//{
//	uint8 ch, i;  // 循环变量

//	for (ch = 0; ch < ADC_CHANNELS; ch++)
//	{
//		adc_index_user[ch] = 0;
//		adc_total_user[ch] = 0;
//		for (i = 0; i < WINDOW_SIZE; i++)
//		{
//			adc_buffer_user[ch][i] = 0;
//		}
//	}
//}


//// 滑动平均滤波函数
//uint16 adc_sliding_filter (uint8 channel, uint16 new_sample)
//{
////    if (channel >= ADC_CHANNELS) {
////        return 0;  // 错误处理：通道号超出范围
////    }

//	// 减去最旧的值
//	adc_total_user[channel] -= adc_buffer_user[channel][adc_index_user[channel]];

//	// 加上最新的值
//	adc_total_user[channel] += new_sample;

//	// 存储新值到缓冲区
//	adc_buffer_user[channel][adc_index_user[channel]] = new_sample;

//	// 更新索引（环形缓冲区）
//	adc_index_user[channel] = (adc_index_user[channel] + 1) % WINDOW_SIZE;
//	//adc_index_user[channel] = (adc_index_user[channel] + 1) & 0x03;  // 等价于 % 4

//	// 返回平均值（整数除法）
//	return (uint16) (adc_total_user[channel] / WINDOW_SIZE);
//	//return (uint16)(adc_total_user[channel] >> 2);  // 等价于 / 4
//}


//void sample_and_filter_all_channels()
//{

////	uint16 raw_value0 = adc_mean_filter (ADC_P00, ADC_12BIT, 1);
////	uint16 raw_value1 = adc_mean_filter (ADC_P01, ADC_12BIT, 1);
////	uint16 raw_value2 = adc_mean_filter (ADC_P05, ADC_12BIT, 1);
////	uint16 raw_value3 = adc_mean_filter (ADC_P13, ADC_12BIT, 1);
////	uint16 raw_value4 = adc_mean_filter (ADC_P14, ADC_12BIT, 1);
////	uint16 raw_value5 = adc_mean_filter (ADC_P16, ADC_12BIT, 1);


//	//硬件画错了，改引脚，1和0互换

//	uint16 raw_value1 = adc_once (ADC_P00, ADC_12BIT);//
//	uint16 raw_value0 = adc_once (ADC_P01, ADC_12BIT);//
//	uint16 raw_value2 = adc_once (ADC_P05, ADC_12BIT);
//	uint16 raw_value3 = adc_once (ADC_P13, ADC_12BIT);
//	uint16 raw_value4 = adc_once (ADC_P14, ADC_12BIT);
//	uint16 raw_value5 = adc_once (ADC_P16, ADC_12BIT);

//	adc_pre[0] = adc_sliding_filter (0, raw_value0);
//	adc_pre[1] = adc_sliding_filter (1, raw_value1);
//	adc_pre[2] = adc_sliding_filter (2, raw_value2);
//	adc_pre[3] = adc_sliding_filter (3, raw_value3);
//	adc_pre[4] = adc_sliding_filter (4, raw_value4);
//	adc_pre[5] = adc_sliding_filter (5, raw_value5);

//}








//void normalize_date() //电感归一化
//{
//	float JSADC_DATE[6];//这个数据类型必须是有符号的  用于计算储存
//	unsigned char  i;
//	float b, c, d;

//	for (i = 0; i < 6; i++)            //归一化处理
//	{
//		b = adc_pre[i];
//		c = adc_max[i];          //手动获取
//		d = adc_min[i];	 		 //手动获取

//		//		if(adc_pre[i]<adc_min[i])
//		//			adc_min[i]=adc_pre[i];//刷新最小值
//		//		if(adc_pre[i]>adc_max[i])
//		//			adc_max[i]=adc_pre[i];//刷新最大值

//		JSADC_DATE[i] = ( (b - d) / (c - d) ) * 100.0; 		//归一化处理 将得到的数据全部归类到0-1之间 并且乘以100

//		if (JSADC_DATE[i] <= 0)
//		{
//			JSADC_DATE[i] = 0.0;
//		}

//		else if (JSADC_DATE[i] >= 100.0)
//		{
//			JSADC_DATE[i] = 100.0;
//		}


//		adc_guiyi[i] = Z_change (JSADC_DATE[i]);
//		//当最大值或者最小值超过设定的范围之内的时候 避免误差
//	}
//}






//int16 I_Median_Average_Filter(int16 *ADC) //去掉最大值和最小值（传进来的是数组，存储的是电感值）
//{
//    uint8 i;
//    int16 max,min;  //定义极值以及和
//    int16 sum = 0;

//    max = ADC[0]; //初始化最小值和最大值
//    min = ADC[0];

//    for(i=0;i<sizeof(ADC);i++) // sizeof(ADC)是传进来的电感数组的长度
//    {
//        if(max<ADC[i])max = ADC[i];  //找出最大的电感
//        if(min>ADC[i])min = ADC[i];  //找出最小的电感
//        sum += ADC[i];  //累加
//    }
//
//    sum =(sum-max-min)/(sizeof(ADC)-2);    //电感的累加值减去最大值和最小值 再求平均
//    return sum; //将去极值的值传回原函数
//}



//float AD_Date_Fitier()
//{
////    uint8 i;
////    int16 filter_L1[FILTER_N]; //存储电感的数组
////	int16 filter_L2[FILTER_N];
////	int16 filter_L3[FILTER_N];
////	int16 filter_R3[FILTER_N];
////	int16 filter_R2[FILTER_N];
////	int16 filter_R1[FILTER_N];
////	//int16 filter_L[FILTER_N];
////	//int16 filter_R[FILTER_N];
////
//////--------伪均值滤波--------------
////	for(i = 0; i <FILTER_N; i++)//采值
////	{
////        filter_L1[i]  =  adc_mean_filter  (ADC_P00, ADC_12BIT, 1); //将1个均值滤波后的电感传入电感数组
////      filter_L2[i]  =  adc_mean_filter  (ADC_P01, ADC_12BIT, 1);
////      filter_L3[i]  =  adc_mean_filter  (ADC_P05, ADC_12BIT, 1);
////		filter_R3[i]  =  adc_mean_filter  (ADC_P13, ADC_12BIT, 1);
////		filter_R2[i]  =  adc_mean_filter  (ADC_P14, ADC_12BIT, 1);
////		filter_R1[i]  =  adc_mean_filter  (ADC_P16, ADC_12BIT, 1);
////	    //filter_L[i]  =  adc_mean_filter  (ADC_P06, ADC_12BIT, 1);
////		//filter_R[i]  =  adc_mean_filter  (ADC_P17, ADC_12BIT, 1);

////    }
//////--------去极值求平均---------
////	adc_pre[0]= I_Median_Average_Filter(filter_L1);//电感最终值
////	adc_pre[1]= I_Median_Average_Filter(filter_L2);
////	adc_pre[2]= I_Median_Average_Filter(filter_L3);
////	adc_pre[3]= I_Median_Average_Filter(filter_R3);
////	adc_pre[4]= I_Median_Average_Filter(filter_R2);
////	adc_pre[5]= I_Median_Average_Filter(filter_R1);
////    //adc_pre[6]= I_Median_Average_Filter(filter_L);
////	//adc_pre[7]= I_Median_Average_Filter(filter_R);

////	return  0.0;







//	//--------zf库均值滤波，没有去掉极值--------------
//	adc_pre[0] = adc_mean_filter (ADC_P00, ADC_12BIT, FILTER_N); //电感最终值
//	adc_pre[1] = adc_mean_filter (ADC_P01, ADC_12BIT, FILTER_N);
//	adc_pre[2] = adc_mean_filter (ADC_P05, ADC_12BIT, FILTER_N);
//	adc_pre[3] = adc_mean_filter (ADC_P13, ADC_12BIT, FILTER_N);
//	adc_pre[4] = adc_mean_filter (ADC_P14, ADC_12BIT, FILTER_N);
//	adc_pre[5] = adc_mean_filter (ADC_P16, ADC_12BIT, FILTER_N);

//	return  0.0;

//}

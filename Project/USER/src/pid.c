#include "headfile.h"

float adc_actual_val = 0.0;
float Weight_HH = 0.0;
float Weight_VV = 0.0;

/**
 * @brief  根据中间电感的值计算水平电感的融合权重
 * @param  mmm: int, 中间电感 (adc_guiyi[5]) 的归一化值 (0-100)
 * @return float, 水平电感权重 (0.0 到 1.0)
 * @note   该函数实现了一个分段线性映射，用于在不同赛道情况下动态调整水平和垂直传感器的作用比例。
 * @author yanyyy
 * @date   2025-09-19
 */
float Calculate_Weight_Mid(int mmm)
{
	float k = 0;
	
	if(mmm >=90)
	{
		k = 100;
	}
	else if ( mmm >= 40)
	{
		k = 1.4 * mmm - 26; //宽电感
		//k = 1.75* mmm - 57.5;
	}
	else
	{
		k = 1* mmm - 10; //宽电感
		//k = 0.667* mmm - 3.34;
	}
	
	k *= 0.01;
	// 限幅在 [0.0, 1.0] 之间
	return (k >=1.0 ? 1.0 :(k <=0.0 ? 0.0 : k));
}

/*
 * @brief  计算垂直电感差异 (备用函数)
 * @return float, 归一化后的差异值
 *
 * float Calculate_Vertical()
 * {
 *     float k2 = 0;
 *     float sum = 200.0;
 *     k2 = fabs(a - b) / sum;
 *     return k2 >= 1.0 ? 1.0 : (k <= 0.1 ? 0.1 : k);
 * }
 */

/**
 * @brief  主要的电磁循迹 PID 控制计算 (方向环)
 * @param  None
 * @return float, 计算出的控制输出值 (adc_actual)
 * @note   此函数融合了水平/垂直电感差比和、陀螺仪角速度(Z)和角加速度(dZ/dt)。
 *         比例项采用 (Kp*e + Kp2*e^2) 的非线性形式。
 *         输出 `adc_actual` 被限幅在 [-80, 80]。
 *         该函数被标记为 `reentrant` (可重入)。
 * @author yanyyy
 * @date   2025-09-19
 */
float PID_ADC() reentrant //方向环
{
	int a = 0, b = 0, c = 0, d = 0, m = 0;
	
	// 读取归一化后的电感值
	a = adc_guiyi[2];
	b = adc_guiyi[3];
	c = adc_guiyi[1];
	d = adc_guiyi[4];
	m = adc_guiyi[5];
	
	// 计算权重 (中间电感越大，水平权重越大)
	Weight_HH = Calculate_Weight_Mid(m);
	Weight_VV = 1.0 - Weight_HH;
	
	// 计算循迹误差 (采用加权差比和)
	if ((adc_guiyi[1]+adc_guiyi[2]+adc_guiyi[3]+adc_guiyi[4]) > 0)
	{
		adcError = adc_target - ( (Weight_VV * (a - b) + Weight_HH * (c - d) ) * 1.0 / (Weight_HH * (c+d) + Weight_VV * abs (a-b) ) );
	}

	else
	{
		adcError = 0;
	}

	// 微分误差
	adcDerivative = adcError - prevAdcError;

	//保存
	prevAdcError = adcError;

	// 计算adcError的绝对值
	adcError_abs = (adcError >= 0) ? adcError : -adcError;

	// 计算比例项 + 二次比例项   先使用绝对值计算，后面再处理符号
	adc_actual_val = (adc_kp1 * adcError_abs) + (adc_kp2 *adcError_abs * adcError_abs);

	// 恢复符号
	if (adcError < 0)
	{
		adc_actual_val = -adc_actual_val;
	}
	
	 // 加上微分项 (D)
	adc_actual_val += (adc_kd * adcDerivative);
	
	
	// --- 陀螺仪修正 ---
	gyroZ_Derivative = gyroZ - gyroZ_before;
	gyroZ_before = gyroZ;

	// 加上陀螺仪角速度 (P)
	adc_actual_val += gyroZ * gyro_kp;
	
	// 当角加速度过大时，加上角加速度 (D) 修正
	if((gyroZ_Derivative > 4000) || (gyroZ_Derivative < -4000)) 
	{
		adc_actual_val += (gyroZ_Derivative * gyro_kd);
		//adc_actual_val += (gyro_kp * gyroZ);
		
	}
	
	
	// --- 最终限幅 ---
	adc_actual_val = (adc_actual_val > 80) ? 80 : (adc_actual_val < -80 ? -80 : adc_actual_val);

	adc_actual = adc_actual_val;
	
	return adc_actual;//返回ADC实际输出值
}








/**
 * @brief  备用的电磁循迹 PID 控制计算 (方向环)
 * @param  None
 * @return float, 计算出的控制输出值 (adc_actual)
 * @note   此函数使用简化的差比和计算 `(b-d)/(b+d)`. 包含积分项 (I)，
 *         但积分项仅在小误差范围内起作用。
 * @author yanyyy
 * @date   2025-09-19
 */
float PID_ADC_2() reentrant //方向环
{
	// 计算误差
	int b = 0, d = 0;
	b = adc_guiyi[3];
	d = adc_guiyi[4];

	//A = 0.3   B = 0.7  C = 0.7
	if (adc_guiyi[2] + adc_guiyi[3] + adc_guiyi[1] + adc_guiyi[4] != 0)
	{

		//差比和2号 yongyu                  A                B                        A               C
		adcError = adc_target - ((b-d)/(b+d));
		
		//限幅
		adcError = (adcError > 1.0) ? 1.0 : (adcError < -1.0 ? -1.0 : adcError);

	}

	else
	{
		adcError = 0;
	}

	// 微分误差
	adcDerivative = adcError - prevAdcError;

	//积分
	adcIntegral += adcError;
	if (adcIntegral >  50)
	{
		adcIntegral =  50;
	}
	if (adcIntegral < -50)
	{
		adcIntegral = -50;
	}


	//保存
	prevAdcError = adcError;

	// 计算adcError的绝对值
	adcError_abs = (adcError >= 0) ? adcError : -adcError;

	// 计算比例项 + 二次比例项   先使用绝对值计算，后面再处理符号
	adc_actual_val = (adc_kp1 * adcError_abs) + (adc_kp2 *adcError_abs * adcError_abs);

	// 如果原始误差为负，需要对计算结果取反
	if (adcError < 0)
	{
		adc_actual_val = -adc_actual_val;
	}

	// 加上微分项(D)和积分项(I)这两项不受误差符号影响，所以放在最后统一处理
	if (adcError < 0.4 && adcError > -0.4)
	{
		adc_actual_val += (adc_kd * adcDerivative) + (adc_ki * adcIntegral);
	}
	else
	{
		adc_actual_val += (adc_kd * adcDerivative);
	}


	//限幅
	//adc_actual_val = (adc_actual_val > 60) ? 60 : (adc_actual_val < -60 ? -60 : adc_actual_val);
	//adc_actual_val = (adc_actual_val > 90) ? 90 : (adc_actual_val < -90 ? -90 : adc_actual_val);
	if (adc_actual_val > 10) 
	{
		adc_actual_val = 10;       // 如果值大于90，强制设为90
	} 
	else if (adc_actual_val < -10) 
	{
		adc_actual_val = -10;      // 如果值小于-90，强制设为-90
	}

	adc_actual = adc_actual_val;
	
	return adc_actual;//返回ADC实际输出值
}

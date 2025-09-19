#include "headfile.h"

#define OFFSET_COUNT 50 // 定义用于计算零点漂移的采样次数

//  逐飞库：
//	imu963ra_get_gyro();    	// 获取陀螺仪数据
//	imu963ra_get_acc(); 		// 获取加速度数据
//	imu963ra_get_mag();			// 获取磁力计数据


/**
 * @brief  计算IMU传感器的零点漂移值
 * @param  None
 * @return void
 * @note   此函数需要在小车完全静止时调用，以确保校准的准确性。
 *         当前版本主要计算陀螺仪Z轴的零漂 `offset_gz`。
 *         加速度计和陀螺仪其他轴的校准代码已被注释。
 *         通过 `imu963ra_offset_flag = 1` 标志校准完成。
 * @author yanyyy
 * @date   2025-09-19
 */
void IMU_offset()
{
    int i;

    // --- 陀螺仪Z轴零漂校准 ---
    for (i = 0; i < OFFSET_COUNT; i++)
    {
        delay_ms(5);
        imu963ra_get_gyro(); // 获取陀螺仪数据

        // 简单的数据有效性检查，若x和y轴数据相同，可能为无效数据，则本次采样作废
        if (imu963ra_gyro_x == imu963ra_gyro_y)
        {
            i--;
        }
        else
        {
            // offset_gx += imu963ra_gyro_x;
            // offset_gy += imu963ra_gyro_y;
            offset_gz += imu963ra_gyro_z; // 累加Z轴数据
        }
    }
    // offset_gx /= OFFSET_COUNT;
    // offset_gy /= OFFSET_COUNT;
    offset_gz /= OFFSET_COUNT; // 求Z轴零漂平均值

    /*
     * // --- 加速度计零位校准 ---
     * // X轴
     * offset_acc_temp = 0;
     * for (i = 0; i < OFFSET_COUNT; i++)
     * {
     *     delay_ms(5);
     *     imu963ra_get_acc();
     *     if (imu963ra_acc_x == imu963ra_acc_y) { i--; }
     *     else { offset_acc_temp += imu963ra_acc_x; }
     * }
     * offset_accx = offset_acc_temp / OFFSET_COUNT;
     *
     * // Y轴
     * offset_acc_temp = 0;
     * for (i = 0; i < OFFSET_COUNT; i++)
     * {
     *     delay_ms(5);
     *     imu963ra_get_acc();
     *     if (imu963ra_acc_x == imu963ra_acc_y) { i--; }
     *     else { offset_acc_temp += imu963ra_acc_y; }
     * }
     * offset_accy = offset_acc_temp / OFFSET_COUNT;
     *
     * // Z轴
     * offset_acc_temp = 0;
     * for (i = 0; i < OFFSET_COUNT; i++)
     * {
     *     delay_ms(5);
     *     imu963ra_get_acc();
     *     if (imu963ra_acc_x == imu963ra_acc_y) { i--; }
     *     else { offset_acc_temp += imu963ra_acc_z; }
     * }
     * offset_accz = offset_acc_temp / OFFSET_COUNT;
     */

    imu963ra_offset_flag = 1; // 设置校准完成标志
}


/**
 * @brief  获取并对陀螺仪Z轴数据进行低通滤波
 * @param  None
 * @return void
 * @note   此函数更新全局变量 `gyroZ`。
 *         它首先获取陀螺仪原始数据，减去 `IMU_offset` 计算出的零漂值，
 *         然后应用一阶低通滤波（新值权重0.3，旧值权重0.7）。
 * @author yanyyy
 * @date   2025-09-19
 */
void gyro_low_pass_filter()
{
    imu963ra_get_gyro(); // 获取陀螺仪数据

    // 陀螺仪原始值减去零漂
    gyro_z_raw = imu963ra_gyro_z - offset_gz;

    // gyro_z_raw = gyro_z_raw / 64;             // (可选) 对数据进行缩放

    // 一阶低通滤波
    gyroZ = (gyroZ * 7 + gyro_z_raw * 3) / 10;
}


/**
 * @brief  获取并对加速度计Y轴数据进行低通滤波
 * @param  None
 * @return void
 * @note   此函数更新全局变量 `accelY`。
 *         当前版本仅处理Y轴数据。它减去零漂、进行缩放，
 *         然后应用一阶低通滤波。
 * @author yanyyy
 * @date   2025-09-19
 */
void acc_low_pass_filter()
{
    imu963ra_get_acc(); // 获取加速计数据

    // 加速计原始值减去零漂 (仅Y轴)
    // acc_x_raw = imu963ra_acc_x - offset_accx;
    acc_y_raw = imu963ra_acc_y - offset_accy;
    // acc_z_raw = imu963ra_acc_z - offset_accz;

    // 对原始数据进行缩放
    // acc_x_raw = acc_x_raw / 32;
    acc_y_raw = acc_y_raw / 16;
    // acc_z_raw = acc_z_raw / 32;

    // 一阶低通滤波 (仅Y轴)
    // accelX = (accelX * 7 + acc_x_raw * 3) / 10;
    accelY = (accelY * 7 + acc_y_raw * 3) / 10;
    // accelZ = (accelZ * 7 + acc_z_raw * 3) / 10;
}


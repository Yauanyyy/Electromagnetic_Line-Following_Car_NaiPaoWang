#include "headfile.h"

/**
 * @brief  阻塞等待电池充电完成
 * @param  maxvoltage: uint16, 判断电池充满的ADC采样阈值。
 * @return void
 * @note   此函数为阻塞函数，会持续循环直到满足退出条件。
 *         退出条件1: ADC采样值 `ad_result` 超过 `maxvoltage`。
 *         退出条件2: P7.0 引脚被拉低 (例如通过按键)，可手动退出。
 *         函数在循环时会在屏幕上显示实时ADC值，退出后会清屏。
 * @author yanyyy
 * @date   2025-09-19
 */
void waitForFullCharge(uint16 maxvoltage)
{
    // 循环直到充电标志位 charge_flag 被置为0
    while (charge_flag)
    {
        // 读取P15引脚的ADC值，用于电压检测
        ad_result = adc_once(ADC_P15, ADC_12BIT);

        /*
         * // 以下为根据ADC值计算实际电压的代码，已被注释
         * // 计算公式为: V_pin = ADC_val * VCC / Resolution
         * battery_voltage_temp = (((uint32)ad_result * 5000) / 4096);
         * // 根据分压电阻计算电池电压: V_bat = V_pin * (R_total / R_gnd)
         * battery_voltage = battery_voltage_temp * 11;
         *
         * // 在屏幕上显示计算出的电压值
         * ips114_showstr(0, 0, "voltage:");
         * ips114_showuint16(8*8, 0, battery_voltage);
         */

        // 在屏幕上显示原始ADC值，用于调试
        ips114_showstr(0, 1, "ADC = ");
        ips114_showuint16(6 * 8, 1, ad_result);

        // 判断ADC值是否超过设定的充电阈值
        if (ad_result > maxvoltage)
        {
            charge_flag = 0; // 达到阈值，清除充电标志，退出循环
        }

        // 检测手动退出信号 (P70引脚，例如按键按下)
        if (!P70)
        {
            charge_flag = 0; // 手动清除充电标志
            while (!P70);    // 等待按键释放，防止重复触发
        }

        // delay_ms(200); // 可以加入延时
    }

    // 充电完成后清屏
    ips114_clear(WHITE);
}

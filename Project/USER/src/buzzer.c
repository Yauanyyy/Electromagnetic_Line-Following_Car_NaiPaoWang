#include "headfile.h"

#define BEEP_DURATION_MS 100 // 定义单次鸣叫的持续时间 (毫秒)
#define BEEP_INTERVAL_MS 100 // 定义多次鸣叫之间的间隔时间 (毫秒)


/**
 * @brief  初始化蜂鸣器所连接的GPIO引脚
 * @param  None
 * @return void
 * @note   将引脚 P6.7 配置为推挽输出模式，并默认设置为低电平（关闭状态）。
 * @author yanyyy
 * @date   2025-09-19
 */
void buzzer_init()
{
    gpio_mode(P6_7, GPO_PP); // 将P6.7设置为推挽输出
    P67 = 0;                 // 默认关闭蜂鸣器
}


/**
 * @brief  控制蜂鸣器鸣叫指定的次数
 * @param  times: uint8, 蜂鸣器鸣叫的次数。
 * @return void
 * @note   每次鸣叫和间隔的时间由宏 `BEEP_DURATION_MS` 和 `BEEP_INTERVAL_MS` 定义。
 *         如果传入次数为0，函数将直接返回。
 * @author yanyyy
 * @date   2025-09-19
 */
void buzzer_beep(uint8 times)
{
    uint8 i = 0;

    // 参数校验，如果鸣叫次数为0则直接退出
    if (times == 0)
    {
        return;
    }

    for (i = 0; i < times; i++)
    {
        P67 = 1;                          // 蜂鸣器开启
        delay_ms(BEEP_DURATION_MS);       // 持续固定时间
        P67 = 0;                          // 蜂鸣器关闭

        // 如果不是最后一次鸣响，则等待一个间隔时间
        if (i < times - 1)
        {
            delay_ms(BEEP_INTERVAL_MS);
        }
    }
}


/**
 * @brief  持续开启蜂鸣器
 * @param  None
 * @return void
 * @note   该函数将P6.7引脚设置为高电平，使蜂鸣器一直发声，直到调用 buzzer_off()。
 * @author yanyyy
 * @date   2025-09-19
 */
void buzzer_on()
{
    P67 = 1;
}


/**
 * @brief  关闭蜂鸣器
 * @param  None
 * @return void
 * @note   该函数将P6.7引脚设置为低电平，停止蜂鸣器发声。
 * @author yanyyy
 * @date   2025-09-19
 */
void buzzer_off()
{
    P67 = 0;
}

#include "headfile.h"

/**
 * @brief  控制单个电机的PWM输出
 * @param  LoR: deviceSide, 指定要控制的电机 (LEFT 或 RIGHT)。
 * @param  pwm: int16, PWM占空比值。正值正转，负值反转。
 * @return void
 * @note   本函数底层驱动H桥。通过控制两路PWM实现正反转：
 *         - 正转: P62(L)/P66(R)输出PWM，P60(L)/P64(R)输出0。
 *         - 反转: P62(L)/P66(R)输出0，P60(L)/P64(R)输出PWM。
 *         引脚对应关系:
 *         - 左电机: P62 (PWMA_CH2P) <-> 1B, P60 (PWMA_CH1P) <-> 2B
 *         - 右电机: P66 (PWMA_CH4P) <-> 1A, P64 (PWMA_CH3P) <-> 2A
 * @author yanyyy
 * @date   2025-09-19
 */
void motorPWMOut(deviceSide LoR, int16 pwm)
{
    // 定义电机控制引脚 (使用static const在编译时确定，效率更高)
    static const PWMCH_enum motorL_P = PWMA_CH2P_P62; // 左电机正转PWM
    static const PWMCH_enum motorL_N = PWMA_CH1P_P60; // 左电机反转PWM
    static const PWMCH_enum motorR_P = PWMA_CH4P_P66; // 右电机正转PWM
    static const PWMCH_enum motorR_N = PWMA_CH3P_P64; // 右电机反转PWM

    switch (LoR)
    {
        case LEFT:
            if (pwm >= 0)
            {
                // 正转
                pwm_duty(motorL_P, pwm);
                pwm_duty(motorL_N, 0);
            }
            else
            {
                // 反转
                pwm_duty(motorL_P, 0);
                pwm_duty(motorL_N, -pwm); // pwm为负，需取反
            }
            break;

        case RIGHT:
            if (pwm >= 0)
            {
                // 正转
                pwm_duty(motorR_P, pwm);
                pwm_duty(motorR_N, 0);
            }
            else
            {
                // 反转
                pwm_duty(motorR_P, 0);
                pwm_duty(motorR_N, -pwm); // pwm为负，需取反
            }
            break;

        default:
            break;
    }
}


/**
 * @brief  电机速度闭环PID控制器
 * @param  LoR: deviceSide, 指定要控制的电机 (LEFT 或 RIGHT)。
 * @param  speed: float, 目标速度值 (单位: 编码器脉冲数/控制周期)。
 * @return void
 * @note   采用增量式PID算法。根据目标速度与编码器反馈的差值，
 *         计算出PWM增量，并更新总输出 `Lputout`/`Rputout`。
 *         输出值被限幅在 [-3000, 3000] 之间。
 * @author yanyyy
 * @date   2025-09-19
 */
void putOut(deviceSide LoR, float speed)
{
    switch (LoR)
    {
        case LEFT:
            // 1. 计算本次误差
            errorL = speed - encoderL;

            // 2. 增量式PID计算
            Lputout += (errorL * motor_ki)                                       // 积分项增量
                       + (errorL - errorL_Before) * motor_kp                     // 比例项增量
                       + (errorL - 2 * errorL_Before + errorL_Before_Before) * motor_kd; // 微分项增量

            // 3. 更新历史误差
            errorL_Before_Before = errorL_Before;
            errorL_Before = errorL;

            // 4. 输出限幅
            if (Lputout > 3000)      Lputout = 3000;
            else if (Lputout < -3000) Lputout = -3000;

            // 5. PWM输出
            motorPWMOut(LEFT, Lputout);
            break;

        case RIGHT:
            // 1. 计算本次误差
            errorR = speed - encoderR;

            // 2. 增量式PID计算
            Rputout += (errorR * motor_ki)
                       + (errorR - errorR_Before) * motor_kp
                       + (errorR - 2 * errorR_Before + errorR_Before_Before) * motor_kd;

            // 3. 更新历史误差
            errorR_Before_Before = errorR_Before;
            errorR_Before = errorR;

            // 4. 输出限幅
            if (Rputout > 3000)      Rputout = 3000;
            else if (Rputout < -3000) Rputout = -3000;
            
            // 5. PWM输出
            motorPWMOut(RIGHT, Rputout);
            break;

        default:
            break;
    }
}


/**
 * @brief  将float类型数据转换为int类型
 * @param  data1: float, 输入的浮点数。
 * @return int, 截断取整后的整数。
 * @note   此函数可能用于将PID参数等浮点数发送到上位机或屏幕显示，
 *         因为某些通信协议或显示函数只支持整数。
 * @author yanyyy
 * @date   2025-09-19
 */
int Z_change(float data1)
{
    return (int)data1; // 显式类型转换
}



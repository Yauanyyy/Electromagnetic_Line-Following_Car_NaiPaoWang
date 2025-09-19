#include "headfile.h"

#define BEEP_DURATION_MS 100 // ���嵥�����еĳ���ʱ�� (����)
#define BEEP_INTERVAL_MS 100 // ����������֮��ļ��ʱ�� (����)


/**
 * @brief  ��ʼ�������������ӵ�GPIO����
 * @param  None
 * @return void
 * @note   ������ P6.7 ����Ϊ�������ģʽ����Ĭ������Ϊ�͵�ƽ���ر�״̬����
 * @author yanyyy
 * @date   2025-09-19
 */
void buzzer_init()
{
    gpio_mode(P6_7, GPO_PP); // ��P6.7����Ϊ�������
    P67 = 0;                 // Ĭ�Ϲرշ�����
}


/**
 * @brief  ���Ʒ���������ָ���Ĵ���
 * @param  times: uint8, ���������еĴ�����
 * @return void
 * @note   ÿ�����кͼ����ʱ���ɺ� `BEEP_DURATION_MS` �� `BEEP_INTERVAL_MS` ���塣
 *         ����������Ϊ0��������ֱ�ӷ��ء�
 * @author yanyyy
 * @date   2025-09-19
 */
void buzzer_beep(uint8 times)
{
    uint8 i = 0;

    // ����У�飬������д���Ϊ0��ֱ���˳�
    if (times == 0)
    {
        return;
    }

    for (i = 0; i < times; i++)
    {
        P67 = 1;                          // ����������
        delay_ms(BEEP_DURATION_MS);       // �����̶�ʱ��
        P67 = 0;                          // �������ر�

        // ����������һ�����죬��ȴ�һ�����ʱ��
        if (i < times - 1)
        {
            delay_ms(BEEP_INTERVAL_MS);
        }
    }
}


/**
 * @brief  ��������������
 * @param  None
 * @return void
 * @note   �ú�����P6.7��������Ϊ�ߵ�ƽ��ʹ������һֱ������ֱ������ buzzer_off()��
 * @author yanyyy
 * @date   2025-09-19
 */
void buzzer_on()
{
    P67 = 1;
}


/**
 * @brief  �رշ�����
 * @param  None
 * @return void
 * @note   �ú�����P6.7��������Ϊ�͵�ƽ��ֹͣ������������
 * @author yanyyy
 * @date   2025-09-19
 */
void buzzer_off()
{
    P67 = 0;
}

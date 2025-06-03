#include "buzzer.h"

#include "tim.h"
void BUZZER_0N(uint16_t frequency)
{
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
    uint32_t period = (uint32_t)(1e6 / frequency) - 1; // 计算周期
    __HAL_TIM_SET_AUTORELOAD(&htim3, period);

    uint32_t pulse = period / 2; // 默认占空比为50%
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, pulse);
}

void updateBuzzerTone(uint16_t voltage)
{
    uint16_t frequency = 2000 - (voltage - 12) * (2000 - 500) / (27 - 8);

    // 限制频率在合理范围内
    if (frequency > 2000) {
        frequency = 2000;
    } else if (frequency < 500) {
        frequency = 500;
    }

    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
    uint32_t period = (uint32_t)(1e6 / frequency) - 1; // 计算周期
    __HAL_TIM_SET_AUTORELOAD(&htim3, period);

    uint32_t pulse = period / 2; // 默认占空比为50%
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, pulse);
}
void BUZZER_Stop()
{
    HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_4);
}

void Buzzer_Startup_Prompting(void)
{
    BUZZER_0N(L6);
    HAL_Delay(114);
    BUZZER_0N(L4);
    HAL_Delay(226);
    BUZZER_0N(L2);
    HAL_Delay(283);
    BUZZER_Stop();
}
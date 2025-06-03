#include "wlc_ctrl.h"
#include "incremental_pid.h"
#include "bsp_hrtim.h"
#include "analog_signal.h"

#include "main.h"
#include "hrtim.h"
#include "stm32g474xx.h"
#include "stm32g4xx_hal.h"
#include "tim.h"
#include <stdint.h>

#define FSBB_GENERAL_TO_NARROW_RATIO 0.95f // 广义占空比到狭义占空比的比例

//
// pid参数结构体
//
incremental_pid_t pid_current;
incremental_pid_t pid_voltage;
incremental_pid_t pid_power;

float test_P = 0.05f;
float test_I = 0.005f;
float test_D = 0.0f;

//
analogdata_t analogdata;
//
void FSBB_PID_init()
{
    // 初始化pid

    // current PID
    incremental_pid_init(&pid_current, 0.005f, 0.05f, 0.0f, -2.5f, +2.5f);
    // voltage PID
    incremental_pid_init(&pid_voltage, 0.585f, 0.185f, 0.0f, -16.0f, +16.0f);
    // power PID
    incremental_pid_init(&pid_power, 0.0375f, 0.05f, 0.0f, +5.0f, +24.0f);
}

//
void WLC_CTRL_INIT(void)
{
    HAL_GPIO_WritePin(EN_12V_GPIO_Port, EN_12V_Pin, GPIO_PIN_SET);

    HAL_HRTIM_WaveformCounterStart(&hhrtim1, HRTIM_TIMERID_MASTER);
    // HRTIM_PWM_init();
    HAL_Delay(2);
    FSBB_PID_init();
    HAL_Delay(2);
    BSP_ADC_Convert_Start();
    HAL_Delay(2);

    // FSBB TIMER B&A
    HAL_HRTIM_WaveformCounterStart(&hhrtim1, HRTIM_TIMERID_TIMER_A);
    HAL_HRTIM_WaveformCounterStart(&hhrtim1, HRTIM_TIMERID_TIMER_B);
    // LCC TIMER C&D
    HAL_HRTIM_WaveformCounterStart(&hhrtim1, HRTIM_TIMERID_TIMER_C);
    HAL_HRTIM_WaveformCounterStart(&hhrtim1, HRTIM_TIMERID_TIMER_D);

    //
    FSBB_output_start();
    HAL_Delay(10);
    LCC_output_start();
    // 开启中断计算PID
    //
    HAL_HRTIM_WaveformCountStart_IT(&hhrtim1, HRTIM_TIMERID_MASTER);
    //
    HAL_TIM_Base_Start_IT(&htim6);
}

void FSBB_pwm_set(float general_duty, uint32_t timerIndex)
{
    const float general_duty_min = 0.2f; // 广义占空比最低值
    const float general_duty_max = 1.0f; // 广义占空比占空比最高值

    // 使用条件运算符限制数值
    general_duty = (general_duty < general_duty_min)   ? general_duty_min
                   : (general_duty > general_duty_max) ? general_duty_max
                                                       : general_duty;

    float duty_cycle = general_duty * FSBB_GENERAL_TO_NARROW_RATIO;

    HRTIM_PWM_duty_set(duty_cycle, timerIndex);
    HRTIM_PWM_duty_set(duty_cycle, timerIndex);
}

void FSBB_pwm_set_factor(float scaling_factor)
{
    const float factor_range_min = 0.5f; // 允许的倍数的最小值
    const float factor_range_max = 2.5f; // 允许的倍数的最大值

    // 把倍数限制在范围内
    scaling_factor = (scaling_factor < factor_range_min)   ? factor_range_min
                     : (scaling_factor > factor_range_max) ? factor_range_max
                                                           : scaling_factor;

    if (scaling_factor <= 1.0f) {
        FSBB_pwm_set(scaling_factor, HRTIM_TIMERINDEX_TIMER_B);
        FSBB_pwm_set(1.0f, HRTIM_TIMERINDEX_TIMER_A);

    } else {
        FSBB_pwm_set(1.0f, HRTIM_TIMERINDEX_TIMER_B);
        FSBB_pwm_set(1.0f / scaling_factor, HRTIM_TIMERINDEX_TIMER_A);
    }
}

// 开启PWM输出
void pwm_output_start(channel_t channel)
{
    if (channel == FSBB) {
        HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TA1 | HRTIM_OUTPUT_TA2 | HRTIM_OUTPUT_TB1 | HRTIM_OUTPUT_TB2);
    } else if (channel == LCC) {
        // LCC PWM 输出
        HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TC1 | HRTIM_OUTPUT_TC2 | HRTIM_OUTPUT_TD1 | HRTIM_OUTPUT_TD2);
    } else {
        // 处理无效的通道
        Error_Handler();
    }
}

// 关闭PWM输出
void pwm_output_stop(channel_t channel)
{
    if (channel == FSBB) {
        HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TA1 | HRTIM_OUTPUT_TA2 | HRTIM_OUTPUT_TB1 | HRTIM_OUTPUT_TB2);
    } else if (channel == LCC) {
        // LCC PWM 输出
        HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TC1 | HRTIM_OUTPUT_TC2 | HRTIM_OUTPUT_TD1 | HRTIM_OUTPUT_TD2);
    } else {
        // 处理无效的通道
        Error_Handler();
    }
}

// 开启 FSBB PWM 输出
void FSBB_output_start()
{
    get_all_analog_data(&analogdata);
    FSBB_pwm_set_factor(analogdata.v_fsbb_output / analogdata.v_fsbb_input);
    pwm_output_start(FSBB);
}
// 开启 LCC PWM 输出
void LCC_output_start()
{
    HRTIM_PWM_duty_set(0.50f, HRTIM_TIMERINDEX_TIMER_C);
    HRTIM_PWM_duty_set(0.50f, HRTIM_TIMERINDEX_TIMER_D);
    pwm_output_start(LCC);
}

//
uint32_t hrtimrepe_test_value = 0;
//
void HAL_HRTIM_RepetitionEventCallback(HRTIM_HandleTypeDef *hhrtim, uint32_t TimerIdx)
{
    if (hhrtim->Instance == HRTIM1) {
        // 处理定时器的重复事件
        if (TimerIdx == HRTIM_TIMERID_MASTER) {
            // 处理主定时器的重复事件
            // hrtimrepe_test_value++; // 设置一个测试值
        }
    }
}

//
float target_power = 125.0f; // 目标功率
//
float ref_duty = 0;
//

//
// PID控制器
//

// 主环路输出
float MainLoop_Output(analogdata_t *analogdata)
{
    float ref_duty    = 0.0f;
    float ref_voltage = 0.0f;
    float ref_current = 0.0f;
    // power PID
    ref_voltage = incremental_pid_compute(&pid_power, analogdata->i_fsbb_input * analogdata->v_fsbb_input, target_power);
    //  voltage PID
    ref_current = incremental_pid_compute(&pid_voltage, analogdata->v_fsbb_output, ref_voltage);
    // current PID
    ref_duty = incremental_pid_compute(&pid_current, analogdata->i_fsbb_output, ref_current);

    return ref_duty;
}

// 定时器中断回调函数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM6) {

        // 获取模拟数据
        get_all_analog_data(&analogdata);

        // PID计算
        // ref_duty = MainLoop_Output(&analogdata);

        // 根据广义占空比输出PWM
        FSBB_pwm_set_factor(MainLoop_Output(&analogdata));
    }
    if (htim->Instance == TIM7) {
        //RIFD识别
        
    }
}

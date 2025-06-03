#pragma once
#include <stdint.h>
#ifndef __WLC_CTRL_H__
#define __WLC_CTRL_H__
#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    FSBB = 0,
    LCC  = 1,
    ALL  = 2,
} channel_t;

typedef enum {
    WLC_NORMAL            = 0, // 正常工作模式
    WLC_LOWPOWER_MODE     = 1, // 低功耗模式
    WLC_CHARGING_CAP_MODE = 2, // 充电容模式，关闭LCC，FSBB工作
    WLC_COMM_MODE         = 3, // 通讯模式，间歇性开启LCC，识别接收端
    WLC_TEST              = 4, // 测试模式

    WLC_ERROR = 5 // 异常错误
} wlcmode_t;

extern void WLC_CTRL_INIT(void);
extern void FSBB_output_start(void);
extern void LCC_output_start(void);
extern void pwm_output_start(channel_t channel);
extern void FSBB_pwm_set_factor(float scaling_factor);
extern void FSBB_pwm_set(float general_duty, uint32_t timerIndex);

#ifdef __cplusplus
}
#endif
#endif // !__WLC_CTRL_H__

#pragma once
#ifndef __BSP_HRTIM_H__
#define __BSP_HRTIM_H__
#ifdef __cplusplus
extern "C" {
#endif
#include "hrtim.h"
extern void HRTIM_PWM_init(void);
extern void HRTIM_SetTimerAOutput1Polarity(HRTIM_HandleTypeDef *hhrtim, uint32_t polarity);
extern void HRTIM_PWM_output_start(void);
extern void HRTIM_PWM_output_stop(void);
extern void HRTIM_PWM_duty_set(float dutyCycle, uint32_t timerIndex);
extern void HRTIM_Phase_shift_angle_set(float angle, uint8_t mode);
#ifdef __cplusplus
}
#endif
#endif // !__BSP_HRTIM_H__
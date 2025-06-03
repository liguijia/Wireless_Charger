#pragma once
#ifndef __ANALOG_SIGNAL_H__
#define __ANALOG_SIGNAL_H__

// 模拟量数据结构体
typedef struct {

    // FSBB
    float v_fsbb_input;  // 输入电压
    float v_fsbb_output; // 输出电压
    float i_fsbb_input;  // 输入电流
    float i_fsbb_output; // 输出电流

    // envelope_detector
    float v_envelope_detector_input; // 包络检波输入电压

    // ntc
    float v_ntc_input; // NTC 输入电压
} analogdata_t;

extern analogdata_t analogdata;

extern void BSP_ADC_Convert_Start(void);

void get_all_analog_data(analogdata_t *data);

#endif // !__ANALOG_SIGNAL_H__

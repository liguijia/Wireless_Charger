#include "analog_signal.h"
#include "mean_filter.h"

#include "adc.h"

#include <stdint.h>

// 定义数据类型和结构体
typedef struct
{
    float k;
    float b;
} adc_calibration_t;

typedef struct
{
    uint32_t stm32id[3]; // 96 bit stm32 id

    adc_calibration_t v_fsbb_input;  // fsbb输入电压线性拟合数据
    adc_calibration_t v_fsbb_output; // fsbb输出电压线性拟合数据
    adc_calibration_t i_fsbb_input;  // fsbb输入电流线性拟合数据
    adc_calibration_t i_fsbb_output; // fsbb输出电流线性拟合数据

    adc_calibration_t v_envelope_detector_input; // 包络检波 ADC2 校准参数 (k, b)
    adc_calibration_t v_ntc_input;               // NTC ADC3 校准参数 (k, b)

} board_adc_calibration_t;

// 全局变量定义
static board_adc_calibration_t adc_cali_array[] = {
    // board_adc_calibration insert start
    {
        {0x00000000, 0x00000000, 0x00000222}, // STM32 唯一 ID

        // FSBB
        // voltage ：R1=49.9k  R2=3.3k
        {0.0008105675f, -0.0000000000f}, // fsbb_input_voltage 校准参数 (k, b)
        {0.0008135675f, -0.0000000000f}, // fsbb_output_voltage 校准参数 (k, b)
        // INA240A2 0.002R VREF=3.3V or INA240A1 0.005R VREF=3.3V
        {0.0004985299f, -16.200000000f}, // fsbb_input_current 校准参数 (k, b)
        {0.0004965033f, -16.300000000f}, // fsbb_output_current 校准参数 (k, b)

        // Envelope Detector
        {0.8555450004f, -0.0000000000f}, // 包络检波 校准参数 (k, b)

        // NTC
        {0.0000160859f, -0.0000000000f}, // NTC 校准参数 (k, b)
    },
    {
        {0x00000000, 0x00000000, 0x00000111}, // STM32 唯一 ID
        {0.0005204504f, -0.2722371015f},
        {0.0009980598f, -32.6001642915f},
        {0.0005204504f, -0.2722371015f},
        {0.0009980598f, -32.6001642915f},

        // {0.0005554504f, -0.0000000000f},      //包络检波 ADC2 校准参数 (k, b)

        // {0.0005035400f, -16.5000000000f},     //NTC ADC3 校准参数 (k, b)

    }
    // board_adc_calibration insert stop
};

#define ADC1_DATA_LEN (4U)
#define ADC2_DATA_LEN (1U)
#define ADC3_DATA_LEN (1U)

uint16_t adc1_data[ADC1_DATA_LEN * 2] = {0};
uint16_t adc2_data[ADC2_DATA_LEN * 2] = {0};
uint16_t adc3_data[ADC3_DATA_LEN * 2] = {0};

#define FILTER_WINDOW_SIZE 8 // 定义滤波窗口大小

static mean_filter_t v_fsbb_input_filter  = {0};
static mean_filter_t v_fsbb_output_filter = {0};
static mean_filter_t i_fsbb_input_filter  = {0};
static mean_filter_t i_fsbb_output_filter = {0};

static mean_filter_t v_envelope_detector_filter = {0};

static mean_filter_t v_ntc_filter = {0};

// 整合后的通用映射函数（单函数完成滤波+线性映射）
static inline float get_mapped_value(mean_filter_t *filter, const adc_calibration_t *calib)
{
    // 1. 执行均值滤波 → 2. 应用线性校准公式
    return calib->k * mean_filter_calculate_average(filter) + calib->b;
}

// 获取全部模拟量
void get_all_analog_data(analogdata_t *data)
{
    // FSBB 模拟量采集
    //  电压采集
    data->v_fsbb_input  = get_mapped_value(&v_fsbb_input_filter,
                                           &adc_cali_array[0].v_fsbb_input);
    data->v_fsbb_output = get_mapped_value(&v_fsbb_output_filter,
                                           &adc_cali_array[0].v_fsbb_output);
    // 电流采样
    data->i_fsbb_input  = get_mapped_value(&i_fsbb_input_filter,
                                           &adc_cali_array[0].i_fsbb_input);
    data->i_fsbb_output = get_mapped_value(&i_fsbb_output_filter,
                                           &adc_cali_array[0].i_fsbb_output);

    // 包络检波采样
    data->v_envelope_detector_input = get_mapped_value(&v_envelope_detector_filter,
                                                       &adc_cali_array[0].v_envelope_detector_input);

    // NTC 模拟量采集
    data->v_ntc_input = get_mapped_value(&v_ntc_filter,
                                         &adc_cali_array[0].v_ntc_input);
}

void BSP_ADC_Convert_Start(void)
{
    HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
    HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED);
    HAL_ADCEx_Calibration_Start(&hadc3, ADC_SINGLE_ENDED);

    HAL_Delay(5);

    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc1_data, ADC1_DATA_LEN * 2);
    HAL_ADC_Start_DMA(&hadc2, (uint32_t *)adc2_data, ADC2_DATA_LEN * 2);
    HAL_ADC_Start_DMA(&hadc3, (uint32_t *)adc3_data, ADC3_DATA_LEN * 2);

    mean_filter_init(&v_fsbb_input_filter, FILTER_WINDOW_SIZE);
    mean_filter_init(&v_fsbb_output_filter, FILTER_WINDOW_SIZE);
    mean_filter_init(&i_fsbb_input_filter, FILTER_WINDOW_SIZE);
    mean_filter_init(&i_fsbb_output_filter, FILTER_WINDOW_SIZE);

    mean_filter_init(&v_envelope_detector_filter, FILTER_WINDOW_SIZE);

    mean_filter_init(&v_ntc_filter, FILTER_WINDOW_SIZE);
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance == ADC1) {
        // FSBB ADC1 采样数据
        mean_filter_update(&v_fsbb_input_filter, adc1_data[0]);
        mean_filter_update(&v_fsbb_output_filter, adc1_data[1]);
        mean_filter_update(&i_fsbb_input_filter, adc1_data[2]);
        mean_filter_update(&i_fsbb_output_filter, adc1_data[3]);
    }
    if (hadc->Instance == ADC2) {
        // 包络检波 ADC2 采样数据
        mean_filter_update(&v_envelope_detector_filter, adc2_data[0]);
    }
    if (hadc->Instance == ADC3) {
        // NTC ADC3 采样数据
        mean_filter_update(&v_ntc_filter, adc3_data[0]);
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance == ADC1) {
        // FSBB ADC1 采样数据
        mean_filter_update(&v_fsbb_input_filter, adc1_data[0 + ADC1_DATA_LEN]);
        mean_filter_update(&v_fsbb_output_filter, adc1_data[1 + ADC1_DATA_LEN]);
        mean_filter_update(&i_fsbb_input_filter, adc1_data[2 + ADC1_DATA_LEN]);
        mean_filter_update(&i_fsbb_output_filter, adc1_data[3 + ADC1_DATA_LEN]);
    }
    if (hadc->Instance == ADC2) {
        // 包络检波 ADC2 采样数据
        mean_filter_update(&v_envelope_detector_filter, adc2_data[0 + ADC2_DATA_LEN]);
    }
    if (hadc->Instance == ADC3) {
        // NTC ADC3 采样数据
        mean_filter_update(&v_ntc_filter, adc3_data[0 + ADC3_DATA_LEN]);
    }
}
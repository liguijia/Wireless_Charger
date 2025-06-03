#include "mean_filter.h"

// 初始化均值滤波器
void mean_filter_init(mean_filter_t* filter, uint16_t size)
{
    if (size == 0 || size > MAX_WINDOW_SIZE)
    {
        return;
    }

    filter->window_size = size;
    filter->head        = 0;
    filter->sum         = 0;

    // 初始化窗口为0
    for (uint16_t i = 0; i < size; ++i)
    {
        filter->window[i] = 0;
    }
}

// 更新均值滤波器状态
void mean_filter_update(mean_filter_t* filter, uint16_t new_sample)
{
    // 如果滤波器未正确初始化，则不执行任何操作
    if (filter->window_size == 0)
    {
        return;
    }

    // 减去即将被替换的旧样本的值
    filter->sum -= filter->window[filter->head];

    // 更新窗口中的样本
    filter->window[filter->head] = new_sample;

    // 添加新样本的值
    filter->sum += new_sample;

    // 移动头部指针到下一个位置
    filter->head = (filter->head + 1) % filter->window_size;
}

// 计算当前平均值
uint16_t mean_filter_calculate_average(const mean_filter_t* filter)
{
    // 如果滤波器未正确初始化，则返回0
    if (filter->window_size == 0)
    {
        return 0;
    }

    // 计算并返回平均值
    return (uint16_t)(filter->sum / filter->window_size);
}
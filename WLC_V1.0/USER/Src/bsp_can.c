#include "main.h"
#include "fdcan.h"

/**************************************************************************************
 * User-defined Headers
 *************************************************************************************/
#include "bsp_can.h"


extern uint8_t target_power;

// 从板子上读取的值 反应PMOS是否真实开启
char PMOS_flag_from_real = 0;

void SurperCap_Process(uint8_t Data[8]);

/**************************************************************************************
 * @brief FDCAN的过滤器初始化函数
 *
 * @version 1.0
 * @author Hong HongLin (wxxdada@163.com)
 * @date 2024 - 04 - 15
 * @copyright Copyright (C) 2024 Hong HongLin.  All Rights Reserved.
 *************************************************************************************/
void BSP_FDCAN_Filter_Init(void)
{
    FDCAN_FilterTypeDef sFilterConfig;

    /* 配置过滤器 */
    sFilterConfig.FilterIndex  = 0;                       // 过滤器索引，范围为0到27
    sFilterConfig.FilterType   = FDCAN_FILTER_MASK;       // 设置过滤器类型为掩码模式
    sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0; // 将过滤器结果传输到RX FIFO0
    sFilterConfig.FilterID1    = 0x0000;                  // 过滤器ID1
    sFilterConfig.FilterID2    = 0x0000;                  // 过滤器ID2
    sFilterConfig.IdType       = FDCAN_STANDARD_ID;       // 使用标准标识符

    // 配置过滤器
    if (HAL_FDCAN_ConfigFilter(&hfdcan2, &sFilterConfig) != HAL_OK) {
        Error_Handler(); // 配置过滤器失败处理
    }
}

/**************************************************************************************
 * @brief FDCAN初始化函数
 *
 * @version 1.0
 * @author Hong HongLin (wxxdada@163.com)
 * @date 2024 - 04 - 27
 * @copyright Copyright (C) 2024 Hong HongLin.  All Rights Reserved.
 *************************************************************************************/
void BSP_FDCAN_Init(void)
{
    BSP_FDCAN_Filter_Init(); // 初始化FDCAN过滤器

    HAL_FDCAN_Start(&hfdcan2); // 启动FDCAN总线

    // 激活FDCAN接收FIFO0的新消息通知中断
    // FDCAN_IT_RX_FIFO0_NEW_MESSAGE 表示接收FIFO0有新消息时触发中断
    // 第三个参数为优先级，设置为0表示最高优先级
    HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
}

/**************************************************************************************
 * @brief FDCAN消息发送函数
 *
 * @param data 发送的数据指针
 * @version 1.0
 * @author Hong HongLin (wxxdada@163.com)
 * @date 2024 - 04 - 27
 * @copyright Copyright (C) 2024 Hong HongLin.  All Rights Reserved.
 *************************************************************************************/
void BSP_FDCAN_Transmit(uint8_t *data)
{
    FDCAN_TxHeaderTypeDef TxHeader; // FDCAN消息头结构体

    // 设置FDCAN消息头的各项参数
    TxHeader.Identifier          = 0x300;              // 消息标识符
    TxHeader.IdType              = FDCAN_STANDARD_ID;  // 使用标准标识符
    TxHeader.TxFrameType         = FDCAN_DATA_FRAME;   // 数据帧类型
    TxHeader.DataLength          = FDCAN_DLC_BYTES_8;  // 数据长度为8字节
    TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;   // 激活错误状态指示器
    TxHeader.BitRateSwitch       = FDCAN_BRS_OFF;      // 比特率切换关闭
    TxHeader.FDFormat            = FDCAN_CLASSIC_CAN;  // 使用经典CAN格式
    TxHeader.TxEventFifoControl  = FDCAN_NO_TX_EVENTS; // 不使用发送事件FIFO
    TxHeader.MessageMarker       = 0;                  // 消息标记为0

    // 向FDCAN的发送FIFO队列中添加消息
    HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan2, &TxHeader, data);
}

/**************************************************************************************
 * @brief   FDCAN接收FIFO 0的回调函数
 *          Callback function for receiving messages in FDCAN FIFO 0.
 *
 * @param   hfdcan FDCAN句柄指针，指向FDCAN控制器
 *                 Pointer to FDCAN handle, points to the FDCAN controller.
 * @param   RxFifo0ITs 触发回调的中断标志位
 *                      Interrupt flags that trigger the callback.
 * @version 1.0
 * @author  Hong HongLin (wxxdada@163.com)
 * @date    2024 - 04 - 27
 * @copyright Copyright (C) 2024 Hong HongLin.
 *            All Rights Reserved.
 *************************************************************************************/
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
    FDCAN_RxHeaderTypeDef RxHeader; // FDCAN接收消息的标头信息结构体
    uint8_t RxData[8];              // 存储接收数据的数组，最大长度为8字节

    // 检查接收FIFO 0是否有新消息
    if ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET) {
        // 读取接收的消息
        if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &RxHeader, RxData) != HAL_OK) {
            Error_Handler(); // 读取消息出错处理
        }

        // 处理接收到的数据
        switch (RxHeader.Identifier) {
            case 0x1FE:
                SurperCap_Process(RxData);
                break;

            default:
                break;
        }
    }
}

// static float uint_to_float(int x_int, float x_min, float x_max, int bits)
// {
//     float span   = x_max - x_min;
//     float offset = x_min;
//     return ((float)x_int) * span / ((float)((1 << bits) - 1)) + offset;
// }
/*将一个float数以无符号形势存在一个int数低位*/
static int float_to_uint(float x, float x_min, float x_max, int bits)
{
    if (x < x_min) {
        x = x_min;
    } else if (x > x_max) {
        x = x_max;
    }
    float span   = x_max - x_min;
    float offset = x_min;
    return (int)((x - offset) * ((float)((1 << bits) - 1)) / span);
}

/**************************************************************************************
 * @brief 电容数据反馈函数
 *
 * @param Chassis_Power 底盘功率
 * @param SuperCap_Volt 电容电压
 * @param SuperCap_Power 超电充放功率，充电为正
 * @param Mode 超电当前模式
 *************************************************************************************/
void SuperCap_Transmit(float Chassis_Power, float SuperCap_Volt, float Chassis_Volt)
{

    struct __packed {
        uint16_t chassis_power;
        uint16_t supercap_voltage;
        uint16_t chassis_voltage;
        uint8_t pmos_flag_from_real;
        uint8_t unused;
    } tx_buffer;
    tx_buffer.chassis_power    = float_to_uint(Chassis_Power, 0.0f, 500.0f, 16);
    tx_buffer.supercap_voltage = float_to_uint(SuperCap_Volt, 0.0f, 50.0f, 16);
    tx_buffer.chassis_voltage  = float_to_uint(Chassis_Volt, 0.0f, 50.0f, 16);

    tx_buffer.pmos_flag_from_real = PMOS_flag_from_real;
    BSP_FDCAN_Transmit((uint8_t *)(&tx_buffer));

    if (PMOS_flag_from_real == 1)
    {
    
    }else{
 
    }
    
}

void SurperCap_Process(uint8_t Data[8])
{
    // target_power = Data[6];

}
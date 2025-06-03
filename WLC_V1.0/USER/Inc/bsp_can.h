#pragma once

#include <stdint.h>

extern void BSP_FDCAN_Filter_Init(void);
extern void BSP_FDCAN_Init(void);
extern void BSP_FDCAN_Transmit(uint8_t *data);
void SuperCap_Transmit(float Chassis_Power, float SuperCap_Volt, float Chassis_Volt);
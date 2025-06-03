#pragma once

#include <stdint.h>

extern void BSP_RS485_Filter_Init(void);
extern void BSP_RS485_Init(void);
extern void BSP_RS485_Transmit(uint8_t *data);
#pragma once

#include <stdint.h>

#define MTK_UART_BASE       0x11001000

#define MTK_UART_THR        0x00
#define MTK_UART_LSR        0x14

#define MTK_UART_LSR_THRE   (1 << 5)

void mtk_uart_putc(int ch);
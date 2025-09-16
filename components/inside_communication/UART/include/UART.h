#ifndef _UART_H_
#define _UART_H_

#include <stdio.h>
#include <stdbool.h>
// #include <unistd.h>  // ESP-IDF 5.5中已弃用
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"  // ESP-IDF 5.5中仍然使用原路径
#include "string.h"
#include "driver/gpio.h"  // ESP-IDF 5.5中仍然使用原路径

static const int RX_BUF_SIZE = 1024;//接收缓冲区大小

#define TXD_PIN    43//(GPIO_NUM_4)//发送引脚
#define RXD_PIN    44//(GPIO_NUM_5)//接收引脚

void uart1_init(int baud);//复位函数输入波特率
void tx_task(void *arg);//发送函数
void rx_task(void *arg);//接送函数
int sendData( const char* data);//发送函数

#endif /* COMPONENTS_UART_INCLUDE_UART1_H_ */

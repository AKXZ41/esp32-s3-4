#ifndef COMPONENTS_INSIDE_COMMUNICATION_SPI_INCLUDE_SPI_H_
#define COMPONENTS_INSIDE_COMMUNICATION_SPI_INCLUDE_SPI_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"  // ESP-IDF 5.5中仍然使用原路径
#include "driver/gpio.h"  // ESP-IDF 5.5中仍然使用原路径
#include "sdkconfig.h"
#include "esp_log.h"

void spi_init(void);//spi初始化
int SI24R1_Write_Reg(uint8_t reg_addr, uint8_t data);//spi写入单个字节数据到指定寄存器
int SI24R1_Read_Reg(uint8_t reg_addr, uint8_t *data);//spi读取指定寄存器的单个字节数据
int SI24R1_Write_Buf(uint8_t reg_addr, uint8_t *data, uint8_t len);//spi写入数据到指定寄存器
int SI24R1_Read_Buf(uint8_t reg_addr, uint8_t *data, uint8_t len);//spi读取指定寄存器的数据


#endif /* COMPONENTS_INSIDE_COMMUNICATION_SPI_INCLUDE_SPI_H_ */

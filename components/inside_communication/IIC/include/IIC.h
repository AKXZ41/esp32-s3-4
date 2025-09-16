#ifndef IIC_H_
#define IIC_H_

#include "driver/i2c.h"  // ESP-IDF 5.5中仍然使用原路径
#include <stdio.h>
#include "esp_system.h"

#define I2C_MASTER_SCL_IO        1      					//IIC时钟线GPIO端口号 1  19
#define I2C_MASTER_SDA_IO        2      					//IIC数据线GPIO端口号 2   18
#define I2C_MASTER_NUM           0      					//IIC端口号(0或1)
#define I2C_MASTER_FREQ_HZ       1000000 					//IIC速率
#define I2C_MASTER_TX_BUF_SIZE   0      					//IIC发送缓冲区
#define I2C_MASTER_RX_BUF_SIZE   0      					//IIC接送缓冲区
#define I2C_MASTER_TIMEOUT_MS    pdMS_TO_TICKS(1000)	//IIC超时时间

//声明函数，只有在这里声明的函数才能被外部调用
void IIC_init(void);
int iic_read(i2c_port_t i2c_num, uint8_t device_address,uint8_t reg_address,uint8_t* read_buffer, size_t read_size);//IIC读数据
int iic_write(i2c_port_t i2c_num, uint8_t device_address,uint8_t reg_address,uint8_t write_buffer); //IIC写数据


#endif /* IIC_H_ */

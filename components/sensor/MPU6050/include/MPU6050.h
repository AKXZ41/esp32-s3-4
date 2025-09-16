#ifndef _MPU6050_H_
#define _MPU6050_H_

#include "IIC.h"
#include "Data_declaration.h"

#define MPU6050_SENSOR_ADDR                 0x68        //MPU6050传感器地址
#define MPU6050_WHO_AM_I_REG_ADDR           0x75        //MPU6050传感器寄存器地址，通过读取这个地址来验证连接是否正常

void mpu6050_Init(void);//MPU6050传感器初始化
int mpu6050_read_data(uint8_t reg_addr,sensorData_t* sensorDataMPU6050);//读取mpu6050原始6轴数据

#endif /* COMPONENTS_MPU6050_INCLUDE_MPU6050_H_ */

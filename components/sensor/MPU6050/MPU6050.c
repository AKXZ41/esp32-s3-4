//陀螺仪

#include "MPU6050.h"
#include "UART.h"
#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"  // ESP-IDF 5.5中仍然使用原路径

INT16_XYZ ACC_OFFSET_RAW; 		//加速度零偏校准值
INT16_XYZ GYRO_OFFSET_RAW;		//陀螺仪零偏校准值

int MPU6050_OffSet(INT16_XYZ value,INT16_XYZ *offset,uint16_t sensivity);//零偏校准函数
int mpu6050_test(void);//mpu6050器件检测


int mpu6050_test(void){ //mpu6050器件检测
	uint8_t test[1] = {0};//接收数据缓冲区
	//读取0x75寄存器
	if(iic_read(I2C_MASTER_NUM,MPU6050_SENSOR_ADDR,0x75,test,1) == ESP_OK) printf("寄存器读取成功\n");
	//0x75寄存器的值与传感器地址对比一致则是成功连接，反之就是未连接
	if(test[0] == MPU6050_SENSOR_ADDR) {
		printf("MPU6050传感器连接 成功\n");
		return ESP_OK;
	}else{
		printf("MPU6050传感器连接 失败\n");
		return ESP_FAIL;
	}
}

void mpu6050_Init(void){ //mpu6050器件初始化
	printf("********************MPU6050初始化开始********************\n");
	if(mpu6050_test() == ESP_OK){//检测传感器成功连接就进行配置

		if(iic_write(I2C_MASTER_NUM,MPU6050_SENSOR_ADDR,0x6B,0x80) == ESP_OK) printf("1.复位MPU6050\n");
		vTaskDelay(pdMS_TO_TICKS(100));//等待100ms
		if(iic_write(I2C_MASTER_NUM,MPU6050_SENSOR_ADDR,0x6B,0x01) == ESP_OK) printf("2.唤醒MPU6050,并选择陀螺仪x轴PLL为时钟源\n");
		if(iic_write(I2C_MASTER_NUM,MPU6050_SENSOR_ADDR,0x38,0x00) == ESP_OK) printf("3.禁止中断\n");
		if(iic_write(I2C_MASTER_NUM,MPU6050_SENSOR_ADDR,0x1B,0x18) == ESP_OK) printf("4.禁陀螺仪满量程+-2000度/秒(最低分辨率 = 2^15/2000 = 16.4LSB/度/秒)止中断\n");
		if(iic_write(I2C_MASTER_NUM,MPU6050_SENSOR_ADDR,0x1C,0x08) == ESP_OK) printf("5.加速度满量程+-4g  (最低分辨率 = 2^15/4g = 8196LSB/g)\n");
		if(iic_write(I2C_MASTER_NUM,MPU6050_SENSOR_ADDR,0x1A,0x04) == ESP_OK) printf("6.设置陀螺的输出为1KHZ,DLPF=20HZ\n");
		if(iic_write(I2C_MASTER_NUM,MPU6050_SENSOR_ADDR,0x19,0x00) == ESP_OK) printf("7.采样分频(采样频率 = 陀螺仪输出频率 / (1+DIV), 采样频率1000hz)\n");
		if(iic_write(I2C_MASTER_NUM,MPU6050_SENSOR_ADDR,0x37,0x02) == ESP_OK) printf("8.MPU 可直接访问MPU6050辅助12C\n");
	}
	printf("********************MPU6050初始化完成********************\n\n");
}
//给入传感器原始数据结构体，更新mpu6050传感器数据，成功后返回ESP_OK
int mpu6050_read_data(uint8_t reg_addr,sensorData_t* sensorDataMPU6050){   //MPU6050读取姿态数据

	static uint8_t test[14] = {0};//接收数据缓冲区
	static uint16_t mpu6050_temp = 0;
	//每次从缓冲区读取一个字节，先读高字节，再读低字节，高字节左移八位，低位补0，再与低字节作或运算，即可得到一个寄存器（长度为2个字节）的完整数据
	if(iic_read(I2C_MASTER_NUM,MPU6050_SENSOR_ADDR,0x3B,test,14) == ESP_OK){

		sensorDataMPU6050->acc_n.X  = (test[0]  << 8 | test[1])  - ACC_OFFSET_RAW.X;
		sensorDataMPU6050->acc_n.Y  = (test[2]  << 8 | test[3])  - ACC_OFFSET_RAW.Y;
		sensorDataMPU6050->acc_n.Z  = (test[4]  << 8 | test[5])  - ACC_OFFSET_RAW.Z;
		mpu6050_temp      =           (test[6]  << 8 | test[7]);
		sensorDataMPU6050->gyro_n.X = (test[8]  << 8 | test[9])  - GYRO_OFFSET_RAW.X;
		sensorDataMPU6050->gyro_n.Y = (test[10] << 8 | test[11]) - GYRO_OFFSET_RAW.Y;
		sensorDataMPU6050->gyro_n.Z = (test[12] << 8 | test[13]) - GYRO_OFFSET_RAW.Z;

		mpu6050_temp = (36.53+mpu6050_temp/340)*10; //温度AD值换算为摄氏度

		//printf("MPU6050加速度原始数据: acc.x = %d acc.y = %d acc.z = %d\n",sensorData.acc_n.xx,sensorData.acc_n.yy,sensorData.acc_n.zz);
		//printf("MPU6050陀螺仪原始数据: gyro.x = %d gyro.y = %d gyro.z = %d\n",sensorData.gyro_n.xx,sensorData.gyro_n.yy,sensorData.gyro_n.zz);
		//printf("MPU6050陀螺仪原始数据: mpu6050_temp = %d\n",mpu6050_temp);
		//printf("MPU6050加速度原始数据: acc.x = %0.2f acc.y = %0.2f acc.z = %0.2f\n",sensorData.acc.x,sensorData.acc.y,sensorData.acc.z);
		//printf("MPU6050陀螺仪原始数据: gyro.x = %0.2f gyro.y = %0.2f gyro.z = %0.2f\n",sensorData.gyro.x,sensorData.gyro.y,sensorData.gyro.z);


		if(gyroscope_calibration == 1) { //陀螺仪零偏校准
			if(MPU6050_OffSet(sensorDataMPU6050->gyro_n,&GYRO_OFFSET_RAW,0)) gyroscope_calibration = 2;//校准完成后切换加速度校准
		}
		if(Acceleration_calibration == 1){ //加速度零偏校准
			if(MPU6050_OffSet(sensorDataMPU6050->acc_n,&ACC_OFFSET_RAW,8196)) Acceleration_calibration = 2;//校准完成后切换3也就全部校准完成
		  }

		return ESP_OK;
	}else{
		return ESP_FAIL;
	}
}

//零偏校准函数
int MPU6050_OffSet(INT16_XYZ value,INT16_XYZ *offset,uint16_t sensivity){
  static int32_t tempgx=0,tempgy=0,tempgz=0;
  static uint16_t cnt_a=0;
  if(cnt_a==0)
  {
    value.X=0;
    value.Y=0;
    value.Z=0;
    tempgx = 0;
    tempgy = 0;
    tempgz = 0;
    cnt_a = 1;
    offset->X = 0;
    offset->Y = 0;
    offset->Z = 0;
  }
  tempgx += value.X;
  tempgy += value.Y;
  tempgz += value.Z - sensivity;
  if(cnt_a==200)
  {
    offset->X= tempgx / cnt_a;
    offset->Y= tempgy / cnt_a;
    offset->Z= tempgz / cnt_a;
    cnt_a = 0;
    return 1;
  }
  cnt_a++;
  return 0;
}



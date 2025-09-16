#include "control.h"
#include "UART.h"
#include "MPU6050.h"
#include "IMU.h"
#include "anotc.h"
#include "LED.h"
#include "driver/gpio.h"
#include "WIFI.h"
#include "UDP_TCP.h"
#include <stdio.h>
#include <stdbool.h>
// #include <unistd.h>  // ESP-IDF 5.5中已弃用
// #include <sys/time.h>  // ESP-IDF 5.5中已弃用，使用esp_timer.h替代
#include "esp_timer.h"
#include <inttypes.h>
#include "Data_declaration.h"
#include "remote_control.h"
#include "PWM.h"
#include "VBAT.h"
#include "SPI.h"
#include "SPL06-001.h"



void app_main(void)//入口函数一开始就执行一次
{

	uart1_init(115200);//初始化串口波特率115200
	IIC_init();//IIC初始化
	PWM_init();//PWM初始化
	VBAT_init();//电池电压检测初始化

	mpu6050_Init();//MPU6050姿态传感器初始化
	SPL06Init(); //气压计初始化
	LED_init();//LED灯初始化

	wifi_init_ap();//初始化WiFi
	UDP_init();//UDP通信初始化
	anotc_Init();//匿名上位机初始化
	remote_control_init();//遥控器初始化

	control_init();//姿态控制初始化

	printf("剩余空闲堆栈: %d 字节\n", (int)esp_get_free_heap_size());	//打印剩余堆栈大小

	init_ok = true;//初始化完成置1让所有任务开始运行

}

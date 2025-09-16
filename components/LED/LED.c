 //数据声明，结构体，堆栈空间，飞行器姿态和PID各类数据在此声明

#include "LED.h"

#include "Data_declaration.h"//全局变量存储地

#include <stdio.h>//兼容
#include <stdbool.h>//兼容
// #include <unistd.h>  // ESP-IDF 5.5中已弃用

#include "freertos/FreeRTOS.h"//RTOS头文件
#include "freertos/task.h"    //RTOS头文件


void led_Task(void *pvParameter)
{
	bool temp_1 = false;
	int temp_val_1 = 0;
	TickType_t adp = xTaskGetTickCount();
	const TickType_t adg = 50;
	while(1){
		vTaskDelayUntil(&adp,adg);
		if(init_ok){

			//*************黄灯*********************
			if(VBAT <= 380){//如果电池电压小于3.8v就闪烁警告
			temp_1 = !temp_1; //通过循环取反达到闪烁的效果
			gpio_set_level(LED2, temp_1);
			}else{
				gpio_set_level(LED2, 0);//电池电量大于3.8v就常亮
			}
			//*************黄灯*********************


			//*************绿灯*********************
			if(temp_val_1 >= 100){
				temp_val_1 = 0;
				if(state.rc_link == true) gpio_set_level(LED1, 0);  //如果连接上灯亮
				else gpio_set_level(LED1, 1);                       //如果未连接灯灭
				state.rc_link = false;                              //重置状态位
			}else{
				temp_val_1++;
			}
			//*************绿灯*********************


			//*************蓝灯*********************






			//*************蓝灯*********************

		}
	}
}

//LED初始化
void LED_init(void){


	gpio_config_t led_conf;//创建GPIO结构体

	led_conf.mode = GPIO_MODE_OUTPUT;		    	 // 配置gpio为输出模式
	led_conf.intr_type = GPIO_INTR_DISABLE; 	 // 禁止中断
	led_conf.pin_bit_mask = 1ULL << LED1;      		 // 配置GPIO引脚
	led_conf.pull_down_en = 0;                 	 	 // 禁止失能
	led_conf.pull_up_en = 0;                   	 	 // 禁止失能

	gpio_config(&led_conf);                    	 	 // 配置LED1的GPIO参数

	led_conf.pin_bit_mask = 1ULL << LED2;			 // 配置GPIO引脚
	gpio_config(&led_conf);              			 // 配置LED2的GPIO参数

	led_conf.pin_bit_mask = 1ULL << LED3;			 // 配置GPIO引脚
	gpio_config(&led_conf);              			 // 配置LED2的GPIO参数

	gpio_set_level(LED1, 1);
	gpio_set_level(LED2, 1);
	gpio_set_level(LED3, 1);
	vTaskDelay(pdMS_TO_TICKS(500));//延时

	gpio_set_level(LED3, 0);
	vTaskDelay(pdMS_TO_TICKS(500));//延时
	gpio_set_level(LED1, 0);
	vTaskDelay(pdMS_TO_TICKS(500));//延时
	gpio_set_level(LED2, 0);
	vTaskDelay(pdMS_TO_TICKS(500));//延时

	gpio_set_level(LED1, 1);
	gpio_set_level(LED2, 1);
	gpio_set_level(LED3, 1);
	vTaskDelay(pdMS_TO_TICKS(1000));//延时

	xTaskCreate(led_Task, "led_Task", 1024 * 5, NULL, 3, NULL); //创建任务，判断状态改变灯的亮灭

}




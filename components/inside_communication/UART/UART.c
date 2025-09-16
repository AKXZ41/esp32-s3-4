#include "UART.h"

void uart1_init(int baud) {              			//串口初始化

	printf("*******************串口初始化开始*******************\n");
    const uart_config_t uart_config = {   			//串口参数结构体
        .baud_rate = baud,               			//波特率
        .data_bits = UART_DATA_8_BITS,	 			//数据位设置为8位
        .parity = UART_PARITY_DISABLE,				//禁用奇偶校验
        .stop_bits = UART_STOP_BITS_1,				//停止位设置为1位
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,		//流量控制模式禁用（CTS/RTS）
        .source_clk = UART_SCLK_APB,				//设置时钟源
    };
	printf("串口号: %d\n",UART_NUM_0);
	printf("波特率: %d\n",uart_config.baud_rate);
	printf("数据位8位\n");
	printf("禁用奇偶校验\n");
	printf("停止位1位\n");
	printf("流量控制模式禁用\n");
	printf("接收缓冲区大小:%d\n",RX_BUF_SIZE * 2);
	printf("发送缓冲区大小:0\n");
	printf("串口事件队列大小:0\n");
	printf("TXD引脚:%d\n",TXD_PIN);
	printf("RXD引脚:%d\n",RXD_PIN);

    if(uart_driver_install(							//安装串口驱动
    					UART_NUM_0,					//串口号
						RX_BUF_SIZE * 2,		 	//接收缓冲区
						0,      					//发送缓冲区
						0,       					//串口事件队列大小
						NULL,   					//串口句柄
						0      					  	//用于分配中断的标志
						) == ESP_OK) printf("串口驱动安装 成功\n");
    else printf("串口驱动安装 失败\n");

    if(uart_param_config(         						//配置参数
    				  UART_NUM_0,					//串口号
					  &uart_config					//串口参数结构体传入
					  ) == ESP_OK)printf("串口参数配置 成功\n");
    else printf("串口参数配置 失败\n");

    if(uart_set_pin(         							//设置引脚
    			 UART_NUM_0,      					//串口号
    			 TXD_PIN, 							//发送引脚
				 RXD_PIN, 							//接收引脚
				 UART_PIN_NO_CHANGE,				//CTS
				 UART_PIN_NO_CHANGE 				//RTS
				 ) == ESP_OK) printf("串口引脚设置 成功\n");
    else printf("串口引脚设置 失败\n");
    printf("*******************串口初始化结束*******************\n");

}

int sendData( const char* data)
{
    const int len = strlen(data);
    const int txBytes = uart_write_bytes(UART_NUM_0, data, len);
    //ESP_LOGI(logName, "Wrote %d bytes", txBytes);
    return txBytes;
}

void tx_task(void *arg)//发送任务
{

    //static const char *TX_TASK_TAG = "TX_TASK";
    //esp_log_level_set(TX_TASK_TAG, ESP_LOG_INFO);
    while (1) {
        sendData("设置");
        sendData("\n");
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void rx_task(void *arg)//接收任务
{
    static const char *RX_TASK_TAG = "RX_TASK";
    esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
    uint8_t* data = (uint8_t*) malloc(RX_BUF_SIZE+1);
    while (1) {
        const int rxBytes = uart_read_bytes(UART_NUM_0, data, RX_BUF_SIZE, pdMS_TO_TICKS(1000));
        if (rxBytes > 0) {
        	uart_write_bytes(UART_NUM_0, data, rxBytes);
            data[rxBytes] = 0;
            //sendData(data);
            //ESP_LOGI(RX_TASK_TAG, "Read %d bytes: '%s'", rxBytes, data);
            //ESP_LOG_BUFFER_HEXDUMP(RX_TASK_TAG, data, rxBytes, ESP_LOG_INFO);
        }
    }
    free(data);
}




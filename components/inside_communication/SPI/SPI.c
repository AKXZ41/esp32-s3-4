#include "SPI.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"  // ESP-IDF 5.5中仍然使用原路径
#include "driver/gpio.h"  // ESP-IDF 5.5中仍然使用原路径
#include "sdkconfig.h"
#include "esp_log.h"

#define SI24R1_MISO    21
#define SI24R1_MOSI    14
#define SI24R1_CLK     13
#define SI24R1_CS      12

spi_device_handle_t SI24R1_spi; //创建SI24R1专属的SPI句柄



//向SI24R1向指定寄存器发送数据       寄存器地址   要写入的数据（数组）   写入多少位
int SI24R1_Write_Buf(uint8_t reg_addr, uint8_t *data, uint8_t len)
{
	uint8_t addr[1] = {reg_addr};//将寄存器地址转换

    gpio_set_level(SI24R1_CS, 0);//将CS引脚设置为低电平选中SPI从设备
    spi_transaction_t t = {       //声明SPI数据传输结构体
    	.length=8,                //设置发送长度8bit
    	.tx_buffer=addr,          //传入发送数据的指针
    	.user=(void*)1,           //D/C needs to be set to 1（意义不明）
    };
    if(spi_device_polling_transmit(SI24R1_spi, &t) != ESP_OK) goto esp_fail;  //开始传输

    t.length=len*8;                 //设置数据发送长度（*8是字节转换为bit）
    t.tx_buffer=data;               //数据指针
    t.user=(void*)1;                //D/C needs to be set to 1 （意义不明）
    if(spi_device_polling_transmit(SI24R1_spi, &t)!= ESP_OK) goto esp_fail; //开始传输

    gpio_set_level(SI24R1_CS, 1);//将CS引脚设置为高电平结束传输

    return ESP_OK;

    esp_fail:
	gpio_set_level(SI24R1_CS, 1);//将CS引脚设置为高电平结束传输
    return ESP_FAIL;
}

int SI24R1_Write_Reg(uint8_t reg_addr, uint8_t data){

	uint8_t transition[1] = {data};//将寄存器地址转换

	return SI24R1_Write_Buf(reg_addr,transition,1);

}

int SI24R1_Read_Buf(uint8_t reg_addr, uint8_t *data, uint8_t len)
{
	uint8_t addr[1] = {reg_addr};
	gpio_set_level(SI24R1_CS, 0);
	spi_transaction_t t = {
	   .length=8,                 //设置数据发送长度（单位bit）
	   .tx_buffer=addr,           //数据指针
	   .user=(void*)1,            //D/C needs to be set to 1 （意义不明）
	};
	if(spi_device_polling_transmit(SI24R1_spi, &t)!= ESP_OK) goto esp_fail;  //Transmit!

    t.flags = SPI_TRANS_USE_RXDATA;
    for(int k = 0 ; k < len ; k++){//循环接收（因为缓冲区最大才4个字节）
    	if(spi_device_polling_transmit(SI24R1_spi, &t)!= ESP_OK) goto esp_fail;
    	data[k] = t.rx_data[0];
    }

    gpio_set_level(SI24R1_CS, 1);

    return ESP_OK;

    esp_fail:
	gpio_set_level(SI24R1_CS, 1);//将CS引脚设置为高电平结束传输
    return ESP_FAIL;
}

int SI24R1_Read_Reg(uint8_t reg_addr, uint8_t *data){

	uint8_t transition[1] = {0x00};//将寄存器地址转换

	int err =SI24R1_Read_Buf(reg_addr,transition,1);

	*data = transition[0];
	return err;
}


void spi_init(void){

	    printf("********************SPI初始化开始********************\n");
	    spi_bus_config_t buscfg={//配置SPI引脚信息
	        .miso_io_num = SI24R1_MISO,                // MISO信号线
	        .mosi_io_num = SI24R1_MOSI,                // MOSI信号线
	        .sclk_io_num = SI24R1_CLK,                 // SCLK信号线
	        .quadwp_io_num = -1,                        // WP信号线，专用于QSPI的D2
	        .quadhd_io_num = -1,                        // HD信号线，专用于QSPI的D3
	        .max_transfer_sz = 64*8,                    // 最大传输数据大小
	    };

	    spi_device_interface_config_t devcfg={
	        .clock_speed_hz = SPI_MASTER_FREQ_10M,      // 时钟频率10mhz
	        .mode = 0,                                  // SPI mode 0
	        .spics_io_num = -1,
	        .queue_size = 7,                            // 传输队列大小，决定了等待传输数据的数量
	    };

	    printf("SPI总线: %d\n",SPI2_HOST+1);
	    printf("SPI模式为%s\n",devcfg.mode == 0 ? "从机模式" : "主机模式" );
	    printf("SPI主设备数据输入MISO引脚: %d\n",buscfg.miso_io_num );
	    printf("SPI主设备数据输出MOSI引脚: %d\n",buscfg.mosi_io_num);
	    printf("SPI主设备时钟CLK引脚引脚: %d\n",buscfg.sclk_io_num);
	    printf("SPI主设备信号线WP引脚: %d\n",buscfg.quadwp_io_num);
	    printf("SPI主设备信号线HD引脚: %d\n",buscfg.quadhd_io_num);
	    printf("SPI时钟速率: %d\n",devcfg.clock_speed_hz);
	    printf("SPI传输队列大小: %d\n",devcfg.queue_size);
	    printf("SPI最大传输数据大小: %d\n",buscfg.max_transfer_sz);

	    //初始化SPI总线
	    if(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO) == ESP_OK) printf("SPI总线初始化成功\n");
	    else printf("SPI总线初始化失败\n");
	    if(spi_bus_add_device(SPI2_HOST, &devcfg, &SI24R1_spi) == ESP_OK) printf("在 SPI 总线上分配设备成功\n");
	    else printf("在 SPI 总线上分配设备失败\n");

	    gpio_reset_pin(SI24R1_CS);                      // 重置GPIO引脚配置
	    gpio_set_direction(SI24R1_CS, GPIO_MODE_OUTPUT);// 把这个GPIO作为输出

	    gpio_set_level(SI24R1_CS, 1);
	    printf("********************SPI初始化完成********************\n\n");
}

//电池电压检测

#include "VBAT.h"

#include <stdio.h>
#include <stdbool.h>
// #include <unistd.h>  // ESP-IDF 5.5中已弃用
#include <stdlib.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"  // ESP-IDF 5.5中ADC头文件路径
#include "esp_adc/adc_cali.h"  // ESP-IDF 5.5中ADC校准头文件路径
#include "esp_adc/adc_cali_scheme.h"  // ESP-IDF 5.5中ADC校准方案头文件路径
#include "Data_declaration.h"

// ADC所接的通道  GPIO4 = ADC1_CHANNEL_3
#define ADC1_TEST_CHANNEL ADC_CHANNEL_3  //GPIO4
// ADC校准句柄
static adc_cali_handle_t adc1_cali_handle = NULL;
// ADC句柄
static adc_oneshot_unit_handle_t adc1_handle;


static bool adc_calibration_init(void)
{
    esp_err_t ret = ESP_FAIL;
    bool calibrated = false;

#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    if (!calibrated) {
        ESP_LOGI("VBAT", "calibration scheme version is Curve Fitting");
        adc_cali_curve_fitting_config_t cali_config = {
            .unit_id = ADC_UNIT_1,
            .atten = ADC_ATTEN_DB_12,
            .bitwidth = ADC_BITWIDTH_12,
        };
        ret = adc_cali_create_scheme_curve_fitting(&cali_config, &adc1_cali_handle);
        if (ret == ESP_OK) {
            calibrated = true;
        }
    }
#endif

#if ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    if (!calibrated) {
        ESP_LOGI("VBAT", "calibration scheme version is Line Fitting");
        adc_cali_line_fitting_config_t cali_config = {
            .unit_id = ADC_UNIT_1,
            .atten = ADC_ATTEN_DB_11,
            .bitwidth = ADC_BITWIDTH_12,
        };
        ret = adc_cali_create_scheme_line_fitting(&cali_config, &adc1_cali_handle);
        if (ret == ESP_OK) {
            calibrated = true;
        }
    }
#endif

    return calibrated;
}


static void VBAT_task(void *pvParameters){
	int read_raw;
	int voltage = 0;
	TickType_t adp = xTaskGetTickCount();
	const TickType_t adg = pdMS_TO_TICKS(100);//这里的数是指ticks（时间片）的意思，等于1就是每个ticks中断都执行
	while(1){
		vTaskDelayUntil(&adp,adg);
		if(init_ok)
		{
			// 采集ADC原始值
			adc_oneshot_read(adc1_handle, ADC1_TEST_CHANNEL, &read_raw);
			
			// 校准ADC值
			if (adc1_cali_handle) {
				adc_cali_raw_to_voltage(adc1_cali_handle, read_raw, &voltage);
				VBAT = voltage * 2 / 10; // 分压器比例
			} else {
				// 如果没有校准，使用原始值进行简单转换
				VBAT = read_raw * 3300 / 4095 * 2 / 10;
			}
			//printf("ADC原始值: %d   转换电压值: %dmV\n", read_raw, VBAT);
		}
	}
}

void VBAT_init(void)
{
	// 初始化ADC校准
	bool cali_enable = adc_calibration_init();
	if (cali_enable) {
		printf("ADC校准初始化成功\n");
	} else {
		printf("ADC校准初始化失败，将使用原始值\n");
	}

	// 配置ADC1
	adc_oneshot_unit_init_cfg_t init_config1 = {
		.unit_id = ADC_UNIT_1,
	};
	adc_oneshot_new_unit(&init_config1, &adc1_handle);

	// 配置ADC通道
	adc_oneshot_chan_cfg_t config = {
		.bitwidth = ADC_BITWIDTH_12,
		.atten = ADC_ATTEN_DB_12,
	};
	adc_oneshot_config_channel(adc1_handle, ADC1_TEST_CHANNEL, &config);

	xTaskCreate(VBAT_task, "VBAT_task", 1024*2, NULL, 3, NULL);//创建任务不断循环获取电池电压
}




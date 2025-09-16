#include "PWM.h"

#include <stdio.h>
#include "driver/ledc.h"  // ESP-IDF 5.5中仍然使用原路径   
#include "esp_err.h"

// 定义PWM输出GPIO
#define PWM1_GPIO          (42)
#define PWM2_GPIO          (6)
#define PWM3_GPIO          (15)
#define PWM4_GPIO          (38)
// 定义PWM定时器
#define PWM1_TIMER              LEDC_TIMER_0
#define PWM2_TIMER              LEDC_TIMER_1
#define PWM3_TIMER              LEDC_TIMER_2
#define PWM4_TIMER              LEDC_TIMER_3
// 定义PWM通道
#define PWM1_CHANNEL            LEDC_CHANNEL_0
#define PWM2_CHANNEL            LEDC_CHANNEL_1
#define PWM3_CHANNEL            LEDC_CHANNEL_2
#define PWM4_CHANNEL            LEDC_CHANNEL_3

#define Moto_PwmMax 1000
#define PWM_MODE                LEDC_LOW_SPEED_MODE  //设置PWM模式为低速

//控制空心杯
#define PWM_RESOLUTION_RATIO    LEDC_TIMER_8_BIT   // 设置分辨率8位
#define PWM_FREQUENCY           (5000)            // 频率单位为赫兹。频率设定为5KHZ

//计算不同分辨率下占空比的最大值例如13位分辨率  ((2 ** 13) - 1)  = 8191  2的13次方（13是分辨率）减去1 就是当前分辨率下的最大占空比数值

void PWM_init(void)
{
    //PWM1定时器配置
    ledc_timer_config_t PWM1_timer = {
        .speed_mode       = PWM_MODE, //设置为低速模式
        .timer_num        = PWM1_TIMER,//选择定时器0
        .duty_resolution  = PWM_RESOLUTION_RATIO,//设置分辨率为13位
        .freq_hz          = PWM_FREQUENCY,  // 设置输出频率为5khz
        .clk_cfg          = LEDC_AUTO_CLK//时钟源配置为自动
    };
    ESP_ERROR_CHECK(ledc_timer_config(&PWM1_timer));

    //PWM2定时器配置
    ledc_timer_config_t PWM2_timer = {
        .speed_mode       = PWM_MODE, //设置为低速模式
        .timer_num        = PWM2_TIMER,//选择定时器0
        .duty_resolution  = PWM_RESOLUTION_RATIO,//设置分辨率为13位
        .freq_hz          = PWM_FREQUENCY,  // 设置输出频率为5khz
        .clk_cfg          = LEDC_AUTO_CLK//时钟源配置为自动
    };
    ESP_ERROR_CHECK(ledc_timer_config(&PWM2_timer));

    //PWM3定时器配置
    ledc_timer_config_t PWM3_timer = {
        .speed_mode       = PWM_MODE, //设置为低速模式
        .timer_num        = PWM3_TIMER,//选择定时器0
        .duty_resolution  = PWM_RESOLUTION_RATIO,//设置分辨率为13位
        .freq_hz          = PWM_FREQUENCY,  // 设置输出频率为5khz
        .clk_cfg          = LEDC_AUTO_CLK//时钟源配置为自动
    };
    ESP_ERROR_CHECK(ledc_timer_config(&PWM3_timer));

    //PWM4定时器配置
    ledc_timer_config_t PWM4_timer = {
        .speed_mode       = PWM_MODE, //设置为低速模式
        .timer_num        = PWM4_TIMER,//选择定时器0
        .duty_resolution  = PWM_RESOLUTION_RATIO,//设置分辨率为13位
        .freq_hz          = PWM_FREQUENCY,  // 设置输出频率为5khz
        .clk_cfg          = LEDC_AUTO_CLK//时钟源配置为自动
    };
    ESP_ERROR_CHECK(ledc_timer_config(&PWM4_timer));

    // PWM1通道配置
    ledc_channel_config_t PWM1_channel = {
        .speed_mode     = PWM_MODE,//设置为低速模式
        .channel        = PWM1_CHANNEL,//PWM通道
        .timer_sel      = PWM1_TIMER,//选择定时器
        .intr_type      = LEDC_INTR_DISABLE,//禁用中断
        .gpio_num       = PWM1_GPIO,//设置io引脚
        .duty           = 0, // 设置占空比为0
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&PWM1_channel));

    // PWM1通道配置
    ledc_channel_config_t PWM2_channel = {
        .speed_mode     = PWM_MODE,//设置为低速模式
        .channel        = PWM2_CHANNEL,//PWM通道
        .timer_sel      = PWM2_TIMER,//选择定时器
        .intr_type      = LEDC_INTR_DISABLE,//禁用中断
        .gpio_num       = PWM2_GPIO,//设置io引脚
        .duty           = 0, // 设置占空比为0
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&PWM2_channel));

    // PWM1通道配置
    ledc_channel_config_t PWM3_channel = {
        .speed_mode     = PWM_MODE,//设置为低速模式
        .channel        = PWM3_CHANNEL,//PWM通道
        .timer_sel      = PWM3_TIMER,//选择定时器
        .intr_type      = LEDC_INTR_DISABLE,//禁用中断
        .gpio_num       = PWM3_GPIO,//设置io引脚
        .duty           = 0, // 设置占空比为0
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&PWM3_channel));

    // PWM1通道配置
    ledc_channel_config_t PWM4_channel = {
        .speed_mode     = PWM_MODE,//设置为低速模式
        .channel        = PWM4_CHANNEL,//PWM通道
        .timer_sel      = PWM4_TIMER,//选择定时器
        .intr_type      = LEDC_INTR_DISABLE,//禁用中断
        .gpio_num       = PWM4_GPIO,//设置io引脚
        .duty           = 0, // 设置占空比为0
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&PWM4_channel));
}

void Moto_Pwm(float MOTO1_PWM,float MOTO2_PWM,float MOTO3_PWM,float MOTO4_PWM){

	//限制数值在0-1000
	if(MOTO1_PWM>1000.0f)	    MOTO1_PWM = 1000.0f;
	if(MOTO2_PWM>1000.0f)	    MOTO2_PWM = 1000.0f;
	if(MOTO3_PWM>1000.0f)	    MOTO3_PWM = 1000.0f;
	if(MOTO4_PWM>1000.0f)	    MOTO4_PWM = 1000.0f;
	if(MOTO1_PWM<0.0f)			MOTO1_PWM = 0.0f;
	if(MOTO2_PWM<0.0f)			MOTO2_PWM = 0.0f;
	if(MOTO3_PWM<0.0f)			MOTO3_PWM = 0.0f;
	if(MOTO4_PWM<0.0f)			MOTO4_PWM = 0.0f;


	//往对应通道写入数值   控制空心杯的程序
	ledc_set_duty(PWM_MODE, PWM1_CHANNEL, MOTO1_PWM*0.255f);
	ledc_set_duty(PWM_MODE, PWM2_CHANNEL, MOTO2_PWM*0.255f);
	ledc_set_duty(PWM_MODE, PWM3_CHANNEL, MOTO3_PWM*0.255f);
	ledc_set_duty(PWM_MODE, PWM4_CHANNEL, MOTO4_PWM*0.255f);

	//刷新各通道的数值
	ledc_update_duty(PWM_MODE, PWM1_CHANNEL);
	ledc_update_duty(PWM_MODE, PWM2_CHANNEL);
	ledc_update_duty(PWM_MODE, PWM3_CHANNEL);
	ledc_update_duty(PWM_MODE, PWM4_CHANNEL);
}

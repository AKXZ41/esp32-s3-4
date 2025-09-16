#include "WIFI.h"
//#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"



#include <stdio.h>
#include <stdbool.h>
// #include <unistd.h>  // ESP-IDF 5.5中已弃用

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
// #include <sys/param.h>  // ESP-IDF 5.5中已弃用

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/portmacro.h"
#include "nvs_flash.h"

#define DEBUG_MODULE "APP_MAIN"

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>



static char AP_WIFI_SSID[32] = "esp32-4"; //AP模式下创建的WiFi名称
static char AP_WIFI_PWD[64] = "12345678" ;  //AP模式下创建的WiFi密码
#define MAX_STA_CONN (2)

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
	if (event_id == WIFI_EVENT_AP_STACONNECTED) {//有设备连接
	        wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *) event_data;//获取连接设备信息信息
	        printf("有设备连接，mac地址:%02x:%02x:%02x:%02x:%02x:%02x, AID=%d\n",
	               event->mac[0], event->mac[1], event->mac[2], 
	               event->mac[3], event->mac[4], event->mac[5], event->aid);

	    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {//有设备断开
	        wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t *) event_data; //获取断开设备的信息
	        printf("有设备断开，mac地址:%02x:%02x:%02x:%02x:%02x:%02x, AID=%d\n",
	               event->mac[0], event->mac[1], event->mac[2], 
	               event->mac[3], event->mac[4], event->mac[5], event->aid);
	    }
}


void wifi_init_ap(void) //AP模式初始化
{
	printf("\n\n********************WiFi初始化开始配置为AP模式********************\n");

	printf("默认NVS分区初始化 ");//初始化默认 NVS 分区
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	if(ret == ESP_OK) printf("成功\n");
	else printf("失败\n");

	printf("初始化基础 TCP/IP 堆栈 ");//初始化基础 TCP/IP 堆栈。
	if(esp_netif_init() == ESP_OK) printf("成功\n");//初始化基础 TCP/IP 堆栈。
	else printf("失败\n");

    printf("创建默认事件循环 ");//创建默认事件循环
    if(esp_event_loop_create_default() == ESP_OK) printf("成功\n");//创建默认事件循环
    else printf("失败\n");

    printf("创建默认的无线网络接入点 成功\n");
    esp_netif_t *ap_netif = NULL;//存储 esp_netif_create_default_wifi_ap 函数返回的指向esp-netif实例指针
    ap_netif = esp_netif_create_default_wifi_ap();//创建默认的无线网络接入点

    printf("*****************************开始初始化WiFi******************************\n");
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT(); //获取简要的WiFi栈配置参数，传递给esp_wifi_init调用
    // ESP-IDF 5.5中WIFI_INIT_CONFIG_DEFAULT()已弃用，但暂时保留
    if(esp_wifi_init(&cfg) == ESP_OK) printf("成功\n");//初始化无线网络 为无线驱动程序分配资源，例如无线控制结构，RX / TX缓冲区，无线NVS结构等。此无线网络也会启动无线网络任务
    else printf("失败\n");
	printf("*****************************初始化WiFi完成*******************************\n");

	printf("将事件处理程序的实例注册到默认循环");
	if(esp_event_handler_instance_register(WIFI_EVENT,//要为其注册处理程序的事件的基本 ID
	                ESP_EVENT_ANY_ID,//要为其注册处理程序的事件的 ID
	                &wifi_event_handler,//在调度事件时调用的处理程序函数
	                NULL,//数据，除了事件数据之外，在调用处理程序时传递给处理程序
	                NULL) == ESP_OK) printf("成功\n");//与已注册的事件处理程序和数据相关的事件处理程序实例对象可以为 NULL。如果在删除整个事件循环之前应取消注册特定的回调实例，则需要保留此项。可以多次注册相同的事件处理程序，并生成不同的实例对象。所有注册的数据都可以相同。如果不需要取消注册，但在删除事件循环时应删除处理程序，则实例可以为 NULL
	else printf("失败\n");

	uint8_t mac[6];//mac地址存储变量
	if(esp_wifi_get_mac(ESP_IF_WIFI_AP, mac) == ESP_OK) printf("mac地址获取成功 %02X %02X %02X %02X %02X %02X \n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);//获取指定接口的mac地址
	else printf("mac地址获取失败\n");

	printf("配置WiFi\n");
	wifi_config_t wifi_config;//WiFi配置结构体
	memcpy(wifi_config.ap.ssid, AP_WIFI_SSID, strlen(AP_WIFI_SSID) + 1) ;//WiFi名称
	wifi_config.ap.ssid_len = strlen(AP_WIFI_SSID);//WiFi名称字符长度
	memcpy(wifi_config.ap.password, AP_WIFI_PWD, strlen(AP_WIFI_PWD) + 1) ;//WiFi密码
	wifi_config.ap.max_connection = MAX_STA_CONN;//最大连接数
	wifi_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;//AP认证模式
	wifi_config.ap.channel  = 13;//AP通道

	printf("WiFi名称:%s\n",AP_WIFI_SSID);
	printf("WiFi密码:%s\n",AP_WIFI_PWD);
	printf("最大连接数:%d\n",MAX_STA_CONN);

	if (strlen(AP_WIFI_PWD) == 0) wifi_config.ap.authmode = WIFI_AUTH_OPEN; //WiFi密码等于0 AP认证模式转为公开

	printf("设置无线操作模式为AP ");
	if(esp_wifi_set_mode(WIFI_MODE_AP) == ESP_OK) printf("成功\n");//设置无线操作模式为AP
	else printf("失败\n");

	printf("设置WiFi接入点配置 \n");
	if(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config) == ESP_OK) printf("成功\n");//设置 ESP32 标准或接入点的配置
	else printf("失败\n");

	printf("******************************WiFi启动******************************\n");
	if(esp_wifi_start() == ESP_OK) printf("成功\n");//根据当前配置启动WiFi
	else printf("失败\n");
	printf("******************************WiFi启动完成******************************\n");

	esp_netif_ip_info_t ip_info = {//IPV4地址结构体
			.ip.addr = ipaddr_addr("192.168.43.42"),//IPV4地址
			.netmask.addr = ipaddr_addr("255.255.255.0"),//IPV4子掩码
			.gw.addr      = ipaddr_addr("192.168.43.42"),//IPV4网关地址
	};

	printf("DHCP 服务器停止 ");
	if(esp_netif_dhcps_stop(ap_netif) == ESP_OK) printf("成功\n");//停止 DHCP 服务器
	else printf("失败\n");

	ret = esp_netif_set_ip_info(ap_netif, &ip_info);//设置IP地址
	if(ret == ESP_OK) printf("IP地址设置成功\nIPV4地址:192.168.43.42 \nIPV4子掩码:255.255.255.0 \nIPV4网关地址:192.168.43.42 \n");
	else printf("IP地址设置失败\n");

	printf("DHCP 服务器启动 ");
	if(esp_netif_dhcps_start(ap_netif) == ESP_OK) printf("成功\n");//启动 DHCP 服务器
	else printf("失败\n");

}



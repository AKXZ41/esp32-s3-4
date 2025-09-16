#include "UDP_TCP.h"

#include <stdbool.h>
// #include <unistd.h>  // ESP-IDF 5.5中已弃用
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "Data_declaration.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"
#include <lwip/netdb.h>

// #include <sys/param.h>  // ESP-IDF 5.5中已弃用
#include "esp_netif.h"
#include "anotc.h"
#include "remote_control.h"


static void udp_anotc_read_task(void *pvParameters); //匿名地面站接收任务
static void udp_rc_read_task(void *pvParameters); //遥控器接收任务

struct sockaddr_storage source_addr_anotc; //地面站套接字地址存储变量足以支持IPV6
struct sockaddr_in6 dest_addr_anotc; //地面站套接字地址
socklen_t socklen_anotc = sizeof(source_addr_anotc);//计算地面站存储变量大小

char rx_buffer_anotc[128];//地面站接收缓冲区
char addr_str_anotc[128]; //地面站设备地址

int sock_anotc; //地面站套接字
int anotc_PORT = 3333; //地面站端口
bool anotc_state = false; //地面站连接状态

struct sockaddr_storage source_addr_rc; //遥控器套接字地址存储变量足以支持IPV6
struct sockaddr_in6 dest_addr_rc; //遥控器套接字地址
socklen_t socklen_rc = sizeof(source_addr_rc);//计算遥控器存储变量大小

char rx_buffer_rc[128];//遥控器接收缓冲区
char addr_str_rc[128]; //遥控器设备地址

int sock_rc; //遥控器套接字
int rc_PORT = 5555; // 遥控器端口

int addr_family = 2;
int ip_protocol = 0;
bool rc_state = false; //遥控器连接状态





static void udp_anotc_read_task(void *pvParameters)//地面站DUP数据接送任务
{
    while (1) {
        while (1) {
            int len = recvfrom(sock_anotc, rx_buffer_anotc, sizeof(rx_buffer_anotc) - 1, 0, (struct sockaddr *)&source_addr_anotc, &socklen_anotc);//接送数据
            if (len < 0) {//数小于0就是接收错误
            	printf("接收失败\n");
            	break;//跳出这个while循环关闭套接字并重新启动
            }
            else {//如果成功接收就获取发件人的ip地址为字符串
                inet_ntoa_r(((struct sockaddr_in *)&source_addr_anotc)->sin_addr, addr_str_anotc, sizeof(addr_str_anotc) - 1);
                anotc_data_decode (rx_buffer_anotc);
                //printf("成功接收地面站数据\n");
            }
        }
        if (sock_anotc != -1) {
        	printf("关闭套接字并重新启动\n");
        	shutdown(sock_anotc, 0);
        	close(sock_anotc);
        }
    }
    vTaskDelete(NULL);
}

static void udp_rc_read_task(void *pvParameters)//地面站DUP数据接送任务
{
    while (1) {
        while (1) {
            int len = recvfrom(sock_rc, rx_buffer_rc, sizeof(rx_buffer_rc) - 1, 0, (struct sockaddr *)&source_addr_rc, &socklen_rc);//接送数据
            if (len < 0) {//数小于0就是接收错误
            	printf("接收失败\n");
            	break;//跳出这个while循环关闭套接字并重新启动
            }
            else {//如果成功接收就获取发件人的ip地址为字符串
                inet_ntoa_r(((struct sockaddr_in *)&source_addr_rc)->sin_addr, addr_str_rc, sizeof(addr_str_rc) - 1);
                rc_data_decode (rx_buffer_rc);
                //printf("收到数据\n");

            }
        }
        if (sock_rc != -1) {
        	printf("关闭套接字并重新启动\n");
        	shutdown(sock_rc, 0);
        	close(sock_rc);
        }
    }
    vTaskDelete(NULL);
}

void socket_anotc_init(void){//地面站套接字初始化

	if (addr_family == AF_INET) {//IPV4
	            struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)&dest_addr_anotc;
	            dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);//ip地址
	            dest_addr_ip4->sin_family = AF_INET;//套接字要使用的协议簇 AF_INET = （TCP/IP – IPv4）
	            dest_addr_ip4->sin_port = htons(anotc_PORT);//端口号
	            ip_protocol = IPPROTO_IP;
	        }

	        //创建套接字，创建成功后返回套接字，创建失败返回-1，错误代码则写入“errno”中
	        sock_anotc = socket(addr_family,  //套接字要使用的协议簇 AF_INET = （TCP/IP – IPv4）
	        				SOCK_DGRAM, //套接字类型
							ip_protocol //确定套接字的协议簇和类型时这个参数为0
							);//创建套接字
	        if (sock_anotc < 0) printf("地面站UDP套接字创建 失败\n");
	        else printf("地面站UDP套接字创建 成功\n");

	        int err = bind(sock_anotc, (struct sockaddr *)&dest_addr_anotc, sizeof(dest_addr_anotc));//套接字绑定

	        if (err < 0)printf("地面站UDP套接字绑定 失败\n");
	        else printf("地面站UDP套接字绑定 成功\n");

	        printf("地面站UDP套接字端口号：%d\n",anotc_PORT);
}

void socket_rc_init(void){//遥控器套接字初始化

	if (addr_family == AF_INET) {//IPV4
	            struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)&dest_addr_rc;
	            dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);//ip地址
	            dest_addr_ip4->sin_family = AF_INET;//套接字要使用的协议簇 AF_INET = （TCP/IP – IPv4）
	            dest_addr_ip4->sin_port = htons(rc_PORT);//端口号
	            ip_protocol = IPPROTO_IP;
	        }

	        //创建套接字，创建成功后返回套接字，创建失败返回-1，错误代码则写入“errno”中
	        sock_rc = socket(addr_family,  //套接字要使用的协议簇 AF_INET = （TCP/IP – IPv4）
	        				SOCK_DGRAM, //套接字类型
							ip_protocol //确定套接字的协议簇和类型时这个参数为0
							);//创建套接字
	        if (sock_rc < 0) printf("遥控器UDP套接字创建 失败\n");
	        else printf("遥控器UDP套接字创建 成功\n");

	        int err = bind(sock_rc, (struct sockaddr *)&dest_addr_rc, sizeof(dest_addr_rc));//套接字绑定

	        if (err < 0)printf("遥控器UDP套接字绑定 失败\n");
	        else printf("遥控器UDP套接字绑定 成功\n");

	        printf("遥控器UDP套接字端口号：%d\n",rc_PORT);
}

void UDP_write_anotc(uint8_t *data,uint8_t len2){//地面站DUP发送
		int err = sendto(sock_anotc, data, len2, 0, (struct sockaddr *)&source_addr_anotc, sizeof(source_addr_anotc));
		if (err < 0) anotc_state = false;//未连接地面站
		else anotc_state = true;//已连接地面站

}

void UDP_write_rc(uint8_t *data,uint8_t len2){//遥控器DUP发送
		int err = sendto(sock_rc, data, len2, 0, (struct sockaddr *)&source_addr_rc, sizeof(source_addr_rc));
		if (err < 0) rc_state = false;//未连接遥控器
		else rc_state = true;//已连接遥控器

}

void UDP_init(void){//UDP通信初始化

	socket_anotc_init();//创建与地面站通信的套接字
	socket_rc_init();   //创建与遥控器通信的套接字
	xTaskCreate(udp_anotc_read_task, "udp_anotc_read_task", 1024*8, (void*)AF_INET, 5, NULL);//创建接收地面站数据的任务
	xTaskCreate(udp_rc_read_task, "udp_rc_read_task", 1024*8, (void*)AF_INET, 8, NULL); //创建接收遥控器数据的任务

}


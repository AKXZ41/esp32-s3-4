#ifndef COMPONENTS_EXTERNAL_COMMUNICATION_UDP_TCP_INCLUDE_UDP_TCP_H_
#define COMPONENTS_EXTERNAL_COMMUNICATION_UDP_TCP_INCLUDE_UDP_TCP_H_

#include <stdio.h>
#include <stdbool.h>
// #include <unistd.h>  // ESP-IDF 5.5中已弃用

void UDP_init(void);
void UDP_write_anotc(uint8_t *data,uint8_t len);//UDP发送
void UDP_write_rc(uint8_t *data,uint8_t len2);

#endif /* COMPONENTS_EXTERNAL_COMMUNICATION_UDP_TCP_INCLUDE_UDP_TCP_H_ */

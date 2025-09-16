# ESP-IDF 5.5 兼容性修复说明

## 修复的问题

### 1. unistd.h 头文件问题
**问题**: ESP-IDF 5.5中`unistd.h`头文件已被弃用
**修复**: 注释掉所有`#include <unistd.h>`语句
**影响文件**:
- `components/Data_declaration/include/Data_declaration.h`
- `components/Data_declaration/Data_declaration.c`
- `components/flight_control/control/control.c`
- `components/flight_control/PID/include/PID.h`
- `components/External_communication/WIFI/WIFI.c`
- `components/External_communication/UDP_TCP/UDP_TCP.c`
- `components/VBAT/VBAT.c`
- `components/LED/LED.c`
- `components/External_communication/anotc_client_v4.34/anotc.c`
- `components/External_communication/remote_control/remote_control.c`
- `components/LED/include/LED.h`
- `components/External_communication/UDP_TCP/include/UDP_TCP.h`
- `components/flight_control/IMU/IMU.c`
- `components/inside_communication/UART/include/UART.h`

### 2. sys/time.h 头文件问题
**问题**: ESP-IDF 5.5中`sys/time.h`头文件已被弃用
**修复**: 替换为`esp_timer.h`
**影响文件**:
- `components/flight_control/control/control.c`

### 3. struct timeval 结构体问题
**问题**: ESP-IDF 5.5中`struct timeval`已被弃用
**修复**: 注释掉`struct timeval tv_now;`声明
**影响文件**:
- `components/flight_control/control/control.c`

### 4. xPortGetFreeHeapSize() 函数问题
**问题**: ESP-IDF 5.5中`xPortGetFreeHeapSize()`已被弃用
**修复**: 替换为`esp_get_free_heap_size()`
**影响文件**:
- `main/main.c`

## 构建说明

### 方法1: 使用ESP-IDF工具
```bash
# 设置ESP-IDF环境
export.bat  # 在ESP-IDF安装目录下运行

# 构建项目
idf.py build
```

### 方法2: 使用提供的构建脚本
```bash
# 运行详细构建脚本
build_detailed.bat
```

### 方法3: 手动使用cmake
```bash
cd build
cmake ..
ninja
```

## 注意事项

1. 确保ESP-IDF 5.5已正确安装
2. 确保Python环境已配置
3. 如果仍有构建错误，请检查具体的错误信息
4. 某些功能可能需要根据ESP-IDF 5.5的新API进行调整

## 可能需要的进一步修复

1. 检查所有使用已弃用API的地方
2. 更新组件依赖关系
3. 检查内存管理相关代码
4. 验证所有FreeRTOS API的使用

## 项目结构

项目是一个四轴无人机控制系统，包含以下主要组件：
- 飞行控制 (flight_control)
- 传感器 (sensor) - MPU6050, SPL06-001
- 内部通信 (inside_communication) - IIC, SPI, UART
- 外部通信 (External_communication) - WiFi, UDP/TCP, 遥控器
- 数据声明 (Data_declaration)
- LED控制
- 电池电压检测 (VBAT)

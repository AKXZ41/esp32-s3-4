
 //数据声明，结构体，堆栈空间，飞行器姿态和PID各类数据在此声明

#include "Data_declaration.h"

#include <stdio.h>
#include <stdbool.h>
// #include <unistd.h>  // ESP-IDF 5.5中已弃用
#include "esp_system.h"
#include "esp_log.h"
#include "string.h"
#include "math.h"

bool init_ok = false;//初始化成功后置1全部任务才会开始运行
int64_t task_run_time_us[10] = {0};  //系统运行时间
int roll_trim;  //横滚角遥控数值补偿
int pitch_trim; //俯仰角遥控数值补偿
int yaw_trim;   //航向角遥控数值补偿
int Acceleration_calibration;
int gyroscope_calibration;
int AirPressure_calibration;
int isRCLocked_p = 0;
uint32_t VBAT;  //电压

// 全局变量定义
sensorData_t sensorData;//传感器原始数据
state_t state;          //状态
setpoint_t setpoint;    //遥控器
spl06CalibCoefficient_t spl06Calib; //气压计校准数据
alt_bro ALT_BRO;

//角度环PID
PID_TYPE PID_ROL_Angle;
PID_TYPE PID_PIT_Angle;
PID_TYPE PID_YAW_Angle;
//角速度环PID
PID_TYPE PID_ROL_Rate;
PID_TYPE PID_PIT_Rate;
PID_TYPE PID_YAW_Rate;
//高度环PID
PID_TYPE PID_ALT_Rate;
PID_TYPE PID_ALT;


/*
 * Data declaration.h
 *
 *  Created on: 2023年1月8日
 *      Author: esp32-4
 */

#ifndef DATA_DECLARATION_H_
#define DATA_DECLARATION_H_

#include <stdio.h>
#include <stdbool.h>
// #include <unistd.h>  // ESP-IDF 5.5中已弃用
#include "esp_system.h"
#include "esp_log.h"
#include "string.h"
#include "math.h"


//PID算法的数据结构
typedef struct PID
{
  float P;         //参数
  float I;
  float D;
  float Error;     //比例项
  float Integral;  //积分项
  float Differ;    //微分项
  float PreError;
  float PrePreError;
  float Ilimit;
  float Irang;
  float Pout;
  float Iout;
  float Dout;
  float OutPut;
  uint8_t Ilimit_flag;    //积分分离
}PID_TYPE;

typedef struct
{
	float pressure;
	float temperature;
	float asl;
} baro_t;

typedef struct
{
		int16_t X;
		int16_t Y;
		int16_t Z;

} INT16_XYZ;

typedef struct
{

		float X;
		float Y;
		float Z;

} FLOAT_XYZ;

typedef struct
{
	INT16_XYZ acc_n;		//加速度原始数值
	INT16_XYZ gyro_n;	//陀螺仪原始数值
	INT16_XYZ mag_n; 	//磁力计原始数值
	baro_t baro;	//气压计原始数值

	FLOAT_XYZ acc_f;		//加速度原始数值
	FLOAT_XYZ gyro_f;	//陀螺仪原始数值

} sensorData_t;

typedef struct
{
	uint32_t timestamp;	//时间戳

	float roll;
	float pitch;
	float yaw;
} attitude_t;

// 四元数的定位
typedef struct quaternion_s
{
	uint32_t timestamp; //时间戳

	union
	{
		struct
		{
			float q0;
			float q1;
			float q2;
			float q3;
		};
		struct
		{
			float x;
			float y;
			float z;
			float w;
		};
	};
} quaternion_t;

struct  vec3_s
{
	uint32_t timestamp;	//时间戳

	float X;
	float Y;
	float Z;
};

typedef enum
{
	modeDisable = 0,/*关闭模式*/
	modeAbs,		/*绝对值模式*/
	modeVelocity	/*速率模式*/
} mode_e;

typedef struct
{
	mode_e x;
	mode_e y;
	mode_e z;
	mode_e roll;
	mode_e pitch;
	mode_e yaw;
}mode_b;

typedef struct vec3_s point_t;
typedef struct vec3_s velocity_t;
typedef struct vec3_s acc_t;

typedef struct
{
	attitude_t attitude;		// deg
	attitude_t attitudeRate;	// deg/s
	point_t position;         	// m
	velocity_t velocity;      	// m/s
	mode_b mode;
	float AUX1;
	float AUX2;
	float AUX3;
	float AUX4;
	float AUX5;
	float AUX6;
	float thrust;
} setpoint_t;

typedef struct
{
    float h;
    float v;
}alt_bro;

typedef struct
{
    int16_t c0;
    int16_t c1;
    int32_t c00;
    int32_t c10;
    int16_t c01;
    int16_t c11;
    int16_t c20;
    int16_t c21;
    int16_t c30;
} spl06CalibCoefficient_t;




typedef struct
{
	attitude_t attitude;//欧拉角
	//quaternion_t attitudeQuaternion;//四元数
	//point_t position; //位置
	//velocity_t velocity;//速度
	//acc_t acc; //角速度
	bool rc_link;
	int airplane_mode; //飞行器当时模式
	bool isRCLocked;//遥控器锁定
} state_t;

extern bool init_ok;
extern int roll_trim;
extern int pitch_trim;
extern int yaw_trim;
extern uint32_t VBAT;
extern int Acceleration_calibration;
extern int gyroscope_calibration;
extern int AirPressure_calibration;
extern int isRCLocked_p;
extern uint32_t VBAT;
 extern int64_t task_run_time_us[10];

extern sensorData_t sensorData;//传感器原始数据
extern state_t state;          //状态
extern setpoint_t setpoint;    //遥控器
extern spl06CalibCoefficient_t spl06Calib; //气压计校准数据
extern alt_bro ALT_BRO;

//角度环PID
extern PID_TYPE PID_ROL_Angle;
extern PID_TYPE PID_PIT_Angle;
extern PID_TYPE PID_YAW_Angle;
//角速度环PID
extern PID_TYPE PID_ROL_Rate;
extern PID_TYPE PID_PIT_Rate;
extern PID_TYPE PID_YAW_Rate;
//高度环PID
extern PID_TYPE PID_ALT_Rate;
extern PID_TYPE PID_ALT;

#endif /* COMPONENTS_DATA_DECLARATION_DATA_DECLARATION_H_ */

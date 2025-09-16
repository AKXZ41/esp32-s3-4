#ifndef COMPONENTS_SENSOR_SPL06_001_INCLUDE_SPL06_001_H_
#define COMPONENTS_SENSOR_SPL06_001_INCLUDE_SPL06_001_H_


#define SPL06_I2C_ADDR					(0x76)
#define SPL06_DEFAULT_CHIP_ID			(0x10)

//测量速率是指每秒刷新多少次结果（测量速度）
//过采样率是指每次结果需要采集多少次数据（测量精度）
//这里的0-7分别代表着 1 2 4 8 16 32 64 128

#define Pressure_measurement_rate		   (4)     //气压测量速率   每秒4次
#define Pressure_oversampling_rate		   (6)      //气压过采样率   每次64

#define Temperature_measurement_rate	   (4)      //温度测量速率   每秒4次
#define Temperature_oversampling_rate	   (6)      //温度过采样率   每次64



// #define PRESSURE_REG 0X00 //压力数据寄存器
// #define TEMP_REG 0X03     //温度数据寄存器

// #define PRS_CFG 0x06  //压强采集的采样速率与过采样速率配置
// #define TMP_CFG 0x07  //温度采集的采样速率与过采样速率配置
// #define MEAS_CFG 0x08 //用来读取当前寄存器的工作状态以及采集模式的配置

#define SPL06_PRESSURE_MSB_REG			(0x00)  /* Pressure MSB Register */
#define SPL06_PRESSURE_LSB_REG			(0x01)  /* Pressure LSB Register */
#define SPL06_PRESSURE_XLSB_REG			(0x02)  /* Pressure XLSB Register */
#define SPL06_TEMPERATURE_MSB_REG		(0x03)  /* Temperature MSB Reg */
#define SPL06_TEMPERATURE_LSB_REG		(0x04)  /* Temperature LSB Reg */
#define SPL06_TEMPERATURE_XLSB_REG		(0x05)  /* Temperature XLSB Reg */
#define SPL06_PRESSURE_CFG_REG			(0x06)	/* Pressure configuration Reg */
#define SPL06_TEMPERATURE_CFG_REG		(0x07)	/* Temperature configuration Reg */
#define SPL06_MODE_CFG_REG				(0x08)  /* Mode and Status Configuration */
#define SPL06_INT_FIFO_CFG_REG			(0x09)	/* Interrupt and FIFO Configuration */
#define SPL06_INT_STATUS_REG			(0x0A)	/* Interrupt Status Reg */
#define SPL06_FIFO_STATUS_REG			(0x0B)	/* FIFO Status Reg */
#define SPL06_RST_REG					(0x0C)  /* Softreset Register */
#define SPL06_CHIP_ID					(0x0D)  /* Chip ID Register */
#define SPL06_COEFFICIENT_CALIB_REG		(0x10)  /* Coeffcient calibraion Register */

#define SPL06_CALIB_COEFFICIENT_LENGTH	(18)
#define SPL06_DATA_FRAME_SIZE			(6)

#define SPL06_CONTINUOUS_MODE			(0x07)

#define TEMPERATURE_INTERNAL_SENSOR		(0)
#define TEMPERATURE_EXTERNAL_SENSOR		(1)

//测量次数 times / S  precision
#define SPL06_MWASURE_1					(0x00)
#define SPL06_MWASURE_2					(0x01)
#define SPL06_MWASURE_4					(0x02)
#define SPL06_MWASURE_8					(0x03)
#define SPL06_MWASURE_16				(0x04)
#define SPL06_MWASURE_32				(0x05)
#define SPL06_MWASURE_64				(0x06)
#define SPL06_MWASURE_128				(0x07)

//过采样率
#define SPL06_OVERSAMP_1				(0x00)
#define SPL06_OVERSAMP_2				(0x01)
#define SPL06_OVERSAMP_4				(0x02)
#define SPL06_OVERSAMP_8				(0x03)
#define SPL06_OVERSAMP_16				(0x04)
#define SPL06_OVERSAMP_32				(0x05)
#define SPL06_OVERSAMP_64				(0x06)
#define SPL06_OVERSAMP_128				(0x07)

float spl0601_get_temperature(int32_t rawTemperature);
float spl0601_get_pressure(int32_t rawPressure, int32_t rawTemperature);

void SPL06Init(void);
void SPL06GetData(float* pressure, float* temperature, float* asl);
float SPL06PressureToAltitude(float pressure/*, float* groundPressure, float* groundTemp*/);

void SPL06GetPressure(void);
int get_offset_start(void);
void Height_Get(void);


#endif /* COMPONENTS_SENSOR_SPL06_001_INCLUDE_SPL06_001_H_ */

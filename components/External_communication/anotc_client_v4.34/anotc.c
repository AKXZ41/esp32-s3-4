//匿名上位机通信（版本v4.34）

#include "anotc.h"

#include "UART.h"

#include "esp_log.h"
#include "UDP_TCP.h"
#include <stdio.h>
#include <stdbool.h>
// #include <unistd.h>  // ESP-IDF 5.5中已弃用
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Data_declaration.h"
#include "PID.h"
#include "control.h"

void anotc_send(void * pvParameters);
void ANO_DT_Send_voltage(float voltage, float electricity);
void ANO_DT_Send_Sensor_calibration_feedback(uint8_t MSG_ID,uint8_t MSG_DATA);
void ANO_DT_Send_Status(float angle_rol, float angle_pit, float angle_yaw, uint32_t alt, uint8_t FLY_ENABLEl, uint8_t armed);
void ANO_DT_Send_PID(uint8_t group,float p1_p,float p1_i,float p1_d,float p2_p,float p2_i,float p2_d,float p3_p,float p3_i,float p3_d);

#define DATA_write  UDP_write_anotc

static uint8_t data_to_send[50];	//发送数据缓存

//数据拆分宏定义，在发送大于1字节的数据类型时，比如int16、float等，需要把数据拆分成单独字节进行发送
#define BYTE0(dwTemp)       ( *( (char *)(&dwTemp)	  ) )
#define BYTE1(dwTemp)       ( *( (char *)(&dwTemp) + 1) )
#define BYTE2(dwTemp)       ( *( (char *)(&dwTemp) + 2) )
#define BYTE3(dwTemp)       ( *( (char *)(&dwTemp) + 3) )


void ANO_DT_Send_Check(uint8_t head, uint8_t check_sum)  //发送校验数据   head 帧头    check_sum  接收到的数据生成的校验
{
	uint8_t sum = 0,i;
	data_to_send[0]=0xAA;
	data_to_send[1]=0xAA;
	data_to_send[2]=0xEF;
	data_to_send[3]=2;
	data_to_send[4]=head;
	data_to_send[5]=check_sum;

	for(i=0;i<6;i++)
		sum += data_to_send[i];
	data_to_send[6]=sum;

	DATA_write(data_to_send, 7);
}

int _temp1;
void anotc_data_decode (char *data){
	uint8_t sum = 0; //校验

	if(data[0] != 0xAA && data[1] != 0xAF)  return; //当帧头不等于AAAF就退出舍弃这段数据
	for(int i = 0;i < data[3] + 4 ;i++) sum+=(uint8_t)data[i];
	if(sum != data[data[3]+4]) return; //当校验不等时就退出舍弃这段数据
	//printf("数据正确 功能字：%d \n",data[2]);
	switch(data[2])
	{
		 case 0X01: //命令集合1
			 switch(data[4])
			 	{
			 		case 0X01: //加速度校准
			 			Acceleration_calibration = 1;
			 		break;

			 		case 0X02: //陀螺仪校准
			 			gyroscope_calibration = 1;


			 		break;
			 		case 0X03: //磁力计校准


			 		break;
			 		case 0X05: //气压计校准
			 			AirPressure_calibration = 1;


			 		break;
			 	}


		 break;
		 case 0X02:  //命令集合2
				switch(data[4])
				{
					case 0X01: //读取PID请求
						ANO_DT_Send_PID(1,PID_ROL_Rate.P,PID_ROL_Rate.I,PID_ROL_Rate.D,PID_PIT_Rate.P,PID_PIT_Rate.I,PID_PIT_Rate.D,PID_YAW_Rate.P,PID_YAW_Rate.I,PID_YAW_Rate.D);
						ANO_DT_Send_PID(2,PID_ROL_Angle.P,PID_ROL_Angle.I,PID_ROL_Angle.D,PID_PIT_Angle.P,PID_PIT_Angle.I,PID_PIT_Angle.D,PID_YAW_Angle.P,PID_YAW_Angle.I,PID_YAW_Angle.D);
						//ANO_DT_Send_PID(3,0,0,0,0,0,0,0,0,0);
						//ANO_DT_Send_PID(4,0,0,0,0,0,0,0,0,0);
						//ANO_DT_Send_PID(5,0,0,0,0,0,0,0,0,0);
						ANO_DT_Send_PID(6,0,0,0,0,0,0,(float)roll_trim/1000+1,(float)pitch_trim/1000+1,(float)yaw_trim/1000+1												);
					break;

					case 0XA1: //恢复默认参数
						PidParameter_init();
						ANO_DT_Send_PID(1,PID_ROL_Rate.P,PID_ROL_Rate.I,PID_ROL_Rate.D,PID_PIT_Rate.P,PID_PIT_Rate.I,PID_PIT_Rate.D,PID_YAW_Rate.P,PID_YAW_Rate.I,PID_YAW_Rate.D);
						ANO_DT_Send_PID(2,PID_ROL_Angle.P,PID_ROL_Angle.I,PID_ROL_Angle.D,PID_PIT_Angle.P,PID_PIT_Angle.I,PID_PIT_Angle.D,PID_YAW_Angle.P,PID_YAW_Angle.I,PID_YAW_Angle.D);
						//ANO_DT_Send_PID(3,0,0,0,0,0,0,0,0,0);
						//ANO_DT_Send_PID(4,0,0,0,0,0,0,0,0,0);
						//ANO_DT_Send_PID(5,0,0,0,0,0,0,0,0,0);
						ANO_DT_Send_PID(6,0,0,0,0,0,0,1.0f,1.0f,1.0f);

					break;
				}


		 break;
		 case 0X10:  //PID1-3
			  PID_ROL_Rate.P = 0.001f*((uint16_t)data[4]<<8| data[5]);
			  PID_ROL_Rate.I = 0.001f*((uint16_t)data[6]<<8| data[7]);
			  PID_ROL_Rate.D = 0.001f*((uint16_t)data[8]<<8| data[9]);
			  PID_PIT_Rate.P = 0.001f*((uint16_t)data[10]<<8| data[11]);
			  PID_PIT_Rate.I = 0.001f*((uint16_t)data[12]<<8| data[13]);
			  PID_PIT_Rate.D = 0.001f*((uint16_t)data[14]<<8| data[15]);
			  PID_YAW_Rate.P = 0.001f*((uint16_t)data[16]<<8| data[17]);
			  PID_YAW_Rate.I = 0.001f*((uint16_t)data[18]<<8| data[19]);
			  PID_YAW_Rate.D = 0.001f*((uint16_t)data[20]<<8| data[21]);

		 break;
		 case 0X11:  //PID4-6
			 PID_ROL_Angle.P = 0.001f*((uint16_t)data[4]<<8| data[5]);
			 PID_ROL_Angle.I = 0.001f*((uint16_t)data[6]<<8| data[7]);
			 PID_ROL_Angle.D = 0.001f*((uint16_t)data[8]<<8| data[9]);
		     PID_PIT_Angle.P = 0.001f*((uint16_t)data[10]<<8| data[11]);
			 PID_PIT_Angle.I = 0.001f*((uint16_t)data[12]<<8| data[13]);
			 PID_PIT_Angle.D = 0.001f*((uint16_t)data[14]<<8| data[15]);
			 PID_YAW_Angle.P = 0.001f*((uint16_t)data[16]<<8| data[17]);
			 PID_YAW_Angle.I = 0.001f*((uint16_t)data[18]<<8| data[19]);
			 PID_YAW_Angle.D = 0.001f*((uint16_t)data[20]<<8| data[21]);
		 break;
		 case 0X12:  //PID7-9
		 break;
		 case 0X13:  //PID10-12
		 break;
		 case 0X14:  //PID13-15
		 break;
		 case 0X15:  //PID16-18

			 _temp1 =  (uint16_t)data[16]<<8| data[17];
			 roll_trim = _temp1 - 1000;
			 _temp1 = (uint16_t)data[18]<<8| data[19];
			 pitch_trim = _temp1 - 1000;
			 _temp1 =   (uint16_t)data[20]<<8| data[21];
			 yaw_trim = _temp1 - 1000;
		 break;
		 default:  printf("功能字未知\n");

	}
	ANO_DT_Send_Check(data[3],sum);

	 //printf("ROLL=%d  ",(int)setpoint.attitude.roll);
	 //printf("PITCH=%d  ",(int)setpoint.attitude.pitch);
	 //printf("YAW=%d  ",(int)setpoint.attitude.yaw);
	 //printf("THRUST=%d  \n",(int)setpoint.thrust);
	 //printf("locked=%d  \n",state.isRCLocked);

}

void ANO_DT_Send_Sensor_calibration_feedback(uint8_t MSG_ID,uint8_t MSG_DATA)
{
	uint8_t _cnt=0,sum = 0,i;

	data_to_send[_cnt++]=0xAA;         //帧头
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0xEE;         //功能字
	data_to_send[_cnt++]=0x02;         //数据长度

	data_to_send[_cnt++]=MSG_ID;   //油门
	data_to_send[_cnt++]=MSG_DATA;

	for(i=0;i<_cnt;i++)
		sum += data_to_send[i];         //数据校验求解

	data_to_send[_cnt++]=sum;         //数据校验赋值

	DATA_write(data_to_send, _cnt);   //帧发送

}


void ANO_DT_Send_RCData(uint16_t thr,uint16_t yaw,uint16_t rol,uint16_t pit,uint16_t aux1,uint16_t aux2,uint16_t aux3,uint16_t aux4,uint16_t aux5,uint16_t aux6)
{
	uint8_t _cnt=0,sum = 0,i;

	data_to_send[_cnt++]=0xAA;         //帧头
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0x03;         //功能字
	data_to_send[_cnt++]=0;            //数据长度
	data_to_send[_cnt++]=BYTE1(thr);   //油门
	data_to_send[_cnt++]=BYTE0(thr);
	data_to_send[_cnt++]=BYTE1(yaw);   //航向角
	data_to_send[_cnt++]=BYTE0(yaw);
	data_to_send[_cnt++]=BYTE1(rol);   //横滚
	data_to_send[_cnt++]=BYTE0(rol);
	data_to_send[_cnt++]=BYTE1(pit);   //俯仰
	data_to_send[_cnt++]=BYTE0(pit);
	data_to_send[_cnt++]=BYTE1(aux1);
	data_to_send[_cnt++]=BYTE0(aux1);
	data_to_send[_cnt++]=BYTE1(aux2);
	data_to_send[_cnt++]=BYTE0(aux2);
	data_to_send[_cnt++]=BYTE1(aux3);
	data_to_send[_cnt++]=BYTE0(aux3);
	data_to_send[_cnt++]=BYTE1(aux4);
	data_to_send[_cnt++]=BYTE0(aux4);
	data_to_send[_cnt++]=BYTE1(aux5);
	data_to_send[_cnt++]=BYTE0(aux5);
	data_to_send[_cnt++]=BYTE1(aux6);
	data_to_send[_cnt++]=BYTE0(aux6);

	data_to_send[3] = _cnt-4;         //数据帧长度赋值

	for(i=0;i<_cnt;i++)
		sum += data_to_send[i];         //数据校验求解

	data_to_send[_cnt++]=sum;         //数据校验赋值

	DATA_write(data_to_send, _cnt);   //帧发送
}


//向匿名上位机发送姿态数据
void ANO_DT_Send_Status(float angle_rol, float angle_pit, float angle_yaw, uint32_t alt, uint8_t FLY_ENABLEl, uint8_t armed)
{
	uint8_t _cnt=0; //发送缓冲区字节数
	uint8_t sum = 0;//校验
	uint16_t _temp;//临时变量1
	uint32_t _temp2 = alt;//临时变量2

	data_to_send[_cnt++]=0xAA;//4个A的帧头
	data_to_send[_cnt++]=0xAA;//4个A的帧头
	data_to_send[_cnt++]=0x01;//功能字为01
	data_to_send[_cnt++]=0;	  //这个是数据长度现在先不填后面再填

	_temp = (int)(angle_rol*100);//横滚角x100转为int16类型
	data_to_send[_cnt++]=BYTE1(_temp);//int16高位填充
	data_to_send[_cnt++]=BYTE0(_temp);//int16低位填充
	_temp = (int)(angle_pit*100);
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = (int)(angle_yaw*100);
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);

	data_to_send[_cnt++]=BYTE3(_temp2);
	data_to_send[_cnt++]=BYTE2(_temp2);
	data_to_send[_cnt++]=BYTE1(_temp2);
	data_to_send[_cnt++]=BYTE0(_temp2);

	data_to_send[_cnt++] = FLY_ENABLEl;

	data_to_send[_cnt++] = armed;

	data_to_send[3] = _cnt-4; //_cnt累计的长度减去前面的4个非数据字节等于数据的长度

	for(int i = 0 ; i < _cnt ; i++) sum += data_to_send[i];

	data_to_send[_cnt++]=sum;

	DATA_write(data_to_send, _cnt);
}

//向匿名上位机发送姿态数据
void ANO_DT_Send_voltage(float voltage, float electricity)
{
	uint8_t _cnt=0; //发送缓冲区字节数
	uint8_t sum = 0;//校验
	uint16_t _temp;//临时变量1

	data_to_send[_cnt++]=0xAA;//4个A的帧头
	data_to_send[_cnt++]=0xAA;//4个A的帧头
	data_to_send[_cnt++]=0x05;//功能字为05
	data_to_send[_cnt++]=0;	  //这个是数据长度现在先不填后面再填

	_temp = (int)voltage;//横滚角x100转为int16类型
	data_to_send[_cnt++]=BYTE1(_temp);//int16高位填充
	data_to_send[_cnt++]=BYTE0(_temp);//int16低位填充
	_temp = (int)(electricity*100);
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);

	data_to_send[3] = _cnt-4; //_cnt累计的长度减去前面的4个非数据字节等于数据的长度

	for(int i = 0 ; i < _cnt ; i++) sum += data_to_send[i];

	data_to_send[_cnt++]=sum;

	DATA_write(data_to_send, _cnt);
}


//向匿名上位机发送PID数据
void ANO_DT_Send_PID(uint8_t group,float p1_p,float p1_i,float p1_d,float p2_p,float p2_i,float p2_d,float p3_p,float p3_i,float p3_d)
{
	uint8_t _cnt=0,sum = 0,i;
	uint16_t _temp;

	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0x10+group-1;  // 设置为不同的PID帧
	data_to_send[_cnt++]=0;


	_temp = p1_p * 1000;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = p1_i  * 1000;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = p1_d  * 1000;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = p2_p  * 1000;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = p2_i  * 1000;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = p2_d * 1000;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = p3_p  * 1000;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = p3_i  * 1000;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = p3_d * 1000;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);

	data_to_send[3] = _cnt-4;

	for(i=0;i<_cnt;i++)
		sum += data_to_send[i];

	data_to_send[_cnt++]=sum;

	DATA_write(data_to_send, _cnt);
}

void anotc_send(void * pvParameters){//匿名上位机数据发送
	TickType_t adp = xTaskGetTickCount();
	const TickType_t adg = 2;//这里的10是指10ms运行一次100hz的意思
	while(1){
		vTaskDelayUntil(&adp,adg);
		if(init_ok)
		{
			static uint8_t status_cnt 	= 0;
			static uint8_t senser_cnt 	= 0;
			static uint8_t rcdata_cnt 	= 0;
			static uint8_t motopwm_cnt	= 0;
			static uint8_t power_cnt	= 0;
			status_cnt++;
			senser_cnt++;
			rcdata_cnt++;
			motopwm_cnt++;
			power_cnt++;
			if(status_cnt>=55)//刷新姿态状态
			{
				status_cnt = 0;
				ANO_DT_Send_Status(state.attitude.roll,state.attitude.pitch,state.attitude.yaw,ALT_BRO.h,1,state.isRCLocked);
			}
			if(senser_cnt>=5)//刷新传感器状态
			{

				senser_cnt = 0;
				ANO_DT_Send_voltage((float)VBAT,0.0f);//反馈电压给上位机

			}
			if(rcdata_cnt>=20)
			{
				rcdata_cnt = 0;
				ANO_DT_Send_RCData((uint16_t)setpoint.thrust,(uint16_t)setpoint.attitude.pitch,(uint16_t)setpoint.attitude.roll,(uint16_t)setpoint.attitude.pitch,(uint16_t)setpoint.AUX1,(uint16_t)setpoint.AUX2,(uint16_t)setpoint.AUX3,(uint16_t)setpoint.AUX4,(uint16_t)setpoint.AUX5,(uint16_t)setpoint.AUX6);
			}
			if(motopwm_cnt>=35)
			{
				motopwm_cnt = 0;
				if(Acceleration_calibration == 2) {
					ANO_DT_Send_Sensor_calibration_feedback(0x01,0x01);//校准完成后反馈信息给匿名上位机
					Acceleration_calibration = 3;
				}
				if(gyroscope_calibration == 2) {
					ANO_DT_Send_Sensor_calibration_feedback(2,1);//校准完成后反馈信息给匿名上位机
					gyroscope_calibration = 3;
				}
				if(AirPressure_calibration == 2) {
					ANO_DT_Send_Sensor_calibration_feedback(3,1);//校准完成后反馈信息给匿名上位机
					AirPressure_calibration = 3;
				}
			}
			if(power_cnt>=60)
			{
				power_cnt = 0;
			}

		}
	}
}


void anotc_Init(void){//匿名上位机初始化

	xTaskCreate(anotc_send,
	             "anotc_send",
	              1024*5,
	              NULL,
	              5,
	              NULL
	              );

}

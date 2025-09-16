//姿态解算程序
#include "IMU.h"
#include "Data_declaration.h"
#include <stdio.h>
#include <stdbool.h>
// #include <unistd.h>  // ESP-IDF 5.5中已弃用
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"  // ESP-IDF 5.5中仍然使用原路径
#include "string.h"
#include "driver/gpio.h"  // ESP-IDF 5.5中仍然使用原路径
#include "math.h"
#include "MPU6050.h"

#define Kp_New      0.9f              //互补滤波当前数据的权重
#define Kp_Old      0.1f              //互补滤波历史数据的权重
#define Acc_Gain    0.0001220f      //加速度变成G (初始化加速度满量程-+4g LSBa = 2*4/65535.0)
#define Gyro_Gain   0.0609756f      //角速度变成度 (初始化陀螺仪满量程+-2000 LSBg = 2*2000/65535.0)
#define Gyro_Gr     0.0010641f      //角速度变成弧度(3.1415/180 * LSBg)

#define N 12      //滤波缓存数组大小
#define M_PI_F 3.1416f



               //把陀螺仪的各通道读出的数据，转换成弧度制
FLOAT_XYZ   Acc_filtold;    //滤波后的各通道数据
float   accb[3],DCMgb[3][3];                  //方向余弦阵（将 惯性坐标系 转化为 机体坐标系）
uint8_t AccbUpdate = 0;



float FindPos(float*a,int low,int high){ //确定一个元素的位序
    float val = a[low];                      //选定一个要确定值val确定位置
    while(low<high)
    {
        while(low<high && a[high]>=val)
        {
             high--;                       //如果右边的数大于VAL下标往前移
             a[low] = a[high];             //当右边的值小于VAL则复值给A[low]
        }
        while(low<high && a[low]<=val)
        {
             low++;                        //如果左边的数小于VAL下标往后移
             a[high] = a[low];             //当左边的值大于VAL则复值给右边a[high]
        }
    }
    a[low] = val;
    return low;
}

void QuiteSort(float* a,int low,int high){ //快速排序
     int pos;
     if(low<high)
     {
         pos = FindPos(a,low,high); //排序一个位置
         QuiteSort(a,low,pos-1);    //递归调用
         QuiteSort(a,pos+1,high);
     }
 }

//acc  Acc_filt n
void  SortAver_FilterXYZ(INT16_XYZ *acc,FLOAT_XYZ *Acc_filt,uint8_t n){ //窗口滑动滤波函数
  static float bufx[20],bufy[20],bufz[20];
  static uint8_t cnt =0,flag = 1;
  float temp1=0,temp2=0,temp3=0;
  uint8_t i;
  bufx[cnt] = acc->X;
  bufy[cnt] = acc->Y;
  bufz[cnt] = acc->Z;
  cnt++;      //这个的位置必须在赋值语句后，否则bufx[0]不会被赋值
  if(cnt<n && flag) //如果两个条件都不为零则条件满足
    return;   //数组填不满不计算
  else
    flag = 0;

  QuiteSort(bufx,0,n-1);
  QuiteSort(bufy,0,n-1);
  QuiteSort(bufz,0,n-1);
  for(i=1;i<n-1;i++)
   {
    temp1 += bufx[i];
    temp2 += bufy[i];
    temp3 += bufz[i];
   }

   if(cnt>=n) cnt = 0;
   Acc_filt->X  = temp1/(n-2);
   Acc_filt->Y  = temp2/(n-2);
   Acc_filt->Z  = temp3/(n-2);
}


void Prepare_Data(sensorData_t* sensorDatafilter){ //获取姿态值并进行处理
  static uint8_t IIR_mode = 1;

//  Aver_FilterXYZ(&MPU6050_ACC_RAW,&Acc_filt,12);//对加速度原始数据进行滑动窗口滤波
  SortAver_FilterXYZ(&sensorDatafilter->acc_n,&sensorDatafilter->acc_f,20);//对加速度原始数据进行去极值滑动窗口滤波

  //加速度AD值 转换成 米/平方秒
  sensorDatafilter->acc_f.X = (float)sensorDatafilter->acc_f.X * Acc_Gain * 9.80665f;
  sensorDatafilter->acc_f.Y = (float)sensorDatafilter->acc_f.Y * Acc_Gain * 9.80665f;
  sensorDatafilter->acc_f.Z = (float)sensorDatafilter->acc_f.Z * Acc_Gain * 9.80665f;
  //printf("ax=%0.2f ay=%0.2f az=%0.2f\r\n",sensorDatafilter->acc_f.X,sensorDatafilter->acc_f.Y,sensorDatafilter->acc_f.Z);

  //陀螺仪AD值 转换成 弧度/秒
  sensorDatafilter->gyro_f.X = (float) sensorDatafilter->gyro_n.X * Gyro_Gr;
  sensorDatafilter->gyro_f.Y = (float) sensorDatafilter->gyro_n.Y * Gyro_Gr;
  sensorDatafilter->gyro_f.Z = (float) sensorDatafilter->gyro_n.Z * Gyro_Gr;
  //printf("ax=%0.2f ay=%0.2f az=%0.2f\r\n",sensorDatafilter->gyro_f.X,sensorDatafilter->gyro_f.Y,sensorDatafilter->gyro_f.Z);
  if(IIR_mode)
  {
	sensorDatafilter->acc_f.X = sensorDatafilter->acc_f.X * Kp_New + Acc_filtold.X * Kp_Old;
	sensorDatafilter->acc_f.Y = sensorDatafilter->acc_f.Y * Kp_New + Acc_filtold.Y * Kp_Old;
	sensorDatafilter->acc_f.Z = sensorDatafilter->acc_f.Z * Kp_New + Acc_filtold.Z * Kp_Old;
//    Gyr_rad.X = Gyr_rad.X * Kp_New + Gyr_radold.X * Kp_Old;
//    Gyr_rad.Y = Gyr_rad.Y * Kp_New + Gyr_radold.Y * Kp_Old;
//    Gyr_rad.Z = Gyr_rad.Z * Kp_New + Gyr_radold.Z * Kp_Old;

    Acc_filtold.X =  sensorDatafilter->acc_f.X;
    Acc_filtold.Y =  sensorDatafilter->acc_f.Y;
    Acc_filtold.Z =  sensorDatafilter->acc_f.Z;
//    Gyr_radold.X = Gyr_rad.X;
//    Gyr_radold.Y = Gyr_rad.Y;
//    Gyr_radold.Z = Gyr_rad.Z;
  }
  accb[0] = sensorDatafilter->acc_f.X;
  accb[1] = sensorDatafilter->acc_f.Y;
  accb[2] = sensorDatafilter->acc_f.Z;
  //printf("ax=%0.2f ay=%0.2f az=%0.2f\r\n",Acc_filt.X,Acc_filt.Y,Acc_filt.Z);
  if(accb[0]&&accb[1]&&accb[2])
  {
    AccbUpdate = 1;
  }
}

static float invSqrt(float x) { //快速计算 1/sqrt(x)
  float halfx = 0.5f * x;
  float y = x;
  long i = *(long*)&y;
  i = 0x5f375a86 - (i>>1);
  y = *(float*)&i;
  y = y * (1.5f - (halfx * y * y));
  return y;
}


//kp=ki=0 就是完全相信陀螺仪
#define Kp 1.50f                         // proportional gain governs rate of convergence to accelerometer/magnetometer
                                         //比例增益控制加速度计，磁力计的收敛速率
#define Ki 0.005f                        // integral gain governs rate of convergence of gyroscope biases
                                         //积分增益控制陀螺偏差的收敛速度
#define halfT 0.005f                     // half the sample period 采样周期的一半 0.005 是5ms的意思


float q0 = 1, q1 = 0, q2 = 0, q3 = 0;     // quaternion elements representing the estimated orientation
float exInt = 0, eyInt = 0, ezInt = 0;    // scaled integral error

void IMUupdate(sensorData_t* sensorDataimu,state_t* stateimu){//四元数姿态解算

  uint8_t i;
  float matrix[9] = {1.f,  0.0f,  0.0f, 0.0f,  1.f,  0.0f, 0.0f,  0.0f,  1.f };//初始化矩阵
  float ax = sensorDataimu->acc_f.X,ay = sensorDataimu->acc_f.Y,az = sensorDataimu->acc_f.Z;
  float gx = sensorDataimu->gyro_f.X,gy = sensorDataimu->gyro_f.Y,gz = sensorDataimu->gyro_f.Z;
  float vx, vy, vz;
  float ex, ey, ez;
  float norm;

  float q0q0 = q0*q0;
  float q0q1 = q0*q1;
  float q0q2 = q0*q2;
  float q0q3 = q0*q3;
  float q1q1 = q1*q1;
  float q1q2 = q1*q2;
  float q1q3 = q1*q3;
  float q2q2 = q2*q2;
  float q2q3 = q2*q3;
  float q3q3 = q3*q3;

  if(ax*ay*az==0)  return;


  //加速度计测量的重力向量(机体坐标系)
  norm = invSqrt(ax*ax + ay*ay + az*az);
  ax = ax * norm;
  ay = ay * norm;
  az = az * norm;
  //  printf("ax=%0.2f ay=%0.2f az=%0.2f\r\n",ax,ay,az);

  //陀螺仪积分估计重力向量(机体坐标系)
  vx = 2*(q1q3 - q0q2);
  vy = 2*(q0q1 + q2q3);
  vz = q0q0 - q1q1 - q2q2 + q3q3 ;
  // printf("vx=%0.2f vy=%0.2f vz=%0.2f\r\n",vx,vy,vz);

  //测量的重力向量与估算的重力向量差积求出向量间的误差
  ex = (ay*vz - az*vy); //+ (my*wz - mz*wy);
  ey = (az*vx - ax*vz); //+ (mz*wx - mx*wz);
  ez = (ax*vy - ay*vx); //+ (mx*wy - my*wx);

  //用上面求出误差进行积分
  exInt = exInt + ex * Ki;
  eyInt = eyInt + ey * Ki;
  ezInt = ezInt + ez * Ki;

  //将误差PI后补偿到陀螺仪
  gx = gx + Kp*ex + exInt;
  gy = gy + Kp*ey + eyInt;
  gz = gz + Kp*ez + ezInt;//这里的gz由于没有观测者进行矫正会产生漂移，表现出来的就是积分自增或自减

  //四元素的微分方程
  q0 = q0 + (-q1*gx - q2*gy - q3*gz)*halfT;
  q1 = q1 + (q0*gx + q2*gz - q3*gy)*halfT;
  q2 = q2 + (q0*gy - q1*gz + q3*gx)*halfT;
  q3 = q3 + (q0*gz + q1*gy - q2*gx)*halfT;

  //单位化四元数
  norm = invSqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
  q0 = q0 * norm;
  q1 = q1 * norm;
  q2 = q2 * norm;
  q3 = q3 * norm;

  //矩阵R 将惯性坐标系(n)转换到机体坐标系(b)
  matrix[0] = q0q0 + q1q1 - q2q2 - q3q3;  // 11(前列后行)
  matrix[1] = 2.f * (q1q2 + q0q3);      // 12
  matrix[2] = 2.f * (q1q3 - q0q2);      // 13
  matrix[3] = 2.f * (q1q2 - q0q3);      // 21
  matrix[4] = q0q0 - q1q1 + q2q2 - q3q3;  // 22
  matrix[5] = 2.f * (q2q3 + q0q1);      // 23
  matrix[6] = 2.f * (q1q3 + q0q2);      // 31
  matrix[7] = 2.f * (q2q3 - q0q1);      // 32
  matrix[8] = q0q0 - q1q1 - q2q2 + q3q3;  // 33

  //四元数转换成欧拉角(Z->Y->X)
  float _temp;
    _temp = sensorDataimu->gyro_f.Z *RadtoDeg*0.01f;
    if(_temp > 0.03f || _temp < -0.01f)  stateimu->attitude.yaw -= _temp;
  //stateimu->attitude.yaw += sensorDataimu->gyro_f.Z *RadtoDeg*0.01f;
//  Att_Angle->yaw = atan2(2.f * (q1q2 + q0q3), q0q0 + q1q1 - q2q2 - q3q3)* 57.3f; // yaw
  stateimu->attitude.roll = -asin(2.f * (q1q3 - q0q2))* 57.3f;                            // roll(负号要注意)
  stateimu->attitude.pitch = atan2(2.f * q2q3 + 2.f * q0q1, q0q0 - q1q1 - q2q2 + q3q3)* 57.3f ; // pitch
  for(i=0;i<9;i++)
  {
    *(&(DCMgb[0][0])+i) = matrix[i];
  }
}


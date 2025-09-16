#只有在这里写出的文件会被编译器编译和被其它地方访问
#这个文件修改一次需要重新编译所有文件
#在这个文件里 # 号是注释

idf_component_register(
SRCS            #这里标记出目录下的c文件
"IMU/IMU.c" "PID/PID.c" "PWM/PWM.c" "control/control.c"   	
			
INCLUDE_DIRS  	#这里标记出目录中存放h文件的文件夹 
"IMU/include" "PID/include" "PWM/include" "control/include"

REQUIRES        #标记出需要的资源
sensor
Data_declaration
driver          #需要driver组件来使用GPIO、UART等驱动
esp_timer       #需要esp_timer组件来使用定时器功能

PRIV_REQUIRES   #标记出需要的资源

)
#只有在这里写出的文件会被编译器编译和被其它地方访问
#这个文件修改一次需要重新编译所有文件
#在这个文件里 # 号是注释

idf_component_register(
SRCS            #这里标记出目录下的c文件
"UART/UART.c" "IIC/IIC.c" "SPI/SPI.c"
			
INCLUDE_DIRS  	#这里标记出目录中存放h文件的文件夹
"UART/include" "IIC/include" "SPI/include"

REQUIRES        #标记出需要的资源
driver          #需要driver组件来使用I2C、SPI、UART、GPIO等驱动

PRIV_REQUIRES   #标记出需要的资源

)
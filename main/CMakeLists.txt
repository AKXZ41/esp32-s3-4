# See the build system documentation in IDF programming guide
# for more information about component CMakeLists.txt files.
#只有在这里写出的文件会被编译器编译和被其它地方访问
#这个文件修改一次需要重新编译所有文件 
#在这个文件里 # 号是注释  

idf_component_register(
SRCS                # 这里标记出目录下的c文件 list the source files of this component
main.c 
INCLUDE_DIRS        # 这里标记出目录中存放h文件的文件夹 optional, add here public include directories

PRIV_INCLUDE_DIRS   # optional, add here private include directories

REQUIRES            # 标记出需要的资源 optional, list the public requirements (component names)
inside_communication  #内部通信
sensor                #传感器
flight_control		  #飞行控制
External_communication#外部通信
LED                   #灯
Data_declaration      #数据结构体存储
VBAT                  #电池电量
PRIV_REQUIRES       # 标记出需要的资源 optional, list the private requirements
)

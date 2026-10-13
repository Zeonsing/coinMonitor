# 指定 CMake 的最低版本
cmake_minimum_required(VERSION 3.22.1)

# 定义项目名称
project("eth-monitor")

# 将你的算法核心文件与 JNI 胶水文件联合编译成一个名为 "pricemonitor" 的共享库
# 编译成功后，Android 会自动生成 libpricemonitor.so
add_library(
        pricemonitor
        SHARED
        price_monitor.c
        native-lib.c   #  关键：必须把胶水层文件加在这里一起打包
)

# 链接 Android 核心日志库（方便你在 C 代码里用 __android_log_print 打印 Log）
find_library(log-lib log)
target_link_libraries(pricemonitor ${log-lib})
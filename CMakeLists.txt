#cmake最低版本和项目名称
cmake_minimum_required(VERSION 3.0)
project(mprpc)

#生成Debug版本供调试
set(CMAKE_BUILD_TYPE "Debug")
#设置项目可执行文件输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#设置项目库文件的输出路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

#设置项目编译头文件搜索路径
include_directories(${PROJECT_SOURCE_DIR}/src/include)
include_directories(${PROJECT_SOURCE_DIR}/example)
#设置项目库文件搜索路径
link_directories(${PROJECT_SOURCE_DIR}/lib)
link_directories(${PROJECT_SOURCE_DIR}/home/stu/ku/zookeeper-3.4.10/src/c/.libs)
link_directories(${PROJECT_SOURCE_DIR}/usr/local/lib)

#框架文件
add_subdirectory(src)
#业务文件 example 包含所有示例代码
add_subdirectory(example)


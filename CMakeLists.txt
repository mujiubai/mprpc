cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDARD 11)
project(mprpc)

#生成debug版本，gdb调试
set(CMAKE_BUILD_TYPE "Debug")

#设置可执行文件输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#设置项目库文件输出路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

#设置编译头文件搜索路径
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/example)
#设置项目库文件搜索路径
link_directories(${PROJECT_SOURCE_DIR}/src)
link_directories(/usr/local/lib)

add_subdirectory(src)
add_subdirectory(example)


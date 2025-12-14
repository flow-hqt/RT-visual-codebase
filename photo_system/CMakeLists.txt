cmake_minimum_required(VERSION 3.10)
project(RtVisionSystem)

set(CMAKE_CXX_STANDARD 17)

# 使用 CMAKE_CURRENT_SOURCE_DIR 确保路径基于当前 CMakeLists.txt 的位置
# 而不是根目录的位置
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# 编译核心库
add_library(rt_vision_lib src/image_system.cpp)

# 编译可执行文件
add_executable(demo_app app/main.cpp)

# 链接
target_link_libraries(demo_app rt_vision_lib)

# Linux/Mac 兼容性
if(UNIX)
    target_link_libraries(demo_app pthread)
endif()
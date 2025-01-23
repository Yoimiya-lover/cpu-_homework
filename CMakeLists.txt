cmake_minimum_required(VERSION 3.20)

project(cpu_homework)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

set(MY_INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build)
# 添加包含目录给所有目标
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)


message(STATUS "Binary directory: ${CMAKE_CURRENT_BINARY_DIR}")
message(STATUS "Source directory: ${CMAKE_CURRENT_SOURCE_DIR}/include")

# 添加编译选项
add_compile_options(-msse3)

# 设置源代码路径
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# 添加库
add_library(WorkerThread SHARED  ${CMAKE_CURRENT_SOURCE_DIR}/WorkerThread.cc)
add_library(MMAClass SHARED  ${CMAKE_CURRENT_SOURCE_DIR}/MMAClass.cc)
add_library(Matrix_Mul_Algorithm SHARED  ${CMAKE_CURRENT_SOURCE_DIR}/Matrix_Mul_Algorithm.cc)

# 添加可执行文件
add_executable(cpu_test ${CMAKE_CURRENT_SOURCE_DIR}/MMAtest.cc)

# 设置库依赖关系
target_link_libraries(Matrix_Mul_Algorithm PUBLIC WorkerThread)
target_link_directories(Matrix_Mul_Algorithm PRIVATE ${CMAKE_BINARY_DIR})
target_link_libraries(cpu_test PRIVATE Matrix_Mul_Algorithm WorkerThread MMAClass)
target_include_directories(cpu_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_directories(cpu_test PRIVATE ${CMAKE_BINARY_DIR})


# 安装目标
install(TARGETS cpu_test WorkerThread MMAClass Matrix_Mul_Algorithm
    RUNTIME DESTINATION ${MY_INSTALL_DIR}/bin
    LIBRARY DESTINATION ${MY_INSTALL_DIR}/lib
    ARCHIVE DESTINATION ${MY_INSTALL_DIR}/lib
)

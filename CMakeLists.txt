cmake_minimum_required(VERSION 3.0)

# DEFINE name of project
project(Test_1)

find_package( OpenCV REQUIRED )
find_package( OpenCL REQUIRED )

# SET source
set(SOURCE test/test_1.cpp)

# add execution
add_executable(${PROJECT_NAME} ${SOURCE})

# DEFINE include glm
include_directories("${CMAKE_SOURCE_DIR}/include/glm-0.9.5.3")
include_directories("${CMAKE_SOURCE_DIR}/test")

# DEFINE OpenCV Library
target_link_libraries(Test_1 ${OpenCV_LIBS})

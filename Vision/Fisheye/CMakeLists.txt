cmake_minimum_required(VERSION 2.8.3)
project(eurobot_fisheye)

#BUIDLING OPTIONS
set(CMAKE_BUILD_TYPE RelWithDebInfo)
add_compile_options(-std=c++11)
add_definitions(-fPIC -shared)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/exec)

#CONFIGURATION FILE
configure_file(src/fisheye_calibration-in.cpp ${CMAKE_SOURCE_DIR}/src/fisheye_calibration.cpp)

#PACKAGES AND DEPENDENCIES
find_package(
	OpenCV REQUIRED
)

#INCLUDE DIRECTORIES
include_directories(
	include/Fisheye/
)

#EXECUTABLES
add_executable(fisheye_calibration src/fisheye_calibration.cpp)
target_link_libraries(fisheye_calibration ${OpenCV_LIBS})

cmake_minimum_required(VERSION 3.1)
 project(StartProject)
 find_package(OpenCV REQUIRED)

 set(CMAKE_CXX_STANDARD 11)
 set(CMAKE_CXX_STANDARD_REQUIRED True)

 set(OpenCV_DIR /home/mrhause/installation/OpenCV-master/lib/cmake/opencv4)
 include_directories($(OpenCV_INCLUDE_DIRS))
 add_executable(StartProject main.cpp)
 target_link_libraries(StartProject ${OpenCV_LIBS} )
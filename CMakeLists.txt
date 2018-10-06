project(Pooh_Bear_Intrusion_Detection_System CXX)
cmake_minimum_required(VERSION 3.10)

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/src)

set(CMAKE_CXX_STANDARD  98)
set(TRAFFIC_VERSION 0.1-dev)
set(dir ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${dir}/src/")
set(SOURCE_FILES src/main.cpp src/ActivityEngine.cpp src/ActivityEngine.h src/Vehicles.cpp src/Vehicles.h
        src/Logger.cpp src/Logger.h src/Helper.cpp src/Helper.h)

add_executable(Traffic ${SOURCE_FILES})

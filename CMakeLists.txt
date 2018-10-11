project(Pooh_Bear_Intrusion_Detection_System CXX)
cmake_minimum_required(VERSION 3.10)

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/src)

set(PYTHON_ADDITIONAL_VERSIONS 3.6)
find_package(PythonLibs REQUIRED)
# Path to the python library
set(PYTHON_LIBRARIES /home/codeninja/anaconda3/envs/pooh_bear_ids/lib/libpython3.6m.so)
# Path where Python.h is found
set(PYTHON_INCLUDE_DIRS /home/codeninja/anaconda3/envs/pooh_bear_ids/include/python3.6m)
# Path to the Python executable
set(PYTHON_EXECUTABLE /home/codeninja/anaconda3/envs/pooh_bear_ids/bin/python)
set(PYTHON_PACKAGES_PATH /home/codeninja/anaconda3/envs/pooh_bear_ids/lib/python3.6/site-packages)
include_directories(${PYTHON_INCLUDE_DIRS})
include_directories(${PYTHON_PACKAGES_PATH})

set(CMAKE_CXX_STANDARD 11)
set(TRAFFIC_VERSION 0.2-dev)
set(dir ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${dir}/src/")
set(SOURCE_FILES src/main.cpp src/ActivityEngine.cpp src/ActivityEngine.h src/Vehicles.cpp src/Vehicles.h
        src/Logger.h src/Utils.cpp src/Utils.h)

add_executable(Traffic ${SOURCE_FILES})
target_link_libraries(Traffic ${PYTHON_LIBRARIES})

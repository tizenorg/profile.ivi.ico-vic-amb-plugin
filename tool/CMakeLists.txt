cmake_minimum_required(VERSION 2.8)
include(CheckIncludeFiles)

set(ico_set_vehicleinfo_sources ico_set_vehicleinfo.c)
include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs} /usr/include/ico-util)
add_executable(ico_set_vehicleinfo ${ico_set_vehicleinfo_sources})
target_link_libraries(ico_set_vehicleinfo websockets -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries} -lico-util)

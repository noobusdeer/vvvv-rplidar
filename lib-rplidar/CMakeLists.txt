# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("lib-rplidar")


list( APPEND APP_SOURCES
	src/lib-rplidar.cpp
	sdk/src/rplidar_driver.cpp
    sdk/src/hal/thread.cpp
    sdk/src/arch/win32/net_serial.cpp
    sdk/src/arch/win32/net_socket.cpp
    sdk/src/arch/win32/timer.cpp
)
# Include sub-projects.
add_library (lib-rplidar SHARED ${APP_SOURCES})

target_include_directories(lib-rplidar PUBLIC include sdk/src sdk/include sdk/src/hal sdk/src/arch/win32)



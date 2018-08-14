# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("lib-rplidar")

# Include sub-projects.
add_library (lib-rplidar SHARED "src/lib-rplidar.cpp")
target_include_directories(lib-rplidar PUBLIC include)



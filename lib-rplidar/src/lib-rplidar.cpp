// lib-rplidar.cpp : Defines the entry point for the application.
//

#include "lib-rplidar.h"

using namespace rp::standalone::rplidar;

extern "C" int __declspec(dllexport) __stdcall testfunc(int iNumber) {
	return iNumber * 2;
}


RPlidarDriver * drv = NULL;
extern "C" void __declspec(dllexport) __stdcall test() {
	 drv = RPlidarDriver::CreateDriver(rp::standalone::rplidar::DRIVER_TYPE_SERIALPORT);
}
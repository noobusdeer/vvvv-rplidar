// lib-rplidar.cpp : Defines the entry point for the application.
//

#include "lib-rplidar.h"

extern "C" int __declspec(dllexport) __stdcall testfunc(int iNumber) {
	return iNumber * 2;
}
// lib-rplidar.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <Windows.h>

extern "C" int __declspec(dllexport) __stdcall testfunc(int iNumber) {
	return iNumber * 2;
}
#include "lib-rplidar.h"

using namespace rp::standalone::rplidar;



RPlidarDriver * drv = NULL;
rplidar_response_measurement_node_t nodes[8192];
u_result op_result;
size_t count;

bool scanned = false;
bool connectSuccess = false;

bool checkRPLIDARHealth( RPlidarDriver * drv ) {
    u_result     op_result;
    rplidar_response_device_health_t healthinfo;
    op_result = drv->getHealth(healthinfo);
    if (IS_OK(op_result)) { // the macro IS_OK is the preperred way to judge whether the operation is succeed.
        printf("RPLidar health status : %d\n", healthinfo.status);
        if (healthinfo.status == RPLIDAR_STATUS_ERROR) {
            fprintf(stderr, "Error, rplidar internal error detected. Please reboot the device to retry.\n");
            return false;
        } else {
            return true;
        }
    } else {
        fprintf(stderr, "Error, cannot retrieve the lidar health code: %x\n", op_result);
        return false;
    }
}

extern "C"  int __declspec(dllexport) __stdcall connecting(const char* portName) {
    rplidar_response_device_info_t devinfo;
    _u32 baudrateArray[2] = {115200, 256000};
    size_t baudRateArraySize = (sizeof(baudrateArray))/ (sizeof(baudrateArray[0]));
    connectSuccess = false;
    delete drv;
    drv = NULL;
    drv = RPlidarDriver::CreateDriver(DRIVER_TYPE_SERIALPORT);
    for(size_t i = 0; i < baudRateArraySize; ++i) {
        if(!drv) drv = RPlidarDriver::CreateDriver(DRIVER_TYPE_SERIALPORT);
        if(IS_OK(drv->connect(portName, baudrateArray[i]))) {         
            op_result = drv->getDeviceInfo(devinfo);
            if (IS_OK(op_result)) {
                connectSuccess = true;
                break;
            } else {
                delete drv;
                drv = NULL;
            }
        }
    }
    if( !connectSuccess ) return 0;
    if( !checkRPLIDARHealth(drv) ) return 0;
}

extern "C" void __declspec(dllexport) __stdcall startScan() {
    if(!drv) return;
    scanned = true;
    drv->startMotor();
    drv->startScan(0,1);
    
}

extern "C" void __declspec(dllexport) __stdcall stopScan() {
    if(!drv) return;
    scanned = false;
    drv->stop();
    drv->stopMotor();
}

DWORD WINAPI ThreadProc( __in  LPVOID lpParameter ) {
    //rplidar_response_measurement_node_t tmpnodes[8192];
    size_t tmpcount = _countof(nodes);
    op_result = drv->grabScanData(nodes, tmpcount);
    if (IS_OK(op_result)) {
        drv->ascendScanData(nodes, tmpcount);
    }
    //nodes[0] = tmpnodes[0];
    count = tmpcount;
    return 0;
}

extern "C" int __declspec(dllexport) __stdcall update() {
    if(!drv) return 0;
    if(!scanned) return 0;
    DWORD DllThreadID;
    HANDLE DllThread; //thread's handle
    DllThread=CreateThread(NULL,0,ThreadProc,0,0,&DllThreadID);
    if (DllThread != NULL)
    {
        CloseHandle(DllThread);
    }
    return (int)count;
}

extern "C" float __declspec(dllexport) __stdcall getAngle(int i) {
    if(!connectSuccess) return 0;
    if(!scanned) return 0;
    return ((nodes[i].angle_q6_checkbit >> RPLIDAR_RESP_MEASUREMENT_ANGLE_SHIFT)/64.0f);
}

extern "C" float __declspec(dllexport) __stdcall getDist(int i) {
    if(!connectSuccess) return 0;
    if(!scanned) return 0;
    return (nodes[i].distance_q2/4.0f);
}

extern "C" float __declspec(dllexport) __stdcall getQuality(int i) {
    if(!connectSuccess) return 0;
    if(!scanned) return 0;
    return (nodes[i].sync_quality >> RPLIDAR_RESP_MEASUREMENT_QUALITY_SHIFT);
}

extern "C" void __declspec(dllexport) __stdcall finish() {
    if(!drv) return;
    RPlidarDriver::DisposeDriver(drv);
    drv = NULL;
}
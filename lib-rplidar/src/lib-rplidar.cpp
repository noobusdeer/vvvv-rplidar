#include "lib-rplidar.h"

using namespace rp::standalone::rplidar;

RPlidarDriver * drv = NULL;
const char * opt_com_path = "\\\\.\\com3";
rplidar_response_measurement_node_t nodes[8192];
u_result op_result;
bool scanned = false;

bool checkRPLIDARHealth( RPlidarDriver * drv ) {
    u_result     op_result;
    rplidar_response_device_health_t healthinfo;
    op_result = drv->getHealth(healthinfo);
    if (IS_OK(op_result)) { // the macro IS_OK is the preperred way to judge whether the operation is succeed.
        printf("RPLidar health status : %d\n", healthinfo.status);
        if (healthinfo.status == RPLIDAR_STATUS_ERROR) {
            fprintf(stderr, "Error, rplidar internal error detected. Please reboot the device to retry.\n");
            return false;
        } 
        else {
            return true;
        }
    } 
    else {
        fprintf(stderr, "Error, cannot retrieve the lidar health code: %x\n", op_result);
        return false;
    }
}

extern "C" int __declspec(dllexport) __stdcall init() {
    drv = RPlidarDriver::CreateDriver(DRIVER_TYPE_SERIALPORT);
    _u32 baudrateArray[2] = {115200, 256000};

    if(!drv) return -1;
    rplidar_response_device_info_t devinfo;
    bool connectSuccess = false;
    size_t baudRateArraySize = (sizeof(baudrateArray))/ (sizeof(baudrateArray[0]));
    for(size_t i = 0; i < baudRateArraySize; ++i) {
        if(!drv) drv = RPlidarDriver::CreateDriver(DRIVER_TYPE_SERIALPORT);
        if(IS_OK(drv->connect(opt_com_path, baudrateArray[i]))) {         
            op_result = drv->getDeviceInfo(devinfo);
            if (IS_OK(op_result)) {
                connectSuccess = true;
                break;
            } 
            else {
                delete drv;
                drv = NULL;
            }
        }
    }
    if(!connectSuccess) return -1;
    if (!checkRPLIDARHealth(drv)) return -1;
    return 1;
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

extern "C" int __declspec(dllexport) __stdcall update() {
    if(!drv) return 0;
    if(!scanned) return 0;
    size_t count = _countof(nodes);
    op_result = drv->grabScanData(nodes, count);
    if (IS_OK(op_result)) {
        drv->ascendScanData(nodes, count);
    }
    return (int)count;
}

extern "C" float __declspec(dllexport) __stdcall getAngle(int i) {
    if(!drv) return 0;
    if(!scanned) return 0;
    return ((nodes[i].angle_q6_checkbit >> RPLIDAR_RESP_MEASUREMENT_ANGLE_SHIFT)/64.0f);
}

extern "C" float __declspec(dllexport) __stdcall getDist(int i) {
    if(!drv) return 0;
    if(!scanned) return 0;
    return (nodes[i].distance_q2/4.0f);
}

extern "C" float __declspec(dllexport) __stdcall getQuality(int i) {
    if(!drv) return 0;
    if(!scanned) return 0;
    return (nodes[i].sync_quality >> RPLIDAR_RESP_MEASUREMENT_QUALITY_SHIFT);
}

extern "C" void __declspec(dllexport) __stdcall finish() {
    if(!drv) return;
    RPlidarDriver::DisposeDriver(drv);
    drv = NULL;
}
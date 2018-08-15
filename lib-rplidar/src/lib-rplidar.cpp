#include "lib-rplidar.h"

using namespace rp::standalone::rplidar;

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

RPlidarDriver * drv = NULL;
const char * opt_com_path = "\\\\.\\com3";
_u32         baudrateArray[2] = {115200, 256000};
u_result     op_result;

#include <signal.h>
bool ctrl_c_pressed;
void ctrlc(int)
{
  ctrl_c_pressed = true;
}

extern "C" int __declspec(dllexport) __stdcall init() {
  drv = RPlidarDriver::CreateDriver(DRIVER_TYPE_SERIALPORT);
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
  signal(SIGINT, ctrlc);
  return 1;
}

extern "C" void __declspec(dllexport) __stdcall startScan() {
  if(!drv) return;
  drv->startMotor();
  drv->startScan(0,1);
}


extern "C" void __declspec(dllexport) __stdcall stopScan() {
  if(!drv) return;
  drv->stop();
  drv->stopMotor();
}



/*
 * @Descripttion: 
 * @version: 
 * @Author: Elegoo
 * @Date: 2020-06-04 11:42:27
 * @LastEditors: Changhua
 * @LastEditTime: 2020-07-23 14:21:48
 */

#ifndef _CameraWebServer_AP_H
#define _CameraWebServer_AP_H
#include "esp_camera.h"
#include <WiFi.h>

class CameraWebServer_AP
{

public:
  void CameraWebServer_AP_Init(void);
  void PingWorker(void);
  String wifi_name;  
private:
  void connectToWifi();
  camera_config_t loadConfig(void);
  void readRemoteSettings(void);
  void parseJsonResponse(const char* jsonResponse);
  void estimateMemoryUsage(void);
};

#endif

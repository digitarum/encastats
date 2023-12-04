/*
 * @Descripttion: 
 * @version: 
 * @Author: Elegoo
 * @Date: 2020-06-04 11:42:27
 * @LastEditors: Changhua
 * @LastEditTime: 2020-07-23 14:21:48
 */

#ifndef _CommandWebServer_H
#define _CommandWßebServer_H
#include <ESPAsyncWebSrv.h>

class CommandWebServer
{
public:
  static void Begin(void);
private:
  static AsyncWebServer server;
  static void handleForward(AsyncWebServerRequest *request);
  static void handleBackward(AsyncWebServerRequest *request);
  static void handleLeft(AsyncWebServerRequest *request);
  static void handleRight(AsyncWebServerRequest *request);
  static void handleNotFound(AsyncWebServerRequest *request);
  static void handleCors(AsyncWebServerResponse *response);
};

#endif

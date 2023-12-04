/*
 * @Description:
 * @version: 
 * @Author: u1062049, based on original code by Elegoo
 * @Date: 2023-11-18 23:53:27
 * @LastEditors: u1062049 (based on original code by Changhua)
 * @LastEditTime: 2023-11-18
 */

#include "CameraWebServer_AP.h"
#include "CommandWebServer.h"
#include <WiFi.h>
#include "esp_camera.h"

#define RXD2 33
#define TXD2 4
CameraWebServer_AP CameraWebServerAP;
CommandWebServer CommandWebServer;

bool WA_en = false;

#define RXD2 33
#define TXD2 4

void setup()
{
  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  CameraWebServerAP.CameraWebServer_AP_Init();

  // give time to the program to finish
  delay(100);
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);

  CommandWebServer.Begin();
}

// Ping
unsigned long lastPingTime = 0;
const unsigned long pingInterval = 5000; // How many milliseconds to wait for each ping

void loop()
{
  if (millis() - lastPingTime >= pingInterval) {
    // todo: restore ping
    Serial.println("camera ping: Serial");
    Serial2.println("camera ping: Serial2");
    // CameraWebServerAP.PingWorker();
    lastPingTime = millis();
  }
}

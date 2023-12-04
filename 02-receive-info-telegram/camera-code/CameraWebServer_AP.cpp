
// WARNING!!! Make sure that you have selected Board ---> ESP32 Dev Module
//            Partition Scheme ---> Huge APP (3MB No OTA/1MB SPIFFS)
//            PSRAM ---> enabled

#define CAMERA_MODEL_M5STACK_WIDE

#include "CameraWebServer_AP.h"
#include "camera_pins.h"
#include "esp_system.h"
#include "credentials.h"

#include <WiFi.h>
#include <HTTPClient.h>

#include <iostream>
#include <sstream>
#include <string>


/* * * * * * * * * * * * * * * * * * * * * * */
/* * *  Send HTTP Turn On    * * * * * * * * */
/* * * * * * * * * * * * * * * * * * * * * * */
void sendTurnOnRequest() {
  HTTPClient http;
  std::ostringstream requestBodyStream;

  requestBodyStream << "{\"message\":{\"chat\":{\"id\":" << TELEGRAM_CHAT_ID << "},\"text\":\"turn on\", \"info\":\"Camera server: " << WiFi.localIP().toString().c_str() <<  "\"}}";

  // Serial.print("Calling URL:");
  // Serial.println(CLOUDFLARE_WORKER_URL);
  //
  // Serial.print("Request body:");
  // Serial.println(requestBodyStream.str().c_str());

  http.begin(CLOUDFLARE_WORKER_URL);
  http.addHeader("Content-Type", "application/json");

  std::string requestBody = requestBodyStream.str();
  int httpResponseCode = http.POST(requestBody.c_str());

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    String response = http.getString();
    Serial.println(response);
  } else {
    Serial.print("HTTP Request failed. Error code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

/* * * * * * * * * * * * * * * * * * * * * * */
/* * *  Ping the worker  * * * * * * * * * * */
/* * * * * * * * * * * * * * * * * * * * * * */
void CameraWebServer_AP::PingWorker(void) {
  HTTPClient http;
  std::ostringstream requestBodyStream;

  requestBodyStream << "{\"message\":{\"chat\":{\"id\":" << TELEGRAM_CHAT_ID << "},\"text\":\"ping\"}}";

  Serial.print("Calling URL: ");
  Serial.println(CLOUDFLARE_WORKER_URL);
  
  Serial.print(requestBodyStream.str().c_str());

  // Get the concatenated string
  std::string requestBody = requestBodyStream.str();

  http.begin(CLOUDFLARE_WORKER_URL);
  http.addHeader("Content-Type", "application/json");

  int httpResponseCode = http.POST(requestBody.c_str());

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    String response = http.getString();
    Serial.println(response);
  } else {
    Serial.print("HTTP Request failed. Error code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

void startCameraServer();

/* * * * * * * * * * * * * * * * * * * * * * */
/* * *  Load the config  * * * * * * * * * * */
/* * * * * * * * * * * * * * * * * * * * * * */
camera_config_t CameraWebServer_AP::loadConfig(void) {
  camera_config_t config;

  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000; // previously: 10000000
  config.pixel_format = PIXFORMAT_JPEG;
  //init with high specs to pre-allocate larger buffers
  if (psramFound())
  {
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  }
  else
  {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  return config;
}

/* * * * * * * * * * * * * * * * * * * * * * */
/* * *  Connect to the wifi  * * * * * * * * */
/* * * * * * * * * * * * * * * * * * * * * * */
void CameraWebServer_AP::connectToWifi() {
  uint64_t chipid = ESP.getEfuseMac();
  char string[10];
  sprintf(string, "%04X", (uint16_t)(chipid >> 32));
  String mac0_default = String(string);
  sprintf(string, "%08X", (uint32_t)chipid);
  String mac1_default = String(string);
  String url = SSID + mac0_default + mac1_default;
  const char *mac_default = url.c_str();

  Serial.println(":----------------------------:");
  Serial.print("wifi_name:");
  Serial.println(mac_default);
  Serial.println(":----------------------------:");
  wifi_name = mac0_default + mac1_default;

  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(666);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  sendTurnOnRequest();
  startCameraServer();

  Serial.print("Camera server is ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");
}

/* * * * * * * * * * * * * * * * * * * * * * */
/* * *  Initialize the Camera Server * * * * */
/* * * * * * * * * * * * * * * * * * * * * * */
void CameraWebServer_AP::CameraWebServer_AP_Init(void)
{
  camera_config_t config = loadConfig();

#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif
  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK)
  {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
  sensor_t *s = esp_camera_sensor_get();
  //drop down frame size for higher initial frame rate
  //s->set_framesize(s, FRAMESIZE_SXGA); //字节长度采样值:60000                 #9 (画质高)  1280x1024
  s->set_framesize(s, FRAMESIZE_SVGA); //字节长度采样值:40000                   #7 (画质中)  800x600
  // s->set_framesize(s, FRAMESIZE_QVGA); //字节长度采样值:10000                #4 (画质低)  320x240

#if defined(CAMERA_MODEL_M5STACK_WIDE)
  s->set_vflip(s, 0);
  s->set_hmirror(s, 1);
#endif
  s->set_vflip(s, 0);   //图片方向设置（上下）
  s->set_hmirror(s, 0); //图片方向设置（左右）

  // s->set_vflip(s, 1);   //图片方向设置（上下）
  // s->set_hmirror(s, 1); //图片方向设置（左右）

  Serial.println("\r\n");

  connectToWifi();
  // readRemoteSettings();
}

/* * * * * * * * * * * * * * * * * * * * * * */
/* * *  Parse an HTTP response into JSON * * */
/* * * * * * * * * * * * * * * * * * * * * * */
// void CameraWebServer_AP::parseJsonResponse(const char* jsonResponse) {
//   // estimateMemoryUsage();
//
//   // const size_t bufferSize = JSON_OBJECT_SIZE(3) + 150;
//   DynamicJsonDocument doc(150);
//
//   // Parse the JSON response
//   DeserializationError error = deserializeJson(doc, jsonResponse);
//
//   // Check for parsing errors
//   if (error) {
//     Serial.print(F("Failed to parse JSON: "));
//     Serial.println(error.c_str());
//     return;
//   }

  // // Extract values from the JSON document
  // // const char* ssid = strdup(doc["ssid"]);
  // // const char* password = strdup(doc["password"]);
  // // long long telegramChatId = strdup(doc["chatId"]);
  // SSID = strdup(doc["ssid"]);
  // PASSWORD = strdup(doc["password"]);
  // long long telegramChatId = doc["chatId"];
  //
  // // Print the extracted values (you can remove this part in the actual code)
  // Serial.print(F("SSID: "));
  // Serial.println(SSID);
  // Serial.print(F("Password: "));
  // Serial.println(PASSWORD);
  // Serial.print(F("Telegram Chat ID: "));
  // Serial.println(telegramChatId);
  //
  // // Now you can use these values in your ESP32 code as needed
  // // For example, you can assign them to your variables:
  // // SSID = ssid;
  // // PASSWORD = password;
  // // TELEGRAM_CHAT_ID = telegramChatId;
// }

/* * * * * * * * * * * * * * * * * * * * * * * * * * */
/* * *  Used to estimate size of JSON responses  * * */
/* * * * * * * * * * * * * * * * * * * * * * * * * * */
// void CameraWebServer_AP::estimateMemoryUsage(void) {
//   // Create a JSON document with estimated sizes
//   const size_t bufferSize = JSON_OBJECT_SIZE(3) + 150;
//   DynamicJsonDocument doc(bufferSize);

//   // Add data to the JSON document
//   doc["ssid"] = "XX_XX";
//   doc["password"] = "5MxxxxPaaaaaaaaaabbbbbbBbBbbb";
//   doc["chatId"] = "1111262831";

//   // Estimate memory usage
//   Serial.print("Estimated memory usage: ");
//   Serial.println(doc.memoryUsage());
// }


/* * * * * * * * * * * * * * * * * * * * * * */
/* * *  Read the config from the Worker  * * */
/* * * * * * * * * * * * * * * * * * * * * * */
// void CameraWebServer_AP::readRemoteSettings(void) {
//   HTTPClient http;
  
//   Serial.print("Calling Settings URL: ");
//   Serial.println(SETTINGS_WORKER_URL);
  
//   http.begin(SETTINGS_WORKER_URL);
//   int httpResponseCode = http.GET();

//   if (httpResponseCode > 0) {
//     Serial.print("Settings Response code: ");
//     Serial.println(httpResponseCode);

//     String response = http.getString();
//     Serial.println("Settings Response: ");
//     Serial.println(response);

//     parseJsonResponse(response.c_str());
//   } else {
//     Serial.print("HTTP Request failed. Error code: ");
//     Serial.println(httpResponseCode);
//   }

//   http.end();
// }

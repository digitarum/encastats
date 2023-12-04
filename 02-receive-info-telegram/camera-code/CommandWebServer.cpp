/*
 * @Description: A Web server that listens to HTTP commands
 * @version: 0.0.1
 * @Author: long
 * @Date: 2023-12-02
 * @LastEditors: long
 * @LastEditTime: 2020-12-02
 */
#include "CommandWebServer.h"
#include <ESPAsyncWebSrv.h>

AsyncWebServer CommandWebServer::server(8080);

void CommandWebServer::Begin(void) {
  server.on("/forward", HTTP_GET, handleForward);
  server.on("/backward", HTTP_GET, handleBackward);
  server.on("/left", HTTP_GET, handleLeft);
  server.on("/right", HTTP_GET, handleRight);

  server.onNotFound(handleNotFound);

  // Begin the server
  server.begin();
}

void CommandWebServer::handleForward(AsyncWebServerRequest *request) {
  Serial.println("forward");
  
  AsyncWebServerResponse *response = request->beginResponse(200, "text/plain",  "Ok");
  handleCors(response);

	request->send(response);
}

void CommandWebServer::handleBackward(AsyncWebServerRequest *request) {
  Serial.println("backward");

  AsyncWebServerResponse *response = request->beginResponse(200, "text/plain",  "Ok");
  handleCors(response);

	request->send(response);
}

void CommandWebServer::handleLeft(AsyncWebServerRequest *request) {
  Serial.println("left");

  AsyncWebServerResponse *response = request->beginResponse(200, "text/plain",  "Ok");
  handleCors(response);

	request->send(response);
}

void CommandWebServer::handleRight(AsyncWebServerRequest *request) {
  Serial.println("right");
  
  AsyncWebServerResponse *response = request->beginResponse(200, "text/plain",  "Ok");
  handleCors(response);

	request->send(response);
}

void CommandWebServer::handleNotFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not Found");
}

void CommandWebServer::handleCors(AsyncWebServerResponse *response) {
  response->addHeader("Access-Control-Allow-Origin", "*");
  response->addHeader("Access-Control-Allow-Methods", "GET,POST,OPTIONS");
  response->addHeader("Access-Control-Allow-Headers", "*");
}


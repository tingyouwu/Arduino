#ifndef _MAIN_H__
#define _MAIN_H__


extern "C" {
#include "user_interface.h"
#include "smartconfig.h"
}

/************** ESP8266相关操作 **************************/
void delayRestart(float t);
void delayNs(uint8_t m);
/*********************************************************/

/*************** OneNet HTTP相关操作 *********************/
WiFiClient client;
HTTPClient http;
String response;
const char* host = "api.heclouds.com";
bool postToDeviceDataPoint(String data);
/*********************************************************/

/*********************** 项目相关操作 ********************/
#define VER             "HTTP_TEMP_V1.1"
const char* ssid = "TP-LINK_5344";//wifi账号
const char* password = "6206908you11011010";//wifi秘密
const unsigned long BAUD_RATE = 115200;// serial connection speed
unsigned long lastWiFiCheckTick = 0;
bool ledState = 0;
unsigned long lastOneNetPostTick=0;
/*********************************************************/

#endif

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

/******************** Http 相关操作 **********************/
const unsigned long HTTP_TIMEOUT = 5000;// max respone time from server
const size_t MAX_CONTENT_SIZE = 500;// max size of the HTTP response

WiFiClient client;
char response[MAX_CONTENT_SIZE];
char endOfHeaders[] = "\r\n\r\n";

bool sendGetRequest(const char* host,String url,String header,char* response);
bool sendPostRequest(const char* host,String url,String header,String postData,char* response);
bool skipResponseHeaders();
void readReponseContent(char* content, size_t maxSize);
void clearResponseBuffer(char* response);
/*********************************************************/

/*************** OneNet HTTP相关操作 *********************/
const char* host = "api.heclouds.com";
bool postToDeviceDataPoint(String data);
/*********************************************************/

/*********************** 项目相关操作 ********************/
#define VER             "HTTP_TEMP_V1.0"
const char* ssid = "TP-LINK_5344";//wifi账号
const char* password = "6206908you11011010";//wifi秘密
const unsigned long BAUD_RATE = 115200;// serial connection speed
unsigned long lastWiFiCheckTick = 0;
bool ledState = 0;
unsigned long lastOneNetPostTick=0;
unsigned long preTCPStartTick=0;
bool preTCPConnected=false;
/*********************************************************/

#endif

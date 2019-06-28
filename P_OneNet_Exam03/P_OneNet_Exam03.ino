/**
 *  功能：ESP8266 HTTP 远程上传温度值 V1.0
 *  作者：单片机菜鸟
 *  时间：2019-06-27
 *  描述：
 *      1.OneNet平台端：创建Http协议的产品，创建DeviceA设备点
 *      2.每个20s上传一个随机值给到OneNet平台
*/

#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <stdlib.h>
#include <Ticker.h>
#include "H_project.h"

void initSystem();
void wifiTick();
bool postTempToOneNet();

int retry = 0;

/**
 * 初始化
 */
void setup() {
  initSystem();
}

void loop() {
  ESP.wdtFeed();
  wifiTick();

  if(millis() - lastOneNetPostTick > 30*1000){
     lastOneNetPostTick = millis();
     if(postTempToOneNet()){
       retry = 0;
     }else{
       retry ++;
       if(retry == 20){
         retry = 0;
         delayRestart(1);
       }
     }
  }
}

/**
 * 初始化系统
 */
void initSystem(){
    int cnt = 0;
    Serial.begin (115200);
    Serial.println("\r\n\r\nStart ESP8266 HTTP");
    Serial.print("Firmware Version:");
    Serial.println(VER);
    Serial.print("SDK Version:");
    Serial.println(ESP.getSdkVersion());
    wifi_station_set_auto_connect(0);//关闭自动连接
    ESP.wdtEnable(5000);
    WiFi.disconnect();
    delay(100);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
          delay(500);
          cnt++;
          Serial.print(".");
          if(cnt>=40){
            cnt = 0;
            //重启系统
            delayRestart(1);
          }
    }
    pinMode(LED_BUILTIN, OUTPUT);
}

/*
*  WiFiTick
*  检查是否需要初始化WiFi
*  检查WiFi是否连接上
*  控制指示灯
*/
void wifiTick(){
  static bool ledTurnon = false;
   if ( WiFi.status() != WL_CONNECTED ) {
       if (millis() - lastWiFiCheckTick > 1000) {
         lastWiFiCheckTick = millis();
         ledState = !ledState; digitalWrite(LED_BUILTIN, ledState);
         ledTurnon = false;
       }
    }else{
       if (ledTurnon == false) {
             ledTurnon = true;
             digitalWrite(LED_BUILTIN, 0);
        }
    }
}

/**
 * 提交数据到OneNet
 */
bool postTempToOneNet(){
   if(WiFi.status() != WL_CONNECTED) return false;
    Serial.println("TCP Client postTempToOneNet.");

    StaticJsonBuffer<230> jsonBuffer;
    //创建根，也就是顶节点
    JsonObject& root = jsonBuffer.createObject();
    //在root对象中加入data数组
    JsonArray& datastreams = root.createNestedArray("datastreams");
    JsonObject& stream = datastreams.createNestedObject();
    stream["id"] = "temperature";
    JsonArray& datapoints = stream.createNestedArray("datapoints");
    JsonObject& value1 = datapoints.createNestedObject();
    value1["value"] =  random(20, 50);
    JsonObject& value2 = datapoints.createNestedObject();
    value2["value"] =  random(20, 50);
       
    int len = root.measureLength();
    char buffer[100];
    root.printTo(buffer, 100);
    String data;
    for(int index = 0;index<len;index++){
           data += buffer[index];
    }
    return postToDeviceDataPoint(data);
}

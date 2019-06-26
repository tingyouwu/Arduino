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

/*************** OneNet MQTT相关操作 ****************************/
void initOneNet(uint8_t *productId,uint8_t *apiKey,uint8_t *deviceId);
int connectToOneNetMqtt();
/*********************************************************/

#define ONENET_DISCONNECTED 1 //已经断开
#define ONENET_CONNECTED 2    //已经连接上
#define ONENET_RECONNECT 3    //重连成功

//常量
#define VER             "MQTT_LED_V1.0"
const char* ssid = "TP-LINK_5344";//wifi账号
const char* password = "6206908you11011010";//wifi秘密

//OneNet相关
PubSubClient mqttClient;
const char* mqttServer = "183.230.40.39";//mqtt服务器
const uint16_t mqttPort = 6002;
#define PRODUCT_ID    "253190" //此为博哥自己的产品id 请新建自己的
#define API_KEY    "aat9ivuJls3gcAFWnLoxfbwW8bI="
#define DEVICE_ID "531724199"
#define TOPIC     "esp8266led"

unsigned long lastWiFiCheckTick = 0;
bool ledState = 0;

#endif

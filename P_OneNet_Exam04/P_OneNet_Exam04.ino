/**
 *  功能：ESP8266 Mqtt客户端自注冊功能，订阅OneNet Mqtt工具发过来的控制Led消息
 *  作者：单片机菜鸟
 *  时间：2019-06-30
 *  描述：
 *      1.初始化工作：初始化网络配置，Mqtt客户端自注冊，连接鉴权，订阅主题
 *      2.订阅消息：获取发送过来的消息（json格式），解析消息，实现控制亮灭灯
 *  技术QQ学习群：1.ESP8266&32物联网开发之旅（491507716）
 *                2.ESP8266&32 物联网开发（622368884）
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <Ticker.h>
#include "H_project.h"

#define MAGIC_NUMBER 0xAA

int state;
WiFiClient espClient;

//声明方法
void initSystem();
void initOneNetMqtt();
void callback(char* topic, byte* payload, unsigned int length);
void saveConfig();
void loadConfig();
bool parseRegisterResponse();

/**
 * 初始化
 */
void setup() {
  initSystem();
  initOneNetMqtt();
}

void loop() {
  ESP.wdtFeed();
  state = connectToOneNetMqtt();
  if(state == ONENET_RECONNECT){
     //重连成功 需要重新注册
     mqttClient.subscribe(TOPIC,1);
     mqttClient.loop();
  }else if(state == ONENET_CONNECTED){
     mqttClient.loop();
  }
  delay(2000);
}

void initSystem(){
    int cnt = 0;
    Serial.begin (115200);
    Serial.println("\r\n\r\nStart ESP8266 MQTT");
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

    loadConfig();
    //还没有注册
    if(strcmp(config.deviceid,DEFAULT_ID) == 0){
        int tryAgain = 0;
        while(!registerDeviceToOneNet()){
          Serial.print(".");
          delay(500);
          tryAgain++;
          if(tryAgain == 5){
            //尝试5次
            tryAgain = 0;
            //重启系统
            delayRestart(1);
          }
        }
        if(!parseRegisterResponse()){
            //重启系统
            delayRestart(1);
            while(1);
        }
    }
}

void initOneNetMqtt(){
    mqttClient.setServer(mqttServer,mqttPort);
    mqttClient.setClient(espClient);
    mqttClient.setCallback(callback);

    initOneNet(PRODUCT_ID,API_KEY,config.deviceid);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if ((char)payload[0] == '1') {
    digitalWrite(LED_BUILTIN, LOW);
  } else {
    digitalWrite(LED_BUILTIN, HIGH);
  }
}

/*
 * 保存参数到EEPROM
*/
void saveConfig()
{
  Serial.println("Save OneNet config!");
  Serial.print("deviceId:");
  Serial.println(config.deviceid);

  EEPROM.begin(150);
  uint8_t *p = (uint8_t*)(&config);
  for (int i = 0; i < sizeof(config); i++)
  {
    EEPROM.write(i, *(p + i));
  }
  EEPROM.commit();
}

/*
 * 从EEPROM加载参数
*/
void loadConfig()
{
  EEPROM.begin(150);
  uint8_t *p = (uint8_t*)(&config);
  for (int i = 0; i < sizeof(config); i++)
  {
    *(p + i) = EEPROM.read(i);
  }
  EEPROM.commit();
  if (config.magic != MAGIC_NUMBER)
  {
    strcpy(config.deviceid, DEFAULT_ID);
    config.magic = MAGIC_NUMBER;
    saveConfig();
    Serial.println("Restore config!");
  }
  Serial.println("-----Read config-----");
  Serial.print("deviceId:");
  Serial.println(config.deviceid);
  Serial.println("-------------------");
}

/**
 * 解析注册返回结果
 */
bool parseRegisterResponse(){
   Serial.println("start parseRegisterResponse");
   StaticJsonBuffer<200> jsonBuffer;
     // StaticJsonBuffer 在栈区分配内存   它也可以被 DynamicJsonBuffer（内存在堆区分配） 代替
     // DynamicJsonBuffer  jsonBuffer;
   JsonObject& root = jsonBuffer.parseObject(response);

     // Test if parsing succeeds.
   if (!root.success()) {
       Serial.println("parseObject() failed");
       return false;
   }

   int errno = root["errno"];
   if(errno !=0){
       Serial.println("register failed!");
       return false;
   }else{
       Serial.println("register sucess!");
       strcpy(config.deviceid, root["data"]["device_id"]);
       saveConfig();
       return true;
   }
}
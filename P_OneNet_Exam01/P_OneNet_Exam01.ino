/**
 *  功能：ESP8266 Mqtt客户端调试主题功能，订阅OneNet Mqtt工具发过来的控制Led消息
 *  作者：单片机菜鸟
 *  时间：2019-06-25
 *  描述：
 *      1.OneNet平台端：提前在OneNet平台创建DeviceA，DeviceB两个设备点，DeviceA属于NodeMcu，DeviceB属于OneNet Mqtt调试工具
 *      2.初始化工作：初始化网络配置，Mqtt客户端配置，连接鉴权，订阅主题
 *      3.订阅消息：获取发送过来的消息（json格式），解析消息，实现控制亮灭灯
 *  技术QQ学习群：1.ESP8266&32物联网开发之旅（491507716）
 *                2.ESP8266&32 物联网开发（622368884）
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Ticker.h>
#include "H_project.h"

int state;
WiFiClient espClient;

//声明方法
void initSystem();
void initOneNetMqtt();
void callback(char* topic, byte* payload, unsigned int length);

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
}

void initOneNetMqtt(){
    mqttClient.setServer(mqttServer,mqttPort);
    mqttClient.setClient(espClient);
    mqttClient.setCallback(callback);

    initOneNet(PRODUCT_ID,API_KEY,DEVICE_ID);
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

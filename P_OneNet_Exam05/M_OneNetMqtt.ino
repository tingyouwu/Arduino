/**
 * 功能：OneNet Mqtt操作方法库
 * 作者：单片机菜鸟
 * 时间：20190626
 */

#include "H_project.h"

const char *onenet_productId;
const char *onenet_apiKey;
const char *onenet_deviceId;

/**
 * 初始化OneNet
 * @param productId 产品id
 * @param apiKey    鉴权信息
 * @param deviceId  设备id
 * @param topic     订阅主题
 */
void initOneNet(const char *productId,const char *apiKey,const char *deviceId){
   onenet_productId = productId;
   onenet_apiKey = apiKey;
   onenet_deviceId = deviceId;
}

/**
 * 连接OneNet鉴权
 * @param bool true  连接成功
 *             false 连接失败
 */
int connectToOneNetMqtt(){
    int cnt = 0;
    while(!mqttClient.connected()){
       ESP.wdtFeed();
       cnt++;
       Serial.println("Connect to OneNet MQTT...");
       /**
        * 参考OneNet mqtt连接报文
        * ClientIdentifier: 创建设备时得到的设备ID
        * UserName: 注册产品时，平台分配的产品ID
        * UserPassword: 为设备的鉴权信息（即唯一设备编号，SN），或者为apiKey
        */
       if (mqttClient.connect(onenet_deviceId,onenet_productId,onenet_apiKey)) {
             Serial.println("connect success!");
             return ONENET_RECONNECT;
        } else {
             Serial.print("connect fail!");
             Serial.println(" try again in 5 seconds");
             // Wait 5 seconds before retrying
             delayNs(5);
        }
        if(cnt >=10){
          //只做10次连接到OneNet，连接不上重启8266
          cnt = 0;
          delayRestart(1);
        }
    }
    return ONENET_CONNECTED;
}

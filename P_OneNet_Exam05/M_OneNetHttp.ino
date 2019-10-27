/**
 * 功能：OneNet Http操作方法库
 * 作者：单片机菜鸟
 * 时间：2019-06-28
 */

#include "H_project.h"

const char* url = "/devices";//设备自注册

extern HTTPClient http;

/**
* 设备自注册
* {
*      "title": "ESP_XXXXXXX",
*      "desc": "ESP_XXXXXXX",
*      "auth_info": "ESP_XXXXXXX",
*  }
*/
bool registerDeviceToOneNet(){

    if(WiFi.status() != WL_CONNECTED) return false;
    Serial.println("RegisterDevice To OneNet.");

    StaticJsonBuffer<250> jsonBuffer;
    //创建根，也就是顶节点
    JsonObject& root = jsonBuffer.createObject();
    root["title"] = "ESP-" + WiFi.macAddress();
    root["auth_info"] = "ESP-" + WiFi.macAddress();
    int len = root.measureLength();
    char buffer[200];
    root.printTo(buffer, 200);
    String data;
    for(int index = 0;index<len;index++){
        data += buffer[index];
    }

    Serial.println(data);

    http.begin(host,80,url);
    http.addHeader("api-key",API_KEY);

    int httpCode = http.POST(data);
    if(httpCode > 0){
        Serial.printf("[HTTP] POST... code: %d\n", httpCode);
        //判断请求是否成功
        if (httpCode == HTTP_CODE_OK) {
             //读取响应内容
             response = http.getString();
             Serial.println("Get the data from Internet!");
             Serial.println(response);
             return true;
         }
    }else {
        Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
        return false;
    }
}

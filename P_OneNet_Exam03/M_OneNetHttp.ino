/**
 * 功能：OneNet Http操作方法库
 * 作者：单片机菜鸟
 * 时间：2019-06-28
 */

#include "H_project.h"

const char* apiKey = "IdlLKSU0=NoEwdXWVG5CT4tX2h0=";//API KEY
const char* url = "/devices/532576793/datapoints";

extern HTTPClient http;

/**
* 给设备新增一个数据点
*/
bool postToDeviceDataPoint(String data){
    http.begin(host,80,url);
    http.addHeader("api-key",apiKey);

    int httpCode = http.POST(data);
    if(httpCode > 0){
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);
        //判断请求是否成功
        if (httpCode == HTTP_CODE_OK) {
             //读取响应内容
             response = http.getString();
             Serial.println("Get the data from Internet!");
             Serial.println(response);
             return true;
         }
    }else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        return false;
    }
}
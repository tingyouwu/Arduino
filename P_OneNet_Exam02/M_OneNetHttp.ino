/**
 * 功能：OneNet Http操作方法库
 * 作者：单片机菜鸟
 * 时间：20190626
 */

#include "H_project.h"

const char* apiKey = "IdlLKSU0=NoEwdXWVG5CT4tX2h0=";//API KEY
const char* deviceUrl = "/devices/532576793";
const char* addDataPoint = "/datapoints";

extern WiFiClient client;

/**
* 给设备新增一个数据点
*/
bool postToDeviceDataPoint(String data){
    String url = deviceUrl;
    url += addDataPoint;

    String header = "api-key: ";
    header += apiKey;
    return sendPostRequest(host,url,header,data,response);
}
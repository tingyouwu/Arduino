/**
 * Demo：
 *    演示Http请求天气接口信息,演示响应头操作
 * @author 单片机菜鸟
 * @date 2019/09/09
 */
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>

//以下三个定义为调试定义
#define DebugBegin(baud_rate)    Serial.begin(baud_rate)
#define DebugPrintln(message)    Serial.println(message)
#define DebugPrint(message)    Serial.print(message)

const char* AP_SSID     = "TP-LINK_5344";         // XXXXXX -- 使用时请修改为当前你的 wifi ssid
const char* AP_PSK = "6206908you11011010";         // XXXXXX -- 使用时请修改为当前你的 wifi 密码
const char* HOST = "http://api.seniverse.com";
const char* APIKEY = "wcmquevztdy1jpca";        //API KEY
const char* CITY = "guangzhou";
const char* LANGUAGE = "zh-Hans";//zh-Hans 简体中文  会显示乱码
const char *keys[] = {"Content-Length","Content-Type","Connection","Date"};//需要收集的响应头的信息
  
const unsigned long BAUD_RATE = 115200;                   // serial connection speed
const unsigned long HTTP_TIMEOUT = 5000;               // max respone time from server;

HTTPClient http;
String GetUrl;
String response;

void setup() {
  // put your setup code here, to run once:
  WiFi.mode(WIFI_STA);     //设置esp8266 工作模式
  DebugBegin(BAUD_RATE);
  DebugPrint("Connecting to ");//写几句提示，哈哈
  DebugPrintln(AP_SSID);
  WiFi.begin(AP_SSID, AP_PSK);   //连接wifi
  WiFi.setAutoConnect(true);
  while (WiFi.status() != WL_CONNECTED) {
    //这个函数是wifi连接状态，返回wifi链接状态
    delay(500);
    DebugPrint(".");
  }
  DebugPrintln("");
  DebugPrintln("WiFi connected");
  DebugPrintln("IP address: " + WiFi.localIP());

  //拼接get请求url  博哥后面考虑看看是否可以封装一个方法来用用 不需要自己一个个拼装这个url
  GetUrl = String(HOST) + "/v3/weather/now.json?key=";
  GetUrl += APIKEY;
  GetUrl += "&location=";
  GetUrl += CITY;
  GetUrl += "&language=";
  GetUrl += LANGUAGE;
  //设置超时
  http.setTimeout(HTTP_TIMEOUT);
  //设置请求url
  http.begin(GetUrl);
  //以下为设置一些头  其实没什么用 最重要是后端服务器支持
  http.setUserAgent("esp8266");//用户代理版本
  http.setAuthorization("esp8266","boge");//用户校验信息
  http.addHeader("myname","cainiaobo");

  //设置获取响应头的信息
  http.collectHeaders(keys,4);
}

void loop() {
  //心知天气  发送http  get请求
  int httpCode = http.GET();
  if (httpCode > 0) {
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      //判断请求是否成功
      if (httpCode == HTTP_CODE_OK) {
        //读取响应内容
        response = http.getString();
        DebugPrintln("Get the data from Internet!");
        DebugPrintln(response);
        DebugPrintln(String("Content-Length:")+ http.header("Content-Length"));
        DebugPrintln(String("Content-Type:")+ http.header("Content-Type"));
        DebugPrintln(String("Connection:")+ http.header("Connection"));
        DebugPrintln(String("Date:")+ http.header("Date"));
      }
  } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
  delay(1000);//每1s调用一次 
}

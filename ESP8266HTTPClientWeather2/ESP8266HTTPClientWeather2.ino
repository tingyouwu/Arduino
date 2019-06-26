/**
 * Demo：
 *    演示Http请求天气接口信息 优化版本
 * @author 单片机菜鸟
 * @date 2019/09/16
 */
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>

#define DEBUG //是否开启debug功能

#ifdef DEBUG
  #define DebugBegin(baud_rate)    Serial.begin(baud_rate)
  #define DebugPrintln(message)    Serial.println(message)
  #define DebugPrint(message)    Serial.print(message)
#else
  #define DebugBegin(baud_rate)
  #define DebugPrintln(message)
  #define DebugPrint(message)
#endif

//以下数据存入flash
static const char AP_SSID[] ICACHE_RODATA_ATTR = "TP-LINK_5344";
static const char AP_PSK[] ICACHE_RODATA_ATTR = "TP-LINK_5344";
static const char HOST[] ICACHE_RODATA_ATTR = "TP-LINK_5344";
static const char APIKEY[] ICACHE_RODATA_ATTR = "TP-LINK_5344";
static const char CITY[] ICACHE_RODATA_ATTR = "TP-LINK_5344";
static const char LANGUAGE[] ICACHE_RODATA_ATTR = "TP-LINK_5344";

//const char* AP_SSID     = "TP-LINK_5344";         // XXXXXX -- 使用时请修改为当前你的 wifi ssid
//const char* AP_PSK = "6206908you11011010";         // XXXXXX -- 使用时请修改为当前你的 wifi 密码
//const char* HOST = "http://api.seniverse.com";
//const char* APIKEY = "wcmquevztdy1jpca";        //API KEY
//const char* CITY = "guangzhou";
//const char* LANGUAGE = "zh-Hans";//zh-Hans 简体中文  会显示乱码

// 我们要从此网页中提取的数据的类型
struct WeatherData {
  char city[16];//城市名称
  char weather[32];//天气介绍（多云...）
  char temp[16];//温度
  char udate[32];//更新时间
};

HTTPClient http;
String GetUrl;
String response;
WeatherData weatherData;

void setup() {
  // put your setup code here, to run once:
  WiFi.mode(WIFI_STA);     //设置esp8266 工作模式
  DebugBegin(115200);
  DebugPrint("Connecting to ");//写几句提示，哈哈
  DebugPrintln(AP_SSID);
  WiFi.begin(AP_SSID, AP_PSK);   //连接wifi
  WiFi.printDiag(Serial);
  WiFi.setAutoConnect(true);
  while (WiFi.status() != WL_CONNECTED) {
    //这个函数是wifi连接状态，返回wifi链接状态
    delay(500);
    DebugPrint(".");
    os_printf("");
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
  http.setTimeout(5000);
  //设置请求url
  http.begin(GetUrl);
  //以下为设置一些头  其实没什么用 最重要是后端服务器支持
  http.setUserAgent("esp8266");//用户代理版本
  http.setAuthorization("esp8266","boge");//用户校验信息
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
        //解析响应内容
        if (parseUserData(response, &weatherData)) {
          //打印响应内容
          printUserData(&weatherData);
        }
      }
  } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
  delay(1000);//每5s调用一次 
}
  
/**
 * @Desc 解析数据 Json解析
 * 数据格式如下：
 * {
 *    "results": [
 *        {
 *            "location": {
 *                "id": "WX4FBXXFKE4F",
 *                "name": "北京",
 *                "country": "CN",
 *                "path": "北京,北京,中国",
 *                "timezone": "Asia/Shanghai",
 *                "timezone_offset": "+08:00"
 *            },
 *            "now": {
 *                "text": "多云",
 *                "code": "4",
 *                "temperature": "23"
 *            },
 *            "last_update": "2017-09-13T09:51:00+08:00"
 *        }
 *    ]
 *}
 */
bool parseUserData(String content, struct WeatherData* weatherData) {
//    -- 根据我们需要解析的数据来计算JSON缓冲区最佳大小
//   如果你使用StaticJsonBuffer时才需要
//    const size_t BUFFER_SIZE = 1024;
//   在堆栈上分配一个临时内存池
//    StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;
//    -- 如果堆栈的内存池太大，使用 DynamicJsonBuffer jsonBuffer 代替
  DynamicJsonBuffer jsonBuffer;
   
  JsonObject& root = jsonBuffer.parseObject(content);
   
  if (!root.success()) {
    DebugPrintln("JSON parsing failed!");
    return false;
  }
    
  //复制我们感兴趣的字符串
  strcpy(weatherData->city, root["results"][0]["location"]["name"]);
  strcpy(weatherData->weather, root["results"][0]["now"]["text"]);
  strcpy(weatherData->temp, root["results"][0]["now"]["temperature"]);
  strcpy(weatherData->udate, root["results"][0]["last_update"]);
  //  -- 这不是强制复制，你可以使用指针，因为他们是指向“内容”缓冲区内，所以你需要确保
  //   当你读取字符串时它仍在内存中
  return true;
}
   
// 打印从JSON中提取的数据
void printUserData(const struct WeatherData* weatherData) {
//  DebugPrintln("Print parsed data :");
//  DebugPrint("City : ");
//  DebugPrint(weatherData->city);
//  DebugPrint(", \t");
//  DebugPrint("Weather : ");
//  DebugPrint(weatherData->weather);
//  DebugPrint(",\t");
//  DebugPrint("Temp : ");
//  DebugPrint(weatherData->temp);
//  DebugPrint(" C");
//  DebugPrint(",\t");
//  DebugPrint("Last Updata : ");
//  DebugPrint(weatherData->udate);
//  DebugPrintln("\r\n");
}

void debug_printf(char *fmt,...){
  
}

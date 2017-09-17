/**
* 日期:2017/09/13
* 功能：在线获取天气数据情况 心知天气API 
* 作者：单片机菜鸟
**/
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
 
const char* ssid     = "360wifilulu";         // XXXXXX -- 使用时请修改为当前你的 wifi ssid
const char* password = "6206908you11011010";         // XXXXXX -- 使用时请修改为当前你的 wifi 密码
const char* host = "api.seniverse.com";
const char* APIKEY = "wcmquevztdy1jpca";        //API KEY
const char* city = "guangzhou";
const char* language = "en";//zh-Hans 简体中文  会显示乱码
 
const unsigned long BAUD_RATE = 115200;                   // serial connection speed
const unsigned long HTTP_TIMEOUT = 5000;               // max respone time from server
const size_t MAX_CONTENT_SIZE = 1000;                   // max size of the HTTP response
 
// 我们要从此网页中提取的数据的类型
struct UserData {
  char city[16];//城市名称
  char weather[32];//天气介绍（多云...）
  char temp[16];//温度
  char udate[32];//更新时间
};
 
WiFiClient client;
char response[MAX_CONTENT_SIZE];
char endOfHeaders[] = "\r\n\r\n";
 
/**
* @Desc 初始化操作
*/
void setup() {
  WiFi.mode(WIFI_STA);     //设置esp8266 工作模式
  Serial.begin(BAUD_RATE);
  delay(10);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");//写几句提示，哈哈
  Serial.println(ssid);
  WiFi.begin(ssid, password);   //连接wifi
  while (WiFi.status() != WL_CONNECTED) {
    //这个函数是wifi连接状态，返回wifi链接状态
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  delay(500);
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());//WiFi.localIP()返回8266获得的ip地址
  client.setTimeout(HTTP_TIMEOUT);
}
 
/**
* @Desc  主函数
*/
void loop() {
  while (!client.connected()){
     if (!client.connect(host, 80)){
         Serial.println("connection....");
         delay(500);
     }
  }
  if (sendRequest(host, city, APIKEY) && skipResponseHeaders()) {
    clrEsp8266ResponseBuffer();
    readReponseContent(response, sizeof(response));
    UserData userData;
    if (parseUserData(response, &userData)) {
      printUserData(&userData);
    }
  }
  delay(5000);//每5s调用一次
}
 
/**
* @发送请求指令
*/
bool sendRequest(const char* host, const char* cityid, const char* apiKey) {
  // We now create a URI for the request
  //心知天气
  String GetUrl = "/v3/weather/now.json?key=";
  GetUrl += apiKey;
  GetUrl += "&location=";
  GetUrl += city;
  GetUrl += "&language=";
  GetUrl += language;
  // This will send the request to the server
  client.print(String("GET ") + GetUrl + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  Serial.println("create a request:");
  Serial.println(String("GET ") + GetUrl + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n");
  delay(1000);
  return true;
}
 
/**
* @Desc 跳过 HTTP 头，使我们在响应正文的开头
*/
bool skipResponseHeaders() {
  // HTTP headers end with an empty line
  bool ok = client.find(endOfHeaders);
  if (!ok) {
    Serial.println("No response or invalid response!");
  }
  return ok;
}
 
/**
* @Desc 从HTTP服务器响应中读取正文
*/
void readReponseContent(char* content, size_t maxSize) {
  size_t length = client.peekBytes(content, maxSize);
  delay(100);
  Serial.println("Get the data from Internet!");
  content[length] = 0;
  Serial.println(content);
  Serial.println("Read data Over!");
  client.flush();//这句代码需要加上  不然会发现每隔一次client.find会失败
}
 
/**
 * @Desc 解析数据 
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
bool parseUserData(char* content, struct UserData* userData) {
//    -- 根据我们需要解析的数据来计算JSON缓冲区最佳大小
//   如果你使用StaticJsonBuffer时才需要
//    const size_t BUFFER_SIZE = 1024;
//   在堆栈上分配一个临时内存池
//    StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;
//    -- 如果堆栈的内存池太大，使用 DynamicJsonBuffer jsonBuffer 代替
  DynamicJsonBuffer jsonBuffer;
  
  JsonObject& root = jsonBuffer.parseObject(content);
  
  if (!root.success()) {
    Serial.println("JSON parsing failed!");
    return false;
  }
   
  //复制我们感兴趣的字符串
  strcpy(userData->city, root["results"][0]["location"]["name"]);
  strcpy(userData->weather, root["results"][0]["now"]["text"]);
  strcpy(userData->temp, root["results"][0]["now"]["temperature"]);
  strcpy(userData->udate, root["results"][0]["last_update"]);
  //  -- 这不是强制复制，你可以使用指针，因为他们是指向“内容”缓冲区内，所以你需要确保
  //   当你读取字符串时它仍在内存中
  return true;
}
  
// 打印从JSON中提取的数据
void printUserData(const struct UserData* userData) {
  Serial.println("Print parsed data :");
  Serial.print("City : ");
  Serial.print(userData->city);
  Serial.print(", \t");
  Serial.print("Weather : ");
  Serial.print(userData->weather);
  Serial.print(",\t");
  Serial.print("Temp : ");
  Serial.print(userData->temp);
  Serial.print(" C");
  Serial.print(",\t");
  Serial.print("Last Updata : ");
  Serial.print(userData->udate);
  Serial.println("\r\n");
}
  
// 关闭与HTTP服务器连接
void stopConnect() {
  Serial.println("Disconnect");
  client.stop();
}
 
void clrEsp8266ResponseBuffer(void){
    memset(response, 0, MAX_CONTENT_SIZE);      //清空
}

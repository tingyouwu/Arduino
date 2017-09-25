/**
* 日期:2017/09/25
* 功能：wifi+weather+oled   8266端
* 作者：单片机菜鸟
* 16X16点阵显示 取模方式 阴码+逐行式+顺向
**/
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
//#define DEBUG 是否启用debug功能

#ifdef DEBUG
#define DebugPrintln(message)    Serial.println(message)
#define DebugPrint(message)    Serial.print(message)
#else
#define DebugPrintln(message)
#define DebugPrint(message)
#endif
#define LED 2

const unsigned long BAUD_RATE = 115200;                   // serial connection speed
const unsigned long HTTP_TIMEOUT = 5000;               // max respone time from server
const size_t MAX_CONTENT_SIZE = 1000;                   // max size of the HTTP response

const char* host = "api.seniverse.com";
const char* APIKEY = "wcmquevztdy1jpca";        //API KEY
const char* city = "guangzhou";
const char* language = "en";//zh-Hans 简体中文  会显示乱码

WiFiClient client;
char response[MAX_CONTENT_SIZE];
char endOfHeaders[] = "\r\n\r\n"; 
unsigned int flag = HIGH;//默认当前灭灯
long lastTime = 0;
// 请求服务间隔
long Delay = 20000;

// 我们要从此网页中提取的数据的类型
struct UserData {
  char city[16];//城市名称
  char weather[32];//天气介绍（多云...）
  char temp[16];//温度
  char udate[32];//更新时间
};

/**
* @Desc 初始化操作
*/
void setup()   {                
  Serial.begin(BAUD_RATE);
  pinMode(LED,OUTPUT);
  digitalWrite(LED, HIGH);
  if(!autoConfig()){
    smartConfig();
    while (WiFi.status() != WL_CONNECTED) {
    //这个函数是wifi连接状态，返回wifi链接状态
       delay(500);
       DebugPrint(".");
    }
  }
  DebugPrintln("IP address: ");
  DebugPrintln(WiFi.localIP());//WiFi.localIP()返回8266获得的ip地址
  lastTime = millis();
}

void loop() {
  while (!client.connected()){
     if (!client.connect(host, 80)){
         flag = !flag;
         digitalWrite(LED, flag);
         delay(500);
     }
  }

  if(millis()-lastTime>=Delay){
	 //每间隔20s左右调用一次
     lastTime = millis();
     if (sendRequest(host, city, APIKEY) && skipResponseHeaders()) {
       clrEsp8266ResponseBuffer();
       readReponseContent(response, sizeof(response));
       UserData userData;
       if (parseUserData(response, &userData)) {
          sendtoArduino(&userData);
       }
     }
  }

  if(Serial.available()>0){
      char ch = Serial.read();
      if(ch == '1'){
        smartConfig();
        delay(1000);
        digitalWrite(LED, LOW);
      }
   }
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
  DebugPrintln("create a request:");
  DebugPrintln(String("GET ") + GetUrl + " HTTP/1.1\r\n" +
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
    DebugPrintln("No response or invalid response!");
  }
  return ok;
}
 
/**
* @Desc 从HTTP服务器响应中读取正文
*/
void readReponseContent(char* content, size_t maxSize) {
  size_t length = client.peekBytes(content, maxSize);
  delay(100);
  DebugPrintln("Get the data from Internet!");
  content[length] = 0;
  DebugPrintln(content);
  DebugPrintln("Read data Over!");
  client.flush();//这句代码需要加上  不然会发现每隔一次client.find会失败
}
  
// 关闭与HTTP服务器连接
void stopConnect() {
  client.stop();
}
 
void clrEsp8266ResponseBuffer(void){
    memset(response, 0, MAX_CONTENT_SIZE);      //清空
}

/**
* 自动连接20s 超过之后自动进入SmartConfig模式
*/
bool autoConfig(){
  WiFi.mode(WIFI_STA);     //设置esp8266 工作模式
  WiFi.begin();
  delay(2000);//刚启动模块的话 延时稳定一下
  DebugPrintln("AutoConfiging ......");
  for(int i=0;i<20;i++){
    int wstatus = WiFi.status();
    if (wstatus == WL_CONNECTED){
      DebugPrintln("AutoConfig Success");
      DebugPrint("SSID:");
      DebugPrintln(WiFi.SSID().c_str());
      DebugPrint("PSW:");
      DebugPrintln(WiFi.psk().c_str());
      digitalWrite(LED, LOW);
      return true;
    }else{
      DebugPrint(".");
      delay(1000);
      flag = !flag;
      digitalWrite(LED, flag);
    } 
  }
  DebugPrintln("AutoConfig Faild!");
  return false;
}

/**
* 开启SmartConfig功能
*/
void smartConfig()
{
  WiFi.mode(WIFI_STA);
  delay(2000);
  DebugPrintln("Wait for Smartconfig");
  // 等待配网
  WiFi.beginSmartConfig();
  while (1){
    delay(500);
    flag = !flag;
    digitalWrite(LED, flag);
    if (WiFi.smartConfigDone()){
      //smartconfig配置完毕
      DebugPrintln("SmartConfig Success");
      DebugPrint("SSID:");
      DebugPrintln(WiFi.SSID().c_str());
      DebugPrint("PSW:");
      DebugPrintln(WiFi.psk().c_str());
      digitalWrite(LED, LOW);
      WiFi.setAutoConnect(true);  // 设置自动连接
      break;
    }
  }
}

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
void sendtoArduino(const struct UserData* userData) {
  StaticJsonBuffer<300> jsonBuffer;
  // StaticJsonBuffer 在栈区分配内存   它也可以被 DynamicJsonBuffer（内存在堆区分配） 代替
  // DynamicJsonBuffer  jsonBuffer;
  //创建根，也就是顶节点
  JsonObject& root = jsonBuffer.createObject();
  root["city"] = userData->city;
  root["weather"] = userData->weather;
  root["temp"] = userData->temp;
  root["time"] = userData->udate;
  root.printTo(Serial);//单行打印
}

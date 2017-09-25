/**
* 日期:2017/09/25
* 功能：wifi+weather+oled   8266端
* 作者：单片机菜鸟
* 16X16点阵显示 取模方式 阴码+逐行式+顺向
**/
#include <ESP8266WiFi.h>

#define DebugPrintln(message)    Serial.println(message)
#define DebugPrint(message)    Serial.print(message)
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
long Delay = 5000;

/**
* @Desc 初始化操作
*/
void setup()   {                
  Serial.begin(BAUD_RATE);
  pinMode(LED,OUTPUT);
  digitalWrite(LED, HIGH);
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
	 //每间隔5s左右调用一次
     lastTime = millis();
     if (sendRequest(host, city, APIKEY) && skipResponseHeaders()) {
       clrEsp8266ResponseBuffer();
       readReponseContent(response, sizeof(response));
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
  for(int i=0;i<20;i++){
    int wstatus = WiFi.status();
    if (wstatus == WL_CONNECTED){
      return true;
    }else{
      delay(1000);
      flag = !flag;
      digitalWrite(LED, flag);
    } 
  }
  return false;
}

/**
* 开启SmartConfig功能
*/
void smartConfig()
{
  WiFi.mode(WIFI_STA);
  delay(2000);
  // 等待配网
  WiFi.beginSmartConfig();
  while (1){
    delay(500);
    flag = !flag;
    digitalWrite(LED, flag);
    if (WiFi.smartConfigDone()){
      //smartconfig配置完毕
      WiFi.setAutoConnect(true);  // 设置自动连接
      break;
    }
  }
}


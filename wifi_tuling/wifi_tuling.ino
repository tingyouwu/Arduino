/**
* 日期:2017/09/14
* 功能：调用图灵API 
* 作者：单片机菜鸟
**/
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
 
const char* ssid     = "360wifilulu";         // XXXXXX -- 使用时请修改为当前你的 wifi ssid
const char* password = "6206908you11011010";         // XXXXXX -- 使用时请修改为当前你的 wifi 密码
const char* host = "www.tuling123.com";
const char* APIKEY = "3a0626a8ee8c489a847780b5f16c7001";        //API KEY
 
const unsigned long BAUD_RATE = 115200;                   // serial connection speed
const unsigned long HTTP_TIMEOUT = 5000;               // max respone time from server
const size_t MAX_CONTENT_SIZE = 500;                   // max size of the HTTP response
 
WiFiClient client;
char response[MAX_CONTENT_SIZE];
char endOfHeaders[] = "1f";
 
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
  delay(2000);
}
 
/**
* @Desc  主函数
*/
void loop() {

  while (!client.connected()){
     if (!client.connect(host, 80)){
         Serial.println("connection....");
         delay(1000);
     }
  }
  
  if (sendRequest(host,APIKEY) && skipResponseHeaders()) {
    clrEsp8266ResponseBuffer();
    readReponseContent(response, sizeof(response));
  }
  delay(10000);//每10s调用一次
}
 
/**
* @发送请求指令
*/
bool sendRequest(const char* host, const char* apiKey) {
  // We now create a URI for the request
  String PostUrl = "/openapi/api";
  // This will send the request to the server
  StaticJsonBuffer<200> jsonBuffer;
  //创建根，也就是顶节点
  JsonObject& root = jsonBuffer.createObject();
  root["key"] = APIKEY;
  root["info"] = "guangzhou";
  int len = root.measureLength();

  char buffer[256];
  root.printTo(buffer, sizeof(buffer));
  String data;
  for(int index = 0;index<len;index++){
     data += buffer[index]; 
  }
  
  client.print(String("POST ") + PostUrl + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Content-Type: application/json;charset=utf-8\r\n" +
               "Content-Length: " + len + "\r\n" +  
               "Connection: Keep Alive\r\n\r\n"+
               data+"\r\n");             
  Serial.println("create a request:");
  Serial.println(String("POST ") + PostUrl + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Content-Type: application/json;charset=utf-8\r\n" +
               "Content-Length: " + len + "\r\n" +  
               "Connection: keep-alive\r\n"+
               data+"\r\n");
  delay(3000);
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
  //  -- 这不是强制复制，你可以使用指针，因为他们是指向“内容”缓冲区内，所以你需要确保
  //   当你读取字符串时它仍在内存中
  return true;
}
  
// 关闭与HTTP服务器连接
void stopConnect() {
  Serial.println("Disconnect");
  client.stop();
}
 
void clrEsp8266ResponseBuffer(void){
    memset(response, 0, MAX_CONTENT_SIZE);      //清空
}

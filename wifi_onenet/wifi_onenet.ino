/**
* 日期:2017/09/15
* 功能：Post数据到OneNet(智能温湿度管理系统) 
* 作者：单片机菜鸟
**/
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

const char* ssid = "360wifilulu";// XXXXXX -- 使用时请修改为当前你的 wifi ssid
const char* password = "6206908you11011010";// XXXXXX -- 使用时请修改为当前你的 wifi 密码
const char* host = "api.heclouds.com";
const char* APIKEY = "0ove=yI5zy01lFtBYH9J1RdpURU=";//API KEY
const char* DeviceID = "13851462";//对应温度计的设备ID
const char* Device = "/devices/";
const char* addDataPoint = "/datapoints?type=3";
//新增一个数据点 Url //api.heclouds.com/devices/device_id/datapoints

const unsigned long BAUD_RATE = 115200;// serial connection speed
const unsigned long HTTP_TIMEOUT = 5000;// max respone time from server
const size_t MAX_CONTENT_SIZE = 500;// max size of the HTTP response

WiFiClient client;
char response[MAX_CONTENT_SIZE];
char endOfHeaders[] = "\r\n\r\n";

/**
* @Desc 初始化操作
*/
void setup() {
    WiFi.mode(WIFI_STA);//设置esp8266 工作模式
    Serial.begin(BAUD_RATE);
    delay(2000);//第一次进来先延时两秒;
    Serial.println();
    Serial.print("Connecting to ");//写几句提示，哈哈
    Serial.println(ssid);
    WiFi.begin(ssid, password);//连接wifi
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
* @Desc  主函数
*/
void loop() {
   while (!client.connected()){
         if (!client.connect(host, 80)){
             Serial.println("connection....");
             delay(1000);
         }
   }
  StaticJsonBuffer<100> jsonBuffer;
  //创建根，也就是顶节点
  JsonObject& root = jsonBuffer.createObject();
  root["temp"] = 25;
  int len = root.measureLength();

  char buffer[100];
  root.printTo(buffer, sizeof(buffer));
  String data;
  for(int index = 0;index<len;index++){
     data += buffer[index]; 
  }
  if(postToDeviceDataPoint(DeviceID,data)){
    clrEsp8266ResponseBuffer();
    readReponseContent(response, sizeof(response));
  }
  delay(10000);//每15s调用一次
}

/**
* @给设备新增一个数据点 
*/
bool postToDeviceDataPoint(String deviceId,String data){
    String url = Device;
    url += DeviceID;
    url += addDataPoint;

    String header = "api-key: ";
    header += APIKEY;
    return sendPostRequest(host,url,header,data);
}

/**
* @发送Get请求
* @param host 
* @param getUrl 服务地址 
* @param header 请求头
*/
bool sendGetRequest(const char* host,String getUrl,String header){
// This will send the request to the server
    client.print(String("GET ") + getUrl + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                  header + "\r\n" +
                 "Connection: Keep-Alive\r\n\r\n");
    Serial.println("Get request:");
    Serial.println(String("GET ") + getUrl + " HTTP/1.1\r\n" +
                   "Host: " + host + "\r\n" +
                   header + "\r\n" +
                   "Connection: Keep-Alive\r\n\r\n");
    delay(1000);
    return true;
}

/**
* @发送Post请求
*/
bool sendPostRequest(const char* host,String postUrl,String header,String data) {
// We now create a URI for the request
    client.print(String("POST ") + postUrl + 
                          " HTTP/1.1\r\n" +
                          "Host: " + host + "\r\n" +
                          header + "\r\n" +
                         "Content-Type: application/json\r\n" +
                         "Content-Length: " + data.length() + "\r\n" +
                         "Connection: Keep-Alive\r\n\r\n"+
                         data+"\r\n");
    Serial.println("Post request:");
    Serial.println(String("POST ") + postUrl +
                          " HTTP/1.1\r\n" +
                          "Host: " + host + "\r\n" +
                          header + "\r\n" +
                         "Content-Type: application/json\r\n" +
                         "Content-Length: " + data.length() + "\r\n" +
                         "Connection: Keep-Alive\r\n\r\n"+
                         data+"\r\n");
    return true;
}

/**
* @Desc 跳过 HTTP 头，使我们在响应正文的开头
*/
bool skipResponseHeaders() {
// HTTP headers end with an empty line
    bool ok = client.find(endOfHeaders);
    if(!ok){
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
   client.flush();//这句代码需要加上  不然会发现每隔一次client.find会失败
}

/**
 * @Desc 解析数据 
 * 数据格式如下：
 */
bool parseData(char* content) {
//    -- 根据我们需要解析的数据来计算JSON缓冲区最佳大小
//   如果你使用StaticJsonBuffer时才需要
//    const size_t BUFFER_SIZE = 1024;
//   在堆栈上分配一个临时内存池
//    StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;
//    -- 如果堆栈的内存池太大，使用 DynamicJsonBuffer jsonBuffer 代替
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(content);

    if(!root.success()){
      Serial.println("JSON parsing failed!");
      return false;
    }
//  -- 这不是强制复制，你可以使用指针，因为他们是指向“内容”缓冲区内，所以你需要确保
//   当你读取字符串时它仍在内存中
    return true;
}
// 关闭与HTTP服务器连接
void stopConnect() {
   Serial.println("Disconnect");
   client.stop();
}
void clrEsp8266ResponseBuffer(void){
  memset(response, 0, MAX_CONTENT_SIZE); //清空
}

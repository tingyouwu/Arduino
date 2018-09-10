/**
 * Demo：
 *    演示webserver auth校验功能
 *    (当wifi模块连接上ap之后，在pc浏览器中输入ip+uri来访问，不过需要带校验请求头)
 * @author 单片机菜鸟
 * @date 2019/09/10
 */
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

//以下三个定义为调试定义
#define DebugBegin(baud_rate)    Serial.begin(baud_rate)
#define DebugPrintln(message)    Serial.println(message)
#define DebugPrint(message)    Serial.print(message)

const char* AP_SSID     = "TP-LINK_5344";         // XXXXXX -- 使用时请修改为当前你的 wifi ssid
const char* AP_PSK = "6206908you11011010";         // XXXXXX -- 使用时请修改为当前你的 wifi 密码
const unsigned long BAUD_RATE = 115200;                   // serial connection speed
const char* www_username = "admin";
const char* www_password = "esp8266";

//声明一下函数
void initBasic(void);
void initWifi(void);
void initWebServer(void);

ESP8266WebServer server(80);//创建webserver

void setup() {
  initBasic();
  initWifi();
  initWebServer();
}

void loop() {
  server.handleClient();
}

/**
 * 初始化基础功能：波特率
 */
void initBasic(){
  DebugBegin(BAUD_RATE);
}

/**
 * 初始化wifi模块：工作模式 连接网络
 */
void initWifi(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(AP_SSID, AP_PSK);
  DebugPrintln("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    DebugPrint(".");
  }
  DebugPrintln("");
  DebugPrint("Connected to ");
  DebugPrintln(AP_SSID);
  DebugPrint("IP address: ");
  DebugPrintln(WiFi.localIP());
}

/**
 * 初始化webserver
 */
void initWebServer(){
  //以下配置uri对应的handler
  server.on("/", []() {
    //校验帐号和密码
    if (!server.authenticate(www_username, www_password)) {
      return server.requestAuthentication();
    }
    server.send(200, "text/plain", "Login OK");
  });
  server.begin();

  DebugPrint("Open http://");
  DebugPrint(WiFi.localIP());
  DebugPrintln("/ in your browser to see it working");
}

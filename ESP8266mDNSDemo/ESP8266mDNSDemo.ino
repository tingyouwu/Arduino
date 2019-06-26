/**
 * Demo：
 *    演示ESP8266 mDNS responder功能
 * @author 单片机菜鸟
 * @date 2019/09/15
 */
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>

//以下三个定义为调试定义
#define DebugBegin(baud_rate)    Serial.begin(baud_rate)
#define DebugPrintln(message)    Serial.println(message)
#define DebugPrint(message)    Serial.print(message)

const char* AP_SSID     = "TP-LINK_5344";         // XXXXXX -- 使用时请修改为当前你的 wifi ssid
const char* AP_PSK = "6206908you11011010";         // XXXXXX -- 使用时请修改为当前你的 wifi 密码
const unsigned long BAUD_RATE = 115200;                   // serial connection speed

//声明一下函数
void initBasic(void);
void initWifi(void);
void initWebServer(void);
void initmDNS(void);
void drawGraph(void); 

ESP8266WebServer server(80);

/**
 * 处理根目录uri请求
 * uri:http://server_ip/
 */
void handleRoot() {
  char temp[400];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  snprintf(temp, 400,

           "<html>\
  <head>\
    <meta http-equiv='refresh' content='5'/>\
    <title>ESP8266 Demo</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Hello from ESP8266!</h1>\
    <p>Uptime: %02d:%02d:%02d</p>\
    <img src=\"/test.svg\" />\
  </body>\
</html>",

           hr, min % 60, sec % 60
          );
  server.send(200, "text/html", temp);
}

/**
 * 处理无效uri
 * uri:http://server_ip/xxxx
 */
void handleNotFound() {
  //打印无效uri的信息 包括请求方式 请求参数
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void) {
  initBasic();
  initWifi();
  initWebServer();
  initmDNS();
}

void loop(void) {
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
  server.on("/", handleRoot);
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });
  server.on("/test.svg", drawGraph);
  server.onNotFound(handleNotFound);
  //启动webserver
  server.begin();
  DebugPrintln("HTTP server started");
}

/**
 * 初始化mDNS
 */
void initmDNS(){
  if (!MDNS.begin("esp8266")) {
    DebugPrintln("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  DebugPrintln("mDNS responder started");
}

/**
 * 画图
 */
void drawGraph() {
  String out = "";
  char temp[100];
  out += "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"400\" height=\"150\">\n";
  out += "<rect width=\"400\" height=\"150\" fill=\"rgb(250, 230, 210)\" stroke-width=\"1\" stroke=\"rgb(0, 0, 0)\" />\n";
  out += "<g stroke=\"black\">\n";
  int y = rand() % 130;
  for (int x = 10; x < 390; x += 10) {
    int y2 = rand() % 130;
    sprintf(temp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke-width=\"1\" />\n", x, 140 - y, x + 10, 140 - y2);
    out += temp;
    y = y2;
  }
  out += "</g>\n</svg>\n";

  server.send(200, "image/svg+xml", out);
}

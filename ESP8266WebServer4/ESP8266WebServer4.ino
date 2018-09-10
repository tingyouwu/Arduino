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

//声明一下函数
void initBasic(void);
void initWifi(void);
void initWebServer(void);

ESP8266WebServer server(80);

/**
 * 校验是否存在cookie头并且cookie头的值是正确的
 */
bool is_authentified() {
  DebugPrintln("Enter is_authentified");
  //是否存在cookie头
  if (server.hasHeader("Cookie")) {
    DebugPrint("Found cookie: ");
    //获取cookie头的信息
    String cookie = server.header("Cookie");
    DebugPrintln(cookie);
    if (cookie.indexOf("ESPSESSIONID=1") != -1) {
      DebugPrintln("Authentification Successful");
      return true;
    }
  }
  DebugPrintln("Authentification Failed");
  return false;
}

/**
 * 处理登陆uri
 */
void handleLogin() {
  String msg;
  //判断是否存在cookie头
  if (server.hasHeader("Cookie")) {
    DebugPrint("Found cookie: ");
    String cookie = server.header("Cookie");
    DebugPrint(cookie);
  }
  //判断是否存在DISCONNECT参数
  if (server.hasArg("DISCONNECT")) {
    DebugPrintln("Disconnection");
    server.sendHeader("Location", "/login");
    server.sendHeader("Cache-Control", "no-cache");
    server.sendHeader("Set-Cookie", "ESPSESSIONID=0");
    server.send(301);
    return;
  }
  //判断是否存在USERNAME和PASSWORD参数
  if (server.hasArg("USERNAME") && server.hasArg("PASSWORD")) {
    if (server.arg("USERNAME") == "admin" &&  server.arg("PASSWORD") == "admin") {
      server.sendHeader("Location", "/");
      server.sendHeader("Cache-Control", "no-cache");
      server.sendHeader("Set-Cookie", "ESPSESSIONID=1");
      server.send(301);
      DebugPrintln("Log in Successful");
      return;
    }
    msg = "Wrong username/password! try again.";
    DebugPrintln("Log in Failed");
  }
  //返回html 填写账号密码页面
  String content = "<html><body><form action='/login' method='POST'>To log in, please use : admin/admin<br>";
  content += "User:<input type='text' name='USERNAME' placeholder='user name'><br>";
  content += "Password:<input type='password' name='PASSWORD' placeholder='password'><br>";
  content += "<input type='submit' name='SUBMIT' value='Submit'></form>" + msg + "<br>";
  content += "You also can go <a href='/inline'>here</a></body></html>";
  server.send(200, "text/html", content);
}

/**
 * 根目录处理器
 */
//root page can be accessed only if authentification is ok
void handleRoot() {
  DebugPrintln("Enter handleRoot");
  String header;
  if (!is_authentified()) {
    //校验不通过
    server.sendHeader("Location", "/login");
    server.sendHeader("Cache-Control", "no-cache");
    server.send(301);
    return;
  }
  String content = "<html><body><H2>hello, you successfully connected to esp8266!</H2><br>";
  if (server.hasHeader("User-Agent")) {
    content += "the user agent used is : " + server.header("User-Agent") + "<br><br>";
  }
  content += "You can access this page until you <a href=\"/login?DISCONNECT=YES\">disconnect</a></body></html>";
  server.send(200, "text/html", content);
}

/**
 * 无效uri处理器
 */
void handleNotFound() {
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
  server.on("/login", handleLogin);
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works without need of authentification");
  });

  server.onNotFound(handleNotFound);
  //设置需要收集的请求头
  const char * headerkeys[] = {"User-Agent", "Cookie"} ;
  size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);
  //收集头信息
  server.collectHeaders(headerkeys, headerkeyssize);
  server.begin();
  DebugPrintln("HTTP server started");
}

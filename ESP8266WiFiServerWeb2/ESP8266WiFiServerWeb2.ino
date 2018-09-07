/*
 * Demo：
 *    演示简单web Server功能
 *    web server会根据请求来做不同的操作
 *    http://server_ip/gpio/0 打印 /gpio0
 *    http://server_ip/gpio/1 打印 /gpio1
 *    server_ip就是ESP8266的Ip地址
 * @author 单片机菜鸟
 * @date 2019/09/05
 */

#include <ESP8266WiFi.h>

//以下三个定义为调试定义
#define DebugBegin(baud_rate)    Serial.begin(baud_rate)
#define DebugPrintln(message)    Serial.println(message)
#define DebugPrint(message)    Serial.print(message)

#define ssid "TP-LINK_5344"
#define password "6206908you11011010"

// 创建tcp server
WiFiServer server(80);

void setup() {
  DebugBegin(115200);
  delay(10);

  // Connect to WiFi network
  DebugPrintln("");
  DebugPrintln(String("Connecting to ") + ssid);
  //我只想做个安静的美男子 STA
  WiFi.mode(WIFI_STA);
  //我想连接路由wifi
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    DebugPrint(".");
  }
  DebugPrintln("");
  DebugPrintln("WiFi connected");

  // 启动server
  server.begin();
  DebugPrintln("Server started");

  // 打印IP地址
  DebugPrintln(WiFi.localIP());
}

void loop() {
  // 等待有效的tcp连接
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  DebugPrintln("new client");
  //等待client数据过来
  while (!client.available()) {
    delay(1);
  }

  // 读取请求的第一行 会包括一个url，这里只处理url
  String req = client.readStringUntil('\r');
  DebugPrintln(req);
  //清掉缓冲区数据
  client.flush();

  // 开始匹配
  int val;
  if (req.indexOf("/gpio/0") != -1) {
    DebugPrintln("/gpio0");
    val = 0;
  } else if (req.indexOf("/gpio/1") != -1) {
    DebugPrintln("/gpio1");
    val = 1;
  } else {
    DebugPrintln("invalid request");
    //关闭这个client请求
    client.stop();
    return;
  }
  //清掉缓冲区数据
  client.flush();

  // 准备响应数据
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nGPIO is now ";
  s += (val) ? "high" : "low";
  s += "</html>\n";

  // 发送响应数据给client
  client.print(s);
  delay(1);
  DebugPrintln("Client disonnected");

  // The client will actually be disconnected
  // when the function returns and 'client' object is detroyed
}


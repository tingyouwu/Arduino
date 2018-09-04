/**
 * Demo：
 *    STA模式下，演示WiFiClient与TCP server之间的通信功能
 *    本实验需要跟TCP调试助手一起使用。
 * @author 单片机菜鸟
 * @date 2019/09/04
 */
#include <ESP8266WiFi.h>

//以下三个定义为调试定义
#define DebugBegin(baud_rate)    Serial.begin(baud_rate)
#define DebugPrintln(message)    Serial.println(message)
#define DebugPrint(message)    Serial.print(message)

#define AP_SSID "TP-LINK_5344" //这里改成你的wifi名字
#define AP_PSW  "6206908you11011010"//这里改成你的wifi密码

const uint16_t port = 80;
const char * host = "192.168.1.1"; // ip or dns
WiFiClient client;//创建一个tcp client连接

void setup() {
  //设置串口波特率，以便打印信息
  DebugBegin(115200);
  //延时5s 为了演示效果
  delay(5000);
  // 我不想别人连接我，只想做个站点
  WiFi.mode(WIFI_STA);
  WiFi.begin(AP_SSID,AP_PSW);

  DebugPrint("Wait for WiFi... ");
  //等待wifi连接成功
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  DebugPrintln("");
  DebugPrintln("WiFi connected");
  DebugPrint("IP address: ");
  DebugPrintln(WiFi.localIP());

  delay(500);
}


void loop() {
  
  DebugPrint("connecting to ");
  DebugPrintln(host);

  if (!client.connect(host, port)) {
    DebugPrintln("connection failed");
    DebugPrintln("wait 5 sec...");
    delay(5000);
    return;
  }

  // 发送数据到Tcp server
  client.println("Send this data to server");

  //读取从server返回到响应数据
  String line = client.readStringUntil('\r');
  DebugPrintln(line);

  DebugPrintln("closing connection");
  client.stop();

  DebugPrintln("wait 5 sec...");
  delay(5000);
}


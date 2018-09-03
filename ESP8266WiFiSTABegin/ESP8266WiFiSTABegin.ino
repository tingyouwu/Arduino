   /**
 * Demo1：
 *    statin模式下，创建一个连接到可接入点（wifi热点），并且打印IP地址
 * @author 单片机菜鸟
 * @date 2019/09/02
 */
#include <ESP8266WiFi.h>

#define AP_SSID "TP-LINK_5344" //这里改成你的wifi名字
#define AP_PSW  "6206908you11011010"//这里改成你的wifi密码
//以下三个定义为调试定义
#define DebugBegin(baud_rate)    Serial.begin(baud_rate)
#define DebugPrintln(message)    Serial.println(message)
#define DebugPrint(message)    Serial.print(message)

void setup(){
  //设置串口波特率，以便打印信息
  DebugBegin(115200);
  //延时2s 为了演示效果
  delay(2000);
  DebugPrintln("Setup start");
  //启动STA模式，并连接到wifi网络
  WiFi.begin(AP_SSID, AP_PSW);

  DebugPrint(String("Connecting to ")+AP_SSID);
  //判断网络状态是否连接上，没连接上就延时500ms，并且打出一个点，模拟连接过程
  //笔者扩展：加入网络一直都连不上 是否可以做个判断，由你们自己实现
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    DebugPrint(".");
  }
  DebugPrintln("");

  DebugPrint("Connected, IP address: ");
  //输出station IP地址，这里的IP地址由DHCP分配
  DebugPrintln(WiFi.localIP());
  DebugPrintln("Setup End");
}

void loop() {
}

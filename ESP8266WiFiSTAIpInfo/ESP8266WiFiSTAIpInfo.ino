/**
 * Demo3：
 *    statin模式下，创建一个连接到可接入点（wifi热点），并且打印station信息
 * @author 单片机菜鸟
 * @date 2019/09/02
 */
#include <ESP8266WiFi.h>

#define AP_SSID "TP-LINK_5344" //这里改成你的wifi名字
#define AP_PSW  "6206908you11011010" //这里改成你的wifi密码
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
  //设置自动连接
  WiFi.setAutoConnect(true);
  //设置自动重连
  WiFi.setAutoReconnect(true);
  DebugPrint(String("Connecting to ")+AP_SSID);
  //判断网络状态是否连接上，没连接上就延时500ms，并且打出一个点，模拟连接过程
  //笔者扩展：加入网络一直都连不上 是否可以做个判断，由你们自己实现
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    DebugPrint(".");
  }
  DebugPrintln("");

  DebugPrintln("Print Network Info:");
  if (WiFi.status() == WL_CONNECTED){
     //输出mac地址
     DebugPrintln(String("Connected, mac address: ")+WiFi.macAddress().c_str());
     
     //输出station IP地址，这里的IP地址由DHCP分配
     DebugPrintln(String("Connected, IP address: ")+WiFi.localIP().toString());
     
     //输出子网掩码地址
     DebugPrintln(String("Subnet mask: ")+WiFi.subnetMask().toString());
     
     //输出网关 IP地址
     DebugPrintln(String("Gataway IP: ")+WiFi.gatewayIP().toString());
     
     //输出hostname
     DebugPrintln(String("Default hostname: ")+WiFi.hostname());
     //设置新的hostname
     WiFi.hostname("Station_host_博哥");
     DebugPrintln(String("New hostname: ")+WiFi.hostname());
     
     //输出SSID
     DebugPrintln(String("SSID: ")+WiFi.SSID());

     //输出psk
     DebugPrintln(String("psk: ")+WiFi.psk());
     
     //输出BSSID
     DebugPrintln(String("BSSID: ")+WiFi.BSSIDstr());
     
     //输出RSSI
     DebugPrintln(String("RSSI: ") + WiFi.RSSI() + " dBm");
  }
  
  DebugPrintln("Setup End");
}

void loop() {
}

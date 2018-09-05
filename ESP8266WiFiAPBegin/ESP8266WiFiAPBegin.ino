/**
 * Demo：
 *    AP模式下，演示AP 函数方法的使用
 * @author 单片机菜鸟
 * @date 2019/09/02
 */
#include <ESP8266WiFi.h>

#define AP_SSID "AP_Test_boge" //这里改成你的AP名字
#define AP_PSW  "123456789" //这里改成你的AP密码 至少8位以上
//以下三个定义为调试定义
#define DebugBegin(baud_rate)    Serial.begin(baud_rate)
#define DebugPrintln(message)    Serial.println(message)
#define DebugPrint(message)    Serial.print(message)

IPAddress local_IP(192,168,4,22);
IPAddress gateway(192,168,4,9);
IPAddress subnet(255,255,255,0);

void setup(){
  //设置串口波特率，以便打印信息
  DebugBegin(115200);
  //延时2s 为了演示效果
  delay(5000);
  DebugPrint("Setting soft-AP configuration ... ");
  //配置AP信息
  WiFi.mode(WIFI_AP);
  DebugPrintln(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");
  //启动AP模式，并设置账号和密码
  DebugPrint("Setting soft-AP ... ");
  bool result = WiFi.softAP(AP_SSID,AP_PSW);
  if(result){
    DebugPrintln("Ready");
    //输出 soft-ap ip地址
    DebugPrintln(String("Soft-AP IP address = ") + WiFi.softAPIP().toString());
    //输出 soft-ap mac地址
    DebugPrintln(String("MAC address = ") + WiFi.softAPmacAddress().c_str());
  }else{
    DebugPrintln("Failed!");
  }
  DebugPrintln("Setup End");
}

void loop() {
  //不断打印当前的station个数
  DebugPrintln(String("Stations connected =") + WiFi.softAPgetStationNum());
  delay(5000);
}

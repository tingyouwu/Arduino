/**
 * Demo：
 *    STA模式下，演示同步扫描Scan wifi功能
 * @author 单片机菜鸟
 * @date 2019/09/03
 */
#include <ESP8266WiFi.h>

//以下三个定义为调试定义
#define DebugBegin(baud_rate)    Serial.begin(baud_rate)
#define DebugPrintln(message)    Serial.println(message)
#define DebugPrint(message)    Serial.print(message)

void setup() {
  //设置串口波特率，以便打印信息
  DebugBegin(115200);
  //延时5s 为了演示效果
  delay(5000);
  // 我不想别人连接我，只想做个站点
  WiFi.mode(WIFI_STA);
  //断开连接
  WiFi.disconnect();
  delay(100);
  DebugPrintln("Setup done");
}

void loop() {
  DebugPrintln("scan start");
  // 同步扫描，等待返回结果
  int n = WiFi.scanNetworks();
  DebugPrintln("scan done");
  if (n == 0){
    DebugPrintln("no networks found");
  }else{
    DebugPrint(n);
    DebugPrintln(" networks found");
    for (int i = 0; i < n; ++i){
      DebugPrint(i + 1);
      DebugPrint(": ");
      //打印wifi账号
      DebugPrint(WiFi.SSID(i));
      DebugPrint(",");
      DebugPrint(String("Ch:")+WiFi.channel(i));
      DebugPrint(",");
      DebugPrint(WiFi.isHidden(i)?"hide":"show");
      DebugPrint(" (");
      //打印wifi信号强度
      DebugPrint(WiFi.RSSI(i));
      DebugPrint("dBm");
      DebugPrint(")");
      //打印wifi加密方式
      DebugPrintln((WiFi.encryptionType(i) == ENC_TYPE_NONE)?"open":"*");
      delay(10);
    }
  }
  DebugPrintln("");
  // 延时5s之后再次扫描
  delay(5000);
}

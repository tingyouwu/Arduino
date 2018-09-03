/**
 * Demo：
 *    STA模式下，演示异步扫描Scan wifi功能
 * @author 单片机菜鸟
 * @date 2019/09/03
 */
#include <ESP8266WiFi.h>

//以下三个定义为调试定义
#define DebugBegin(baud_rate)    Serial.begin(baud_rate)
#define DebugPrintln(message)    Serial.println(message)
#define DebugPrint(message)    Serial.print(message)
//定义一个扫描时间间隔
#define SCAN_PERIOD 5000
long lastScanMillis;

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

 long currentMillis = millis();
 //触发扫描
 if (currentMillis - lastScanMillis > SCAN_PERIOD){
    WiFi.scanNetworks(true);
    Serial.print("\nScan start ... ");
    lastScanMillis = currentMillis;
  }

  // 判断是否有扫描结果
  int n = WiFi.scanComplete();
  if(n >= 0){
    Serial.printf("%d network(s) found\n", n);
    for (int i = 0; i < n; i++){
      Serial.printf("%d: %s, Ch:%d (%ddBm) %s\n", i+1, WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i), WiFi.encryptionType(i) == ENC_TYPE_NONE ? "open" : "");
    }
    //打印完一次扫描结果之后  删除内存保存结果
    WiFi.scanDelete();
  }
}

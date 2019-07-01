#include <WiFi.h>

/**
 * Demo：
 *    测试ESP32 demo
 *    打印ESP32模块信息
 *    1.打印Arduino Core For ESP32 版本
 *    2.打印Flash的唯一性芯片id（读者可以思考一下是否可以用来做点什么唯一性参考）
 *    4.打印IDE配置的使用Flash大小
 *    5.打印IDE配置的Flash连接通信的频率
 *    6.打印Flash连接模式：QIO QOUT DIO DOUT，可以理解为Flash传输速率
 * @author 单片机菜鸟
 * @date 2019/07/01
 */

#define LED_BUILTIN  22

int state = HIGH;
 
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  FlashMode_t ideMode = ESP.getFlashChipMode();
  String coreVersion = ESP.getSdkVersion();
  Serial.print(F("Arduino Core For ESP32 Version: "));
  Serial.println(coreVersion);
  Serial.printf("Flash real id（唯一标识符）:   %08X\n", ESP.getChipRevision());
  Serial.printf("IDE配置Flash大小: %u KBytes\n", ESP.getFlashChipSize()/1024);
  Serial.printf("IDE配置Flash频率 : %u MHz\n", ESP.getFlashChipSpeed()/1000000);
  Serial.printf("Flash ide mode:  %s\n\n", (ideMode == FM_QIO ? "QIO" : ideMode == FM_QOUT ? "QOUT" : ideMode == FM_DIO ? "DIO" : ideMode == FM_DOUT ? "DOUT" : "UNKNOWN"));
  state = !state;
  digitalWrite(LED_BUILTIN, state);
  delay(1000);
}

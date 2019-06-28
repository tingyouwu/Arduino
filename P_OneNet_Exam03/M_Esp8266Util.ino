/**
 * 功能：ESP8266常用操作方法库
 * 作者：单片机菜鸟
 * 时间：2019-06-26
 */

#include "H_project.h"

Ticker delayTimer;

/**
 * 延时N秒
 */
void delayNs(uint8_t m){
  for(uint8_t index = 0;index<m;index ++){
    delay(1000);
    ESP.wdtFeed();
  }
}

/*
 * 延时重启
*/
void delayRestart(float t) {
  Serial.print("Restart after ");
  Serial.print(t);
  Serial.println("s");
  delayTimer.attach(t, []() {
    Serial.println("\r\nRestart now!");
    ESP.restart();
  });
}
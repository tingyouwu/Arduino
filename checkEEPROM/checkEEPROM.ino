/**
* @Desc  测试EEPROM
* @author 单片机菜鸟
* @Date  2016/12/25
* 
* 注意：Arduino Mega 2560的EEPROM空间是4KB
* 也就是4*1024 B = 4096B
* 也就是地址是0~4095
*/

#include<EEPROM.h>

//表示EEPROM的当前地址，也就是即将要写入的地址
int addr = 0;

void setup() {

  // 初始化串口
  Serial.begin(9600);
  //等待串口准备好
  while(!Serial);
  
  int value = 2;
  //在地址0~3依次写入2~5
  for(int i = addr;i<4;i++){
    EEPROM.write(i,value++);
    //延时100ms 等待写入
    delay(100);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("read data start");
  int value;
  for(int i = addr;i<4;i++){
    value = EEPROM.read(i);
    Serial.print("addr:");
    Serial.print(i);
    Serial.print("\n");
    Serial.print("value:");
    Serial.print(value);
    Serial.print("\n");
  }
  Serial.println("read data end");
  
  while(1);
}

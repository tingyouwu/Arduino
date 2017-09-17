/**
* 日期:2017/09/14
* 功能：用来打印 8266收到的信息
* 作者：单片机菜鸟
**/
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
  
const unsigned long BAUD_RATE = 115200;                   // serial connection speed
const size_t MAX_CONTENT_SIZE = 1000; 

//#define UNO      //uncomment this line when you use it with UNO board
#define MEGA    //uncomment this line when you use it with MEGA board

#ifdef UNO
 SoftwareSerial mySerial(10,11);
#endif
  
#ifdef UNO
#define WifiSerial  Serial
#define MyDebugSerial mySerial
#endif
   
#ifdef MEGA
#define WifiSerial Serial1
#define MyDebugSerial Serial
#endif  

//该条语句用于使能DEBUG输出信息，屏蔽掉就不会输出debug调试信息
#define DEBUG

#ifdef DEBUG
#define DBGLN(message)    MyDebugSerial.println(message)
#else
#define DBGLN(message)
#endif
  
/**
* @Desc 初始化操作
*/
void setup() {
  WifiSerial.begin(BAUD_RATE);
  #ifdef DEBUG
    #ifdef UNO
      MyDebugSerial.begin(9600);//软串口9600稳定
    #else
      MyDebugSerial.begin(BAUD_RATE);
    #endif
  #endif
  DBGLN("Arduino Init End");
}
  
/**
* @Desc  主函数
*/
void loop() {
  if(WifiSerial.available()>0){
    ReceiveMessage();
  }
}

/**
* 读取串口缓冲区里面的数据
*/
void ReceiveMessage(){
  //不用 readBytes 因为比较耗时
  while(WifiSerial.available()>0){
     MyDebugSerial.write(WifiSerial.read());
  }
}

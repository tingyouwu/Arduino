/**
* @Desc  测试红外通信
* @author 单片机菜鸟
* @Date  2016/12/29
* 
*/
#include <IRremote.h>

#define MAX 22

int RECV_PIN = 7;      //定义红外接收模块输出口接arduino数字引脚11
  
IRrecv irrecv(RECV_PIN);
decode_results results;
//Car mp3编码
unsigned long rremote_code[MAX] = {
    0xFFA25D,0xFF629D,0xFFE21D,//CH- CH CH+
    0xFF22DD,0xFF02FD,0xFFC23D,//PREV NEXT PLAY
    0xFFE01F,0xFFA857,0xFF906F,//VOL- VOL+ EQ
    0xFF6897,0xFF9867,0xFFB04F,// 0 100+ 200+
    0xFF30CF,0xFF18E7,0xFF7A85,// 1  2  3
    0xFF10EF,0xFF38C7,0xFF5AA5,// 4  5  6
    0xFF42BD,0xFF4AB5,0xFF52AD, // 7  8  9
    0xFFFFFFFF//长按
};

//Car mp3对应的字符串
String rremote_string[MAX] = {
   "CH-","CH","CH+",
   "PREV","NEXT","PLAY/PAUSE",
   "VOL-","VOL+","EQ",
   "0","100+","200+",
   "1","2","3",
   "4","5","6",
   "7","8","9",
   "longPress"
};
  
void setup()
{
  Serial.begin(9600);
  irrecv.enableIRIn(); //初始化红外遥控
}
  
void loop() {
  if (irrecv.decode(&results)) {
    //打印字符串
    String codeString = getRremoteString(results.value);
    if(codeString.length()!=0)
      Serial.println(codeString);
    irrecv.resume(); // 接收下一个值
  }
}

/**
* 解析红外编码并返回对应的字符串
*/
String getRremoteString(unsigned long code){
  String rremotestring = "";
  int i = 0;
  for(i = 0;i<MAX-1;i++){
    if(code == rremote_code[i]){
      break;  
    }
  }
  //过滤掉长按
  if(i==MAX-1) return "";
  else return rremote_string[i];
}


/**
* 日期:2016/11/24
* 功能：蓝牙台灯
* 作者：单片机菜鸟
**/

#include <SoftwareSerial.h>

//#define UNO      //uncomment this line when you use it with UNO board
#define MEGA    //uncomment this line when you use it with MEGA board

#ifdef UNO
 SoftwareSerial mySerial(10,11);
#endif

#ifdef UNO
#define BTSerial  Serial
#define MyDebugSerial mySerial

#endif  
#ifdef MEGA
#define BTSerial Serial1
#define MyDebugSerial Serial
#endif  
 
//该条语句用于使能DEBUG输出信息，屏蔽掉就不会输出debug调试信息
//#define DEBUG
//该条语句用于使能是共阴RGB  屏蔽掉就是共阳RGB
//#define COMMON_GND
 
#ifdef DEBUG
#define DBGLN(message)    MyDebugSerial.println(message)
#else
#define DBGLN(message)
#endif

#ifdef UNO 
#define PIN_RED 3
#define PIN_GREEN 5
#define PIN_BLUE 6
#else
#define PIN_RED 2
#define PIN_GREEN 3
#define PIN_BLUE 4
#endif  

#ifdef UNO
  #define LED 3
#else
  #define LED 13
#endif

int delay_time = 1500; //延时时间，可以调节然后就可以看到亮灭频率
unsigned long currenttime;  
 
int red = 0;//红色
int green = 0;//绿色
int blue = 0;//蓝色

int light = 5;//亮度
int led_style = 0;//led风格 0亮灭 1闪烁 2亮度
int default_status = LOW;//灯默认状态
 
void setup() {
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE, OUTPUT);
  pinMode(LED, OUTPUT);
  digitalWrite(LED,LOW);//点亮LED
  BTSerial.begin(9600);
  MyDebugSerial.begin(9600);
  currenttime = millis();
}
 
void loop() {

  if (BTSerial.available()>0){
      char c0 = BTSerial.read();
      DBGLN("start:"+c0);
      if(c0 == '+'){
        //RGB
        int leng = ReceiveBTMessage();
        if(leng>0){
            //收到了新RGB数值
            colorRGB(red,green,blue);
         }
      }else if(c0 == '1'){
        //led 亮
        led_style = 0;
        digitalWrite(LED,HIGH);//点亮LED
      }else if(c0 == '0'){
        //led灭
        led_style = 0;
        digitalWrite(LED,LOW);//灭掉LED
      }else if(c0 == 'F'){
        //闪烁+
        led_style = 1;
        currenttime = millis();
        if(delay_time != 500){
          delay_time-=500;  
        }else{
          delay_time=500;
        }
      }else if(c0 == 'G'){
        //闪烁-
        led_style = 1;
        currenttime = millis();
        if(delay_time != 2500){
          delay_time+=500;  
        }else{
          delay_time=2500;
        }
      }else if(c0 == 'L'){
        //亮度+
        led_style = 2;
        if(light!=10){
          light++;  
        }else{
          light = 10;
        }
      }else if(c0 == 'M'){
        //亮度-
        led_style = 2;
        if(light!=0){
          light--;  
        }else{
          light = 0;
        }
      }
  }

  if(led_style == 1){
      //闪烁频率
      if(millis()-currenttime>=delay_time){
        currenttime = millis();
        default_status = !default_status;
        digitalWrite(LED,default_status);
      }
  }else if(led_style == 2){
      //亮度
      analogWrite(LED,light*25);
  }
  
}
 
/**
* 初始化RGB灯(后面可以考虑加入一个记录之前状态的功能，下次打开就取出来显示)
*/
void init_RGB(){
  #ifdef COMMON_GND
    colorRGB(255,255,255);
  #else
    colorRGB(0,0,0);
  #endif
}
 
/**
* 读取串口缓冲区里面的数据
* 
*/
int ReceiveBTMessage(){
  /**
     * @Desc 把RGB组合成协议内的数据格式
     *  开始符+长度+rgb+结束符
     * "+C,n:rgbString;"
     * 例子:"+C,10:100,255,60;"
     **/
  String data = "";
  unsigned long start;
  start = millis();
  char c;
  while (millis()-start<100) {
        if (BTSerial.available()>0){
          c = BTSerial.read();
          data += c;
        }
        if (c==';')
          break;
  }
 
  int sLen = strlen(data.c_str());
  int i,r,g,b;
  for (i = 0; i <= sLen; i++){
        if (data[i] == ':'){
          break;
        }
  }
  DBGLN(data);
  int iSize;
  //"+C,10:100,255,60;"    
  String _size = data.substring(2, i);
  iSize = _size.toInt();
 
  for(r=i+1;r<=sLen;r++){
         if(data[r]==','){
           break;
          }  
  }
 
  for(g=r+1;g<=sLen;g++){
         if(data[g]==','){
           break;
          }  
  }
 
  for(b=g+1;b<=sLen;b++){
         if(data[b]==';'){
           break;
          }  
  }
 
  String sred = data.substring(i+1,r);
  String sgreen = data.substring(r+1,g);
  String sblue = data.substring(g+1,b);
 
  red = sred.toInt();
  green = sgreen.toInt();
  blue = sblue.toInt();
 
  DBGLN("red:"+sred);
  DBGLN("green:"+sgreen);
  DBGLN("blue:"+sblue);
  return iSize;
}
 
void colorRGB(int red, int green, int blue){
  #ifdef COMMON_GND
     analogWrite(PIN_RED,constrain(red,0,255));
     analogWrite(PIN_GREEN,constrain(green,0,255));
     analogWrite(PIN_BLUE,constrain(blue,0,255));
  #else
     analogWrite(PIN_RED,constrain(255-red,0,255));
     analogWrite(PIN_GREEN,constrain(255-green,0,255));
     analogWrite(PIN_BLUE,constrain(255-blue,0,255));
  #endif
}

/**
* 日期:2017/09/18
* 功能：wifi lamp arduino端
* 作者：单片机菜鸟
**/
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
  
const unsigned long BAUD_RATE = 115200;                   // serial connection speed
const size_t MAX_CONTENT_SIZE = 50; 
const size_t t_bright=1,t_color=2,t_frequency=3,t_switch=4;

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
//该条语句用于使能是共阴RGB  屏蔽掉就是共阳RGB
//#define COMMON_GND

#ifdef DEBUG
#define DBGLN(message)    MyDebugSerial.println(message)
#else
#define DBGLN(message)
#endif

#ifdef UNO 
#define PIN_RED 3 //red 引脚
#define PIN_GREEN 5 //green 引脚
#define PIN_BLUE 6 //blue 引脚
#define PIN_ENABLE 9  //使能引脚 pwm控制亮度
#define PIN_KEY 7// 按键
#else
#define PIN_RED 2
#define PIN_GREEN 3
#define PIN_BLUE 4
#define PIN_ENABLE 5
#define PIN_KEY 6  
#endif 

int red = 0,green = 0,blue = 0;
int type = 4;//当前模式 1亮度 2颜色 3呼吸 4开关
int frequency = 1;//频率
int switch_status = 1;//关闭 or 开启
int bright = 1;//亮度

char response[MAX_CONTENT_SIZE];
int fadeValue = 0;//当前亮度
bool isAdd = true;//是否是从暗到亮

// 定义记录按键当前状态的变量
int state_btn;
// 定义记录按键最近一次状态变化的变量，并初始化状态为LOW。
int lastButtonState = LOW;
// 定义记录最近一次抖动的时间变量，并初始化时间为0毫秒。
long lastDebounceTime = 0;
// 定义延迟抖动的时间变量
long debouncdDelay = 60;
  
/**
* @Desc 初始化操作
*/
void setup() {
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE, OUTPUT);
  pinMode(PIN_ENABLE, OUTPUT);
  pinMode(PIN_KEY,INPUT);
  
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
    clrEsp8266ResponseBuffer();
    int data_size = ReceiveMessage(response, sizeof(response));
    if(data_size>0){
      //开始解析数据
      parseData(response);
    }
  }

  if(type == t_frequency){
    //呼吸灯效果
    breatheRGB(frequency);
  }
  checkButton();
}

/**
* 读取串口缓冲区里面的数据
*/
int ReceiveMessage(char* content, size_t maxSize){
  //不用 readBytes 因为比较耗时
  size_t length = WifiSerial.readBytesUntil('}',content, maxSize);
  content[length] = '}';
  content[++length] = 0;
  DBGLN(content);
  return length;
}

/**
     * @Desc 解析json
     * 有三种
     * 1.亮度控制页面(0 暗 1正常 2亮)
     * {
     *     "t": 1,
     *     "bb": 2
     * }
     * 2.颜色控制页面
     * {
     *     "t": 2,
     *     "cr": 154,
     *     "cg": 147,
     *     "cb": 255
     * }
     * 3.呼吸灯控制页面(0 慢呼吸 1正常 2快)
     * {
     *    "t": 3,
     *    "gf": 1
     * }
     * 4.开关控制(0关闭 1开启)
     * {
     *    "t": 4,
     *    "ss": 1
     * } 
     **/
bool parseData(char* content) {
//    -- 根据我们需要解析的数据来计算JSON缓冲区最佳大小
//   如果你使用StaticJsonBuffer时才需要
//    const size_t BUFFER_SIZE = 1024;
//   在堆栈上分配一个临时内存池
//    StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;
//    -- 如果堆栈的内存池太大，使用 DynamicJsonBuffer jsonBuffer 代替
  DynamicJsonBuffer jsonBuffer;
   
  JsonObject& root = jsonBuffer.parseObject(content);
   
  if (!root.success()) {
    Serial.println("JSON parsing failed!");
    return false;
  }
    
  type = root["t"];
  switch(type){
    case t_bright:
         bright = root["bb"];
         brightRGB(bright);
         break;
    case t_color:
         red = root["cr"];
         green = root["cg"];
         blue = root["cb"];
         colorRGB(red,green,blue);
         break;
    case t_frequency:
         frequency = root["gf"];
         break;
    case t_switch:
         switch_status = root["ss"];
         bool enable = switch_status == 1;
         switchRGB(enable);
         break;     
  }
  return true;
}

/**
* 控制灯亮度
*/
void brightRGB(int bright){
  int level = bright%3;
  int bright_level;
  switch(level){
    case 0://暗  50
      bright_level = 50;
      break;
    case 1://正常 100
      bright_level = 100;
      break;
    case 2://亮  200
      bright_level = 200;
      break;
  }
    #ifdef COMMON_GND
     //共地
     analogWrite(PIN_ENABLE,bright_level);
    #else
     analogWrite(PIN_ENABLE,255-bright_level);
    #endif
}

/**
* 控制RGB颜色
*/
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

/**
* 控制亮灭
*/
void switchRGB(bool enable){
  if(enable){
    //打开
    #ifdef COMMON_GND
     //共地
     analogWrite(PIN_ENABLE,255);
    #else
     analogWrite(PIN_ENABLE,0);
    #endif
  }else{
    //关闭
    #ifdef COMMON_GND
     //共地
     analogWrite(PIN_ENABLE,0);
    #else
     analogWrite(PIN_ENABLE,255);
    #endif
  }
}

/**
* 呼吸灯
*/
void breatheRGB(int frequency){
  int level = frequency%3;
  int f_level;
  switch(level){
    case 0://慢  50
      f_level = 3;
      break;
    case 1://正常 100
      f_level = 10;
      break;
    case 2://快  200
      f_level = 20;
      break;
  }
  if(isAdd){
    //递增方向
     fadeValue +=f_level;
     if(fadeValue>=255){
       fadeValue = 255;
       isAdd =false;
     }
  }else{
    //递减方向
    fadeValue -=f_level;
     if(fadeValue<=0){
       fadeValue = 0;
       isAdd =true;
     }
  }
  analogWrite(PIN_ENABLE,fadeValue);
  delay(20);
}

/**
* 检查按键功能
*/
void checkButton(){
  int buttonState = digitalRead(PIN_KEY);//读取当前按键状态
  if(buttonState != lastButtonState){
     //如果按键发生了变化  则重新设置最近一次抖动的时间
     //方法millis()可以获取当前时间，单位统一为毫秒。
     lastDebounceTime = millis();  
  }
   
  // 判断按键按下状态时间间隔是否大于延迟抖动的时间长度。
  if(millis()-lastDebounceTime>debouncdDelay){
    // 判断当前的按键状态是否和之前有所变化
    if(buttonState != state_btn){
       // 如果发生了变化，
       // 则更新按键状态变量。
       state_btn = buttonState;
       if(state_btn == HIGH){
        //再次确认是否真的按下了按键
         DBGLN("smartconfig");
         WifiSerial.write('1');
       }
    }
  }
  // 更新按键最近一次状态变化的变量
  lastButtonState = buttonState;
}

void clrEsp8266ResponseBuffer(void){
    memset(response, 0, MAX_CONTENT_SIZE);      //清空
}

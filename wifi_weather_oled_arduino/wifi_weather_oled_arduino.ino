/**
* 日期:2017/09/25
* 功能：wifi+weather+oled   arduino端
* 作者：单片机菜鸟
* 16X16点阵显示 取模方式 阴码+逐行式+顺向
**/

#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "code.cpp"

#define OLED_RESET 4
#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

#define DEBUG //是否开启debug功能
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

#ifdef DEBUG
#define DBGLN(message)    MyDebugSerial.println(message)
#else
#define DBGLN(message)
#endif

#ifdef UNO 
#define PIN_KEY 7// 按键
#else
#define PIN_KEY 6  
#endif 

Adafruit_SSD1306 display(OLED_RESET);

const unsigned long BAUD_RATE = 115200;                   // serial connection speed
const size_t MAX_CONTENT_SIZE = 1000;                   // max size of the HTTP response
char response[MAX_CONTENT_SIZE];

// 定义记录按键当前状态的变量
int state_btn;
// 定义记录按键最近一次状态变化的变量，并初始化状态为LOW。
int lastButtonState = LOW;
// 定义记录最近一次抖动的时间变量，并初始化时间为0毫秒。
long lastDebounceTime = 0;
// 定义延迟抖动的时间变量
long debouncdDelay = 60;

// 我们要从此网页中提取的数据的类型
struct UserData {
  char city[16];//城市名称
  char weather[32];//天气介绍（多云...）
  char temp[16];//温度
  char udate[32];//更新时间
};

/**
* @Desc 初始化操作
*/
void setup()   {
  pinMode(PIN_KEY,INPUT);
  WifiSerial.begin(BAUD_RATE);
  #ifdef DEBUG
    #ifdef UNO
      MyDebugSerial.begin(9600);//软串口9600稳定
    #else
      MyDebugSerial.begin(BAUD_RATE);
    #endif
  #endif
  delay(500);
  DBGLN("Arduino Init End");
                  
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  display.clearDisplay();   // clears the screen and buffer
  //显示欢迎界面
  display.ShowCN_16(0,16, Welcome_16x16,sizeof(Welcome_16x16)/32,WHITE);
  display.ShowCN_16(48,32, Author_16x16,sizeof(Author_16x16)/32,WHITE);
  display.display();
  delay(2000);
  display.clearDisplay();   // clears the screen and buffer
  display.display();  
}

void loop() {
  if(WifiSerial.available()>0){
    clrEsp8266ResponseBuffer();
    int data_size = ReceiveMessage(response, sizeof(response));
    if(data_size>0){
      //开始解析数据
      UserData userData;
      parseData(response,&userData);
    }
  }
  checkButton();
}

/**
* 读取串口缓冲区里面的数据
*/
int ReceiveMessage(char* content, size_t maxSize){
  size_t length = WifiSerial.readBytes(content, maxSize);
  delay(100);
  content[length] = 0;
  WifiSerial.flush();
  DBGLN(content);
  return length;
}

/**
 * 数据格式如下：
 * {
 *    "results": [
 *        {
 *            "location": {
 *                "id": "WX4FBXXFKE4F",
 *                "name": "北京",
 *                "country": "CN",
 *                "path": "北京,北京,中国",
 *                "timezone": "Asia/Shanghai",
 *                "timezone_offset": "+08:00"
 *            },
 *            "now": {
 *                "text": "多云",
 *                "code": "4",
 *                "temperature": "23"
 *            },
 *            "last_update": "2017-09-13T09:51:00+08:00"
 *        }
 *    ]
 *}
 */
bool parseData(char* content,struct UserData* userData) {
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
 
  //复制我们感兴趣的字符串
  strcpy(userData->city, root["results"][0]["location"]["name"]);
  strcpy(userData->weather, root["results"][0]["now"]["text"]);
  strcpy(userData->temp, root["results"][0]["now"]["temperature"]);
  strcpy(userData->udate, root["results"][0]["last_update"]);
  //  -- 这不是强制复制，你可以使用指针，因为他们是指向“内容”缓冲区内，所以你需要确保
  //   当你读取字符串时它仍在内存中
  return true;
}

/**
* 检查按键功能 进入一键配置模式
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

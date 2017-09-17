/**
* 日期:2017/04/16
* 功能：蓝牙台灯V3.0
* 作者：单片机菜鸟
**/
#include<EEPROM.h>
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
#define PIN_RED 3 //red 引脚
#define PIN_GREEN 5 //green 引脚
#define PIN_BLUE 6 //blue 引脚
#else
#define PIN_RED 2
#define PIN_GREEN 3
#define PIN_BLUE 4
#endif 
 
#define MAX  7
#define KEY  7
 
#define RED_ADDR 0  //红色 eeprom地址
#define BLUE_ADDR 1 //蓝色 eeprom地址
#define GREEN_ADDR 2 //绿色 eeprom地址
 
int red = 0;//红色
int green = 0;//绿色
int blue = 0;//蓝色
boolean isNeedtoSave = false;//是否需要保存
int saveCountX = 0;//统计当前次数
int saveCountY = 0;//统计当前次数
 
// 定义记录按键当前状态的变量
int state_btn;
// 定义记录按键最近一次状态变化的变量，并初始化状态为LOW。
int lastButtonState = LOW;
// 定义记录最近一次抖动的时间变量，并初始化时间为0毫秒。
long lastDebounceTime = 0;
// 定义延迟抖动的时间变量
long debouncdDelay = 60;
 
int currentcode = 0;
 
//rgb编码
unsigned int rgb_code[MAX][3] = {
  {255,255,255},//白光
  {255,0,0},//红光
  {0,255,0},//绿光
  {0,0,255},//蓝光
  {255,255,0},//黄光
  {0,255,255},//浅蓝
  {255,0,255}//浅紫
};
 
void setup() {
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE, OUTPUT);
  //配置10号引脚为输入引脚 如果是自己连接普通按键 pinMode(KEY,INPUT_PULLUP),弱上拉
  pinMode(KEY,INPUT);
  BTSerial.begin(9600);
  #ifdef DEBUG
    MyDebugSerial.begin(9600);
  #endif
  init_RGB();
}
 
void loop() {
  int leng = ReceiveBTMessage();
  if(leng>0){
    //收到了新RGB数值
    colorRGB(red,green,blue);
  }
  changeRGBbyButton();
  saveEEPROM();
}
 
/**
* 初始化RGB灯
*/
void init_RGB(){
  red = EEPROM.read(RED_ADDR);
  DBGLN(red);
  delay(10);
  green = EEPROM.read(GREEN_ADDR);
  delay(10);
   DBGLN(green);
  blue = EEPROM.read(BLUE_ADDR);
  delay(10);
  DBGLN(blue);
  colorRGB(red,green,blue);
}
 
void saveEEPROM(){
  //以下这段代码是为了存储eeprom  因为定时中断不能用 所以就用了这么猥琐的代码
  if(saveCountX == 30){
 
    if(saveCountY == 30000){
      saveCountX = 0;
      saveCountY = 0;
      if(isNeedtoSave){
        isNeedtoSave = false;
        saveRGBtoEEPROM();
      }
    }else{
      saveCountY ++;
    }
  }else{
    if(saveCountY == 30000){
      saveCountX ++;
      saveCountY = 0;
    }else{
      saveCountY ++;
    }
  }
}
 
/**
* 把rgb数据保存到EEPROM上去
*/
void saveRGBtoEEPROM(){
  DBGLN("saveRGBtoEEPROM");
  DBGLN(red);
  DBGLN(green);
  DBGLN(blue);
  EEPROM.write(RED_ADDR,red);
  delay(50);
  EEPROM.write(GREEN_ADDR,green);
  delay(50);
  EEPROM.write(BLUE_ADDR,blue);
  delay(50);
}
 
/**
* 读取串口缓冲区里面的数据
*/
int ReceiveBTMessage(){
  /**
     * @Desc 把RGB组合成协议内的数据格式
     *  开始符+长度+rgb+结束符
     * "+C,n:rgbString;"
     * 例子:"+C,10:100,255,60;"
     **/
  String data = "";
  if (BTSerial.available()>0){
    unsigned long start;
    start = millis();
    char c0 = BTSerial.read();
    if (c0 == '+'){
      char c;
      boolean data_right = false;
      while (millis()-start<200) {
        if (BTSerial.available()>0){
          c = BTSerial.read();
          delay(1);
          data += c;
        }
        if (c==';'){
          data_right = true;
          break;
        }
      }
      DBGLN(data);
      if(!data_right)return;
 
      int sLen = strlen(data.c_str());
      int i,r,g,b;
      for (i = 0; i <= sLen; i++){
        if (data[i] == ':'){
          break;
        }
      }
      int iSize;
      //"+C,10:100,255,60;"    
      String _size = data.substring(2, i);
      iSize = _size.toInt();
 
      //"+C,10:1,1,1;"
      if(iSize!=sLen-i-2)return;
 
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
 
      isNeedtoSave = true;
      //变成0
      saveCountX = 0;
      saveCountY = 0;
       
      return iSize;
    }else{
      while(BTSerial.read() >= 0){}  
    }
  }
  return 0;
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
* 按键切换显示（应用于无app情景）
*/
void changeRGBbyButton(){
  int buttonState = digitalRead(KEY);//读取当前按键状态
 
  if(buttonState != lastButtonState){
     //如果按键发生了变化  则重新设置最近一次抖动的时间
     //方法millis()可以获取当前时间，单位统一为毫秒。
     lastDebounceTime = millis();  
  }
   
  // 判断按键按下或抬起的状态时间间隔是否大于延迟抖动的时间长度。
  if(millis()-lastDebounceTime>debouncdDelay){
    // 判断当前的按键状态是否和之前有所变化
    if(buttonState != state_btn){
       // 如果发生了变化，
       // 则更新按键状态变量。
       state_btn = buttonState;
       if(state_btn == HIGH){
        //再次确认是否真的按下了按键,是的话
         currentcode++;
         if(currentcode == MAX){
             currentcode = 0;
         }
         red = rgb_code[currentcode][0];
         green = rgb_code[currentcode][1];
         blue = rgb_code[currentcode][2];
         colorRGB(red,green,blue);
         isNeedtoSave = true;
        //变成0
        saveCountX = 0;
        saveCountY = 0;
       }
    }
  }
  // 更新按键最近一次状态变化的变量
     lastButtonState = buttonState;
}

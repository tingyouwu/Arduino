#include <SoftwareSerial.h>
/*
日期:2016/11/18
功能：wifi小车程序
作者：单片机菜鸟
*/

#define IN1 6 // 7 6 右轮
#define IN2 7 
#define IN3 4 // 5 4 左轮
#define IN4 5 

#define LEFT "3" //左转编码
#define RIGHT "4"//右转编码
#define GO "1"//前进编码
#define BACK "2"//后退编码
#define STOP "0"//停止编码

SoftwareSerial mySerial(10,11);

int ReceiveWifiMessage(char *buf);

void setup() {
  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);
  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);
  Serial.begin(9600);
  delay(1000);
  //开启多连接模式命令
  mySerial.println("AT+CIPMUX=1");
  //开启服务器模式    8080为端口号，手机连接时会用到
  delay(2000);
  mySerial.println("AT+CIPSERVER=1,8080");
  delay(3000);
  while(Serial.read() >= 0){}
  while(mySerial.read() >= 0){}
  initCar();
  Serial.println("wificar init");
}
 
void loop() {
    char buf[100];
    int leng = ReceiveWifiMessage(buf);

    if(leng>0){
      if(strcmp(buf, GO) == 0){
        Serial.println("go");
         //前进
         go();
      }else if(strcmp(buf, BACK) == 0){
        Serial.println("back");
         //后退
         back();
      }else if(strcmp(buf, LEFT) == 0){
         //左转
         Serial.println("left");
         turnLeft();
      }else if(strcmp(buf, RIGHT) == 0){
        //右转
        Serial.println("right");
        turnRight();
      }else if(strcmp(buf, STOP) == 0){
        //停车
        Serial.println("stop");
        stopCar();
      }
    }
}

void initCar(){
  //默认全是低电平 停止状态
  digitalWrite(IN1,LOW);      
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW);   
  digitalWrite(IN4,LOW);
}

/**
* 左转
*/
void turnLeft(){
  digitalWrite(IN1,HIGH);      
  digitalWrite(IN2,LOW);         //右轮前进
  digitalWrite(IN3,LOW);      
  digitalWrite(IN4,LOW);         //左轮不动
}

/**
* 右转
*/
void turnRight(){
  digitalWrite(IN1,LOW);      
  digitalWrite(IN2,LOW);         //右轮不动
  digitalWrite(IN3,HIGH);      
  digitalWrite(IN4,LOW);         //左轮前进
}

/**
* 前进
*/
void go(){
  digitalWrite(IN1,HIGH);      
  digitalWrite(IN2,LOW);         //右轮前进
  digitalWrite(IN3,HIGH);      
  digitalWrite(IN4,LOW);         //左轮前进
}

/**
* 倒车
*/
void back(){
  digitalWrite(IN1,LOW);      
  digitalWrite(IN2,HIGH);        //右轮后退
  digitalWrite(IN3,LOW);      
  digitalWrite(IN4,HIGH);        //左轮后退
}

/**
* 停车
*/
void stopCar(){
  initCar();
}

/**
* 读取串口缓冲区里面的数据
*/
int ReceiveWifiMessage(char *buf){
  //+IPD,<len>:<data>
  //+IPD,<id>,<len>:<data>
  String data = "";
  if (mySerial.available()>0){
    unsigned long start;
    start = millis();
    char c0 = mySerial.read();
    if (c0 == '+')
    {
      while (millis()-start<100) 
      {
        if (mySerial.available()>0)
        {
          char c = mySerial.read();
          data += c;
        }
        if (data.indexOf("\nOK")!=-1)
        {
          break;
        }
      }

      int sLen = strlen(data.c_str());
      int i,j;
      for (i = 0; i <= sLen; i++){
        if (data[i] == ':'){
          break;
        }
      }
      //判断是否存在id
      boolean found = false;
      for (j = 4; j <= i; j++){
        if (data[j] == ','){
          found = true;
          break;
        }
      }
      int iSize;
      if(found ==true){
        //+IPD,<id>,<len>:<data>
          String _size = data.substring(j+1, i);
          iSize = _size.toInt();
          String str = data.substring(i+1, i+1+iSize);
          strcpy(buf, str.c_str());     
      }else{
        //+IPD,<len>:<data>     
          String _size = data.substring(4, i);
          iSize = _size.toInt();
          String str = data.substring(i+1, i+1+iSize);
          strcpy(buf, str.c_str());
      }
      return iSize;
    }else{
      while(mySerial.read() >= 0){}  
    }
  }
  return 0;
}


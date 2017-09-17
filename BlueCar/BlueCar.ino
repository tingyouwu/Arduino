/*
日期:2016/11/05
功能：蓝牙小车程序
作者：单片机菜鸟
*/

#define IN1 6 // 7 6 右轮
#define IN2 7 
#define IN3 4 // 5 4 左轮
#define IN4 5 

#define LEFT '3' //左转编码
#define RIGHT '4'//右转编码
#define GO '1'//前进编码
#define BACK '2'//后退编码
#define STOP '0'//停止编码

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);
  initCar();
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available()>0){
      char ch = Serial.read();
      if(ch == GO){
         //前进
         go();
      }else if(ch == BACK){
         //后退
         back();
      }else if(ch == LEFT){
         //左转
         turnLeft();
      }else if(ch == RIGHT){
        //右转
        turnRight();
      }else if(ch=='0'){
        //停车
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



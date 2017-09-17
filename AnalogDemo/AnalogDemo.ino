/**
* @Desc  可调呼吸灯
* @author 单片机菜鸟
* @Date  2016/12/8
* 注意：LED的一端接到5V，另外一端接到输出引脚，低电平亮
*       按键是高电平有效
*/

#define LED 9

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  //从暗到亮，每次亮度值加5，直到最亮
  for(int fadeValue = 0;fadeValue<=255;fadeValue+=5){
     //输出PWM
     analogWrite(LED,fadeValue);
     //延时一下
     int delaytime = analogRead(A0)/5;//读取电位器输出电压 用于延时
     delay(delaytime);  
  }

  //从亮到暗 每次亮度-5直到最暗
  for(int fadeValue=255;fadeValue>=0;fadeValue-=5){
     //输出PWM
     analogWrite(LED,fadeValue);
     //延时一下
     int delaytime = analogRead(A0)/5;
     delay(delaytime);  
  }
}

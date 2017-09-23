/**
* 日期:2017/09/22
* 功能：wifi+weather+oled
* 作者：单片机菜鸟
**/
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

/**
* 16X16点阵显示 取模方式 阴码+逐行式+顺向
*/
static const char PROGMEM Welcome2_16x16[] ={
0x00,0x40,0x40,0x40,0x23,0xF8,0x20,0x40,0x00,0x40,0x07,0xFE,0x11,0x10,0x12,0x88,  
0x24,0x84,0xE1,0xF0,0x23,0x10,0x24,0xA0,0x20,0x40,0x20,0xA0,0x23,0x10,0x0C,0x0C
}; 

void setup()   {                
  Serial.begin(9600);
  delay(500);
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  display.clearDisplay();   // clears the screen and buffer
  delay(500);
  //中文字符显示  
  display.drawBitmap(32,16, Welcome2_16x16, 16, 16, WHITE);
  display.display();  
}

void loop() {
  
}

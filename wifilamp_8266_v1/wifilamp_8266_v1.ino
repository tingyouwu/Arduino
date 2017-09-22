/**
* 日期:2017/09/19
* 功能：wifi lamp 8266端
*       加入SmartConfig功能
* 作者：单片机菜鸟
**/
#include <ESP8266WiFi.h>
#include <EEPROM.h>

#define MAX_SRV_CLIENTS 3   //最大同时联接数，即你想要接入的设备数量，8266tcpserver只能接入五个，哎
#define led 2
#define DEBUG //是否开启debug功能

#ifdef DEBUG
#define DebugPrintln(message)    Serial.println(message)
#else
#define DebugPrintln(message)
#endif

#ifdef DEBUG
#define DebugPrint(message)    Serial.print(message)
#else
#define DebugPrint(message)
#endif
  
const char* ssid     = "360wifilulu";         // XXXXXX -- 使用时请修改为当前你的 wifi ssid
const char* password = "6206908you11011010";         // XXXXXX -- 使用时请修改为当前你的 wifi 密码
  
const unsigned long BAUD_RATE = 115200;                   // serial connection speed
const unsigned long HTTP_TIMEOUT = 5000;               // max respone time from server
  
WiFiServer server(8266);//你要的端口号，随意修改，范围0-65535
WiFiClient serverClients[MAX_SRV_CLIENTS];

//ssid和密码结构体
struct config_type{
  char ssid[32];
  char psw[64];
};

config_type config;
int flag = HIGH;//默认当前灭灯
  
/**
* @Desc 初始化操作
*/
void setup() {
  Serial.begin(BAUD_RATE);
  pinMode(led,OUTPUT);
  digitalWrite(led, HIGH);

  if(!autoConfig()){
    smartConfig();
    DebugPrint("Connecting to ");//写几句提示，哈哈
    DebugPrintln(ssid);
    while (WiFi.status() != WL_CONNECTED) {
    //这个函数是wifi连接状态，返回wifi链接状态
       delay(500);
       DebugPrint(".");
    }
  }
  
  delay(1000);
  digitalWrite(led, LOW);
  DebugPrintln("IP address: ");
  DebugPrintln(WiFi.localIP());//WiFi.localIP()返回8266获得的ip地址
  server.begin();
  server.setNoDelay(true);  //加上后才正常些
}
  
/**
* @Desc  主函数
*/
void loop() {
  uint8_t i;
  if (server.hasClient()){
        for (i = 0; i < MAX_SRV_CLIENTS; i++){
            if (!serverClients[i] || !serverClients[i].connected()){
                if (serverClients[i]) serverClients[i].stop();//未联接,就释放
                serverClients[i] = server.available();//分配新的
                continue;
            }
        }
        //8266tcpserver只能接入五个  超出的需要释放
        WiFiClient serverClient = server.available();
        if (serverClient){
          serverClient.stop();
        }
  }
  
  for (i = 0; i < MAX_SRV_CLIENTS; i++){
        if (serverClients[i] && serverClients[i].connected()){
            //处理客户端发过来的数据
            if (serverClients[i].available()){
                while (serverClients[i].available()) 
                    Serial.write(serverClients[i].read());
            }
        }
   }

   if(Serial.available()>0){
      char ch = Serial.read();
      if(ch == '1'){
        smartConfig();
        delay(1000);
        digitalWrite(led, LOW);
        DebugPrintln("IP address: ");
        DebugPrintln(WiFi.localIP());//WiFi.localIP()返回8266获得的ip地址
      }
   }
}

/**
* 自动连接20s 超过之后自动进入SmartConfig模式
*/
bool autoConfig(){
  WiFi.mode(WIFI_AP_STA);     //设置esp8266 工作模式
  WiFi.begin();
  delay(2000);//刚启动模块的话 延时稳定一下
  DebugPrintln("AutoConfiging ......");
  for(int i=0;i<20;i++){
    int wstatus = WiFi.status();
    if (wstatus == WL_CONNECTED){
      DebugPrintln("AutoConfig Success");
      DebugPrint("SSID:");
      DebugPrintln(WiFi.SSID().c_str());
      DebugPrint("PSW:");
      DebugPrintln(WiFi.psk().c_str());
      return true;
    }else{
      DebugPrint(".");
      delay(1000);
      flag = !flag;
      digitalWrite(led, flag);
    } 
  }
  DebugPrintln("AutoConfig Faild!");
  return false;
}

/**
* 开启SmartConfig功能
*/
void smartConfig()
{
  WiFi.mode(WIFI_STA);
  delay(2000);
  DebugPrintln("Wait for Smartconfig");
  // 等待配网
  WiFi.beginSmartConfig();
  while (1){
    DebugPrint(".");
    delay(500);
    flag = !flag;
    digitalWrite(led, flag);
    
    if (WiFi.smartConfigDone()){
      //smartconfig配置完毕
      DebugPrintln("SmartConfig Success");
      DebugPrint("SSID:");
      DebugPrintln(WiFi.SSID().c_str());
      DebugPrint("PSW:");
      DebugPrintln(WiFi.psk().c_str());
      strcpy(config.ssid, WiFi.SSID().c_str());
      strcpy(config.psw, WiFi.psk().c_str());
      saveConfig();
      WiFi.mode(WIFI_AP_STA);     //设置esp8266 工作模式
      WiFi.setAutoConnect(true);  // 设置自动连接
      break;
    }
  }
}
 
/*
保存参数到eeprom
 */
void saveConfig()
{
  EEPROM.begin(1024);
  uint8_t *p = (uint8_t*)(&config);
  for (int i = 0; i < sizeof(config); i++)
  {
    EEPROM.write(i, *(p + i));
  }
  EEPROM.commit();
}
 
/*
   从EEPROM加载参数
*/
void loadConfig()
{
  EEPROM.begin(1024);
  uint8_t *p = (uint8_t*)(&config);
  for (int i = 0; i < sizeof(config); i++)
  {
    *(p + i) = EEPROM.read(i);
  }
  EEPROM.commit();
}

/**
* 日期:2017/09/14
* 功能：wifi lamp 8266端
* 作者：单片机菜鸟
**/
#include <ESP8266WiFi.h>

#define MAX_SRV_CLIENTS 3   //最大同时联接数，即你想要接入的设备数量，8266tcpserver只能接入五个，哎
#define led 2
  
const char* ssid     = "360wifilulu";         // XXXXXX -- 使用时请修改为当前你的 wifi ssid
const char* password = "6206908you11011010";         // XXXXXX -- 使用时请修改为当前你的 wifi 密码
  
const unsigned long BAUD_RATE = 115200;                   // serial connection speed
const unsigned long HTTP_TIMEOUT = 5000;               // max respone time from server
  
WiFiServer server(8266);//你要的端口号，随意修改，范围0-65535
WiFiClient serverClients[MAX_SRV_CLIENTS];
  
/**
* @Desc 初始化操作
*/
void setup() {
  WiFi.mode(WIFI_AP_STA);     //设置esp8266 工作模式
  Serial.begin(BAUD_RATE);
  pinMode(led,OUTPUT);
  digitalWrite(led, HIGH);
  delay(2000);//刚启动模块的话 延时稳定一下
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");//写几句提示，哈哈
  Serial.println(ssid);
  WiFi.begin(ssid, password);   //连接wifi
  while (WiFi.status() != WL_CONNECTED) {
    //这个函数是wifi连接状态，返回wifi链接状态
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  delay(500);
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());//WiFi.localIP()返回8266获得的ip地址
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
}

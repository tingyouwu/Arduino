/**
 * Demo：
 *    演示web Server功能
 *    打开PC浏览器 输入IP地址。请求web server
 * @author 单片机菜鸟
 * @date 2019/09/05
 */
#include <ESP8266WiFi.h>

const char* ssid = "TP-LINK_5344";
const char* password = "6206908you11011010";

//创建 tcp server
WiFiServer server(80);

void setup(){
  Serial.begin(115200);
  Serial.println();

  Serial.printf("Connecting to %s ", ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");
  //启动TCP 连接
  server.begin();
  //打印TCP server IP地址
  Serial.printf("Web server started, open %s in a web browser\n", WiFi.localIP().toString().c_str());
}

/**
 * 模拟web server 返回http web响应内容
 * 这里是手动拼接HTTP响应内容
 * 后面楼主会继续讲解另外两个专用于http请求的库
 */
String prepareHtmlPage(){
  String htmlPage =
     String("HTTP/1.1 200 OK\r\n") +
            "Content-Type: text/html\r\n" +
            "Connection: close\r\n" +  // the connection will be closed after completion of the response
            "Refresh: 5\r\n" +  // refresh the page automatically every 5 sec
            "\r\n" +
            "<!DOCTYPE HTML>" +
            "<html>" +
            "Analog input:  " + String(analogRead(A0)) +
            "</html>" +
            "\r\n";
  return htmlPage;
}


void loop(){
  WiFiClient client = server.available();
  // wait for a client (web browser) to connect
  if (client){
    Serial.println("\n[Client connected]");
    while (client.connected()){
      // 不断读取请求内容
      if (client.available()){
        String line = client.readStringUntil('\r');
        Serial.print(line);
        // wait for end of client's request, that is marked with an empty line
        if (line.length() == 1 && line[0] == '\n'){
          //返回响应内容
          client.println(prepareHtmlPage());
          break;
        }
      }
    }
    delay(100); // give the web browser time to receive the data

    // close the connection:
    client.stop();
    Serial.println("[Client disonnected]");
  }
}

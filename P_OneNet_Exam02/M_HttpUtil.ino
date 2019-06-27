/**
 * 功能：Http操作方法库
 * 作者：单片机菜鸟
 * 时间：2019-06-27
 */

#include "H_project.h"

extern WiFiClient client;
extern char response[MAX_CONTENT_SIZE];
extern char endOfHeaders[];
/**
* 发送Get请求
* @param host
* @param getUrl 服务地址
* @param header 请求头
*/
bool sendGetRequest(const char* host,String url,String header,char* response){

    if (!client.connected()) {
        Serial.println("Client is not readly");
        return false;
    }
    // This will send the request to the server
    String httpGet = String("GET ") + url
                        + " HTTP/1.1\r\n"
                        + "Host: " + host + "\r\n"
                        + header + "\r\n"
                        + "Connection: Keep-Alive\r\n\r\n";
    client.print(httpGet);
    Serial.println("Get request:");
    Serial.println(httpGet);

    //判断是否有结果返回 如果超过5s没有内容返回 提示失败
    int cnt = 0;
    while(!client.available()){
         delay(500);
         ESP.wdtFeed();
         cnt ++;
         if(cnt >= 10){
              cnt = 0;
              Serial.println("No response or invalid response!");
              return false;
          }
    }

    if(skipResponseHeaders()){
         //清除缓冲
         clearResponseBuffer(response);
         //读取响应数据
         readReponseContent(response, sizeof(response));
         return true;
    }else{
         return false;
    }
}

/**
* 发送Post请求
* @param host
* @param getUrl 服务地址
* @param header 请求头
*/
bool sendPostRequest(const char* host,String url,String header,String postData,char* response) {

    if (!client.connected()) {
        Serial.println("Client is not readly");
        return false;
    }
    // We now create a URI for the request
    String httpPost = String("POST ") + url
                         + " HTTP/1.1\r\n"
                         + "Host: " + host + "\r\n"
                         + header + "\r\n"
                         + "Content-Type: application/json\r\n"
                         + "Content-Length: " + postData.length() + "\r\n"
                         + "Connection: Keep-Alive\r\n\r\n"+ postData +"\r\n";
    client.print(httpPost);
    Serial.println("Post request:");
    Serial.println(httpPost);

    //判断是否有结果返回 如果超过5s没有内容返回 提示失败
    int cnt = 0;
    while(!client.available()){
        delay(500);
        ESP.wdtFeed();
        cnt ++;
        if(cnt >= 10){
          cnt = 0;
          Serial.println("No response or invalid response!");
          return false;
        }
    }

    if(skipResponseHeaders()){
        //清除缓冲
        clearResponseBuffer(response);
        //读取响应数据
        readReponseContent(response);
        return true;
    }else{
      return false;
    }
}

/**
* 跳过 HTTP 头，使我们在响应正文的开头
* @return bool 是否找到响应头
*/
bool skipResponseHeaders() {
    // HTTP headers end with an empty line
    bool ok = client.find(endOfHeaders);
    if(!ok){
       Serial.println("No response or invalid response!");
    }
    return ok;
}

/**
* 从HTTP服务器响应中读取正文
* @param content 存储正文内容
* @param maxSize 最大内容大小
*/
void readReponseContent(char* content) {
   size_t length = client.readBytes(content, MAX_CONTENT_SIZE);
   Serial.println("Get the data from Internet!");
   content[length] = 0;
   Serial.println(content);
   Serial.println("Read data Over!");
   client.flush();//这句代码需要加上  不然会发现每隔一次client.find会失败
}

/**
 * 清除缓冲区
 */
void clearResponseBuffer(char* response){
    memset(response, 0, MAX_CONTENT_SIZE);      //清空
}

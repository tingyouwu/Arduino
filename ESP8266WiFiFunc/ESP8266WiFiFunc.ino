//以下是ESP8266WiFiSTA库
/**
* 切换工作模式到STA模式，并自动连接到最近接入的wifi热点
* @param void
* @return void
* @note 调用这个方法就会切换到STA模式，并且连接到最近使用的接入点（会从flash中读取之前存储的配置信息）
*       如果没有配置信息，那么这个方法基本上没有什么用。
*/
wl_status_t begin()

/**
* 切换工作模式到STA模式，并根据connect属性来判断是否连接wifi
* @param ssid       wifi热点名字
* @param password   wifi热点密码
* @param channel    wifi热点的通道号，用特定通信通信，可选参数
* @param bssid      wifi热点的mac地址，可选参数
* @param connect    boolean参数，默认等于true，当设置为false，不会去连接wifi热点，会建立module保存上面参数
* @return wl_status_t  wifi状态
* @note 调用这个方法就会切换到STA模式。
*       如果connect等于true，会连接到ssid的wifi热点。
*       如果connect等于false，不会连接到ssid的wifi热点，会建立module保存上面参数。
*/
wl_status_t begin(char* ssid, char *passphrase = NULL, int32_t channel = 0, const uint8_t* bssid = NULL, bool connect = true)

/**
* 禁止DHCP client，设置station 模式下的IP配置
* @param  local_ip    station固定的ip地址
* @param  gateway     网关
* @param  subnet      子网掩码
* @param  dns1，dns2  可选参数定义域名服务器（dns）的ip地址，这些域名服务器
*                     维护一个域名目录（如www.google.co.uk），并将它们翻译成ip地址  
* @return boolean值，如果配置成功，返回true；
*         如果配置没成功（模块没处于station或者station+soft AP模式），返回false；
* @note   有着固定IP配置地址的station，通常会更快连接上网络，原因是通过DHCP client获得IP配置这一步被跳过了。        
*         如果你把三个参数（local_ip, gateway and subnet）设置为0.0.0.0，那么它会重新启动DHCP，这时你需要重新连接wifi以拿到最新的IP
*/
bool config(IPAddress local_ip, IPAddress gateway, IPAddress subnet, IPAddress dns1 = (uint32_t)0x00000000, IPAddress dns2 = (uint32_t)0x00000000)

/**
* 断开连接并且重新连接station到同一个AP
* @param void
* @return false or true
*         返回false，意味着ESP8266不处于STA模式或者说Station在此之前没有连接到一个可接入点。
*         返回true，意味着已经成功重新启动连接，但是用户仍应该去检测网络连接状态指导WL_CONNECTED。
*/
bool reconnect()

/**
* 使用案例
*/
WiFi.reconnect();
while (WiFi.status() != WL_CONNECTED)
{
  delay(500);
  Serial.print(".");
}

/**
* 断开wifi连接，设置当前配置SSID和pwd为null
* @param wifioff 可选参数，设置为true，那么就会关闭Station模式
* @return false or true 返回wl_status_t状态
*/
bool disconnect(bool wifioff = false);

/**
* 判断STA模式下是否连接上AP
* @return 如果STA连接上AP，那么就返回true
*/
bool isConnected();

/**
* 当电源启动后，设置ESP8266在STA模式下是否自动连接flash中存储的AP
* @param autoConnect bool 默认是自动连接
* @return 返回保存状态 true or false
*/
bool setAutoConnect(bool autoConnect);

/**
* 检测ESP8266 station模式下是否启动自动连接
* @return 返回自动连接状态 true or false
*/
bool getAutoConnect();

/**
* 设置当断开连接的时候是否自动重连
* @param autoConnect bool
* @return 返回保存状态 true or false
* @note 如果在网络已经断开了之后才去设置setAutoReconnect(true)，这是无效的
*/
bool setAutoReconnect(bool autoReconnect);

/**
* 等待直到ESP8266连接AP返回结果
* @return uint8_t 连接结果
*         1.WL_CONNECTED 成功连接
*         2.WL_NO_SSID_AVAIL  匹配SSID失败（账号错误）
*         3.WL_CONNECT_FAILED psw错误
*         4.WL_IDLE_STATUS 当wi-fi正在不同的状态中变化
*         5.WL_DISCONNECTED 这个模块没有配置STA模式
* @note 如果在网络已经断开了之后才去设置setAutoReconnect(true)，这是无效的
*/
uint8_t waitForConnectResult();

/**
 * 获取ESP station下的Mac地址
 * @param mac   uint8_t数组的指针，数组长度为Mac地址的长度，这里为6
 * @return      返回uint8_t数组的指针
 */
uint8_t * macAddress(uint8_t* mac);
//实例代码
if (WiFi.status() == WL_CONNECTED)
{
  uint8_t macAddr[6];
  WiFi.macAddress(macAddr);
  Serial.printf("Connected, mac address: %02x:%02x:%02x:%02x:%02x:%02x\n", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
  //Connected, mac address: 5C:CF:7F:08:11:17
}

/**
 * 获取ESP station下的Mac地址
 * @return  返回String的Mac地址
 */
String macAddress();
//实例代码
if (WiFi.status() == WL_CONNECTED)
{
  Serial.printf("Connected, mac address: %s\n", WiFi.macAddress().c_str());
  ////Connected, mac address: 5C:CF:7F:08:11:17
}

/**
 * 返回ESP8266 STA模式下的IP地址
 * @return IP地址
 */
IPAddress localIP();
//实例代码
if (WiFi.status() == WL_CONNECTED)
{
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
  //Connected, IP address: 192.168.1.10
}

/**
 * 获取子网掩码的地址
 * @return 返回子网掩码的IP地址
 */
IPAddress subnetMask();
//实例代码
Serial.print("Subnet mask: ");
Serial.println(WiFi.subnetMask());
//Subnet mask: 255.255.255.0

/**
 * 获取网关IP地址
 * @return 返回网关IP地址
 */
IPAddress gatewayIP();
//实例代码
Serial.printf("Gataway IP: %s\n", WiFi.gatewayIP().toString().c_str());
//Gataway IP: 192.168.1.9

/**
 * 获取DNS ip地址
 * @param dns_no dns序列号
 * @return 返回DNS服务的IP地址
 */
IPAddress dnsIP(uint8_t dns_no = 0);
//实例代码
Serial.print("DNS #1, #2 IP: ");
WiFi.dnsIP().printTo(Serial);
Serial.print(", ");
WiFi.dnsIP(1).printTo(Serial);
Serial.println();
//DNS #1, #2 IP: 62.179.1.60, 62.179.1.61

/**
 * 获取ESP8266 station DHCP的主机名
 * @return 主机名
 */
String hostname();
/**
 * 设置ESP8266 station DHCP的主机名
 * @param aHostname 最大长度:32
 * @return ok
 */
bool hostname(char* aHostname);
bool hostname(const char* aHostname);
bool hostname(String aHostname);
//实例代码
Serial.printf("Default hostname: %s\n", WiFi.hostname().c_str());
WiFi.hostname("Station_Tester_02");
Serial.printf("New hostname: %s\n", WiFi.hostname().c_str());
//Default hostname: ESP_081117
//New hostname: Station_Tester_02

/**
 * 返回wifi的连接状态
 * @return 返回wl_status_t中定义的其中一值，wl_status_t在 wl_definitions.h中定义
 */
wl_status_t status();

/**
 * 返回当前通信网络的SSID
 * @return SSID
 */
String SSID() const;
//实例代码
Serial.printf("SSID: %s\n", WiFi.SSID().c_str());
//SSID: sensor-net
/**
 * 返回当前通信网络的密码
 * @return psk
 */
String psk() const;
/**
 * 返回当前通信网络的mac地址
 * @return bssid uint8_t *
 */
uint8_t * BSSID();
String BSSIDstr();
//实例代码
Serial.printf("BSSID: %s\n", WiFi.BSSIDstr().c_str());
//BSSID: 00:1A:70:DE:C1:68

/**
 * Return the current network RSSI.返回当前通信网络的信号强度，单位是dBm
 * @return  RSSI value
 */
int32_t RSSI();
//实例代码
Serial.printf("RSSI: %d dBm\n", WiFi.RSSI());
//RSSI: -68 dBm

bool beginWPSConfig(void);
/**
 * 启动 SmartConfig
 */
bool beginSmartConfig();
/**
 * 停止 SmartConfig
 */
bool stopSmartConfig();
/**
 * 查找SmartConfig状态来决定是否停止配置
 * @return smartConfig Done
 */
bool smartConfigDone();

//以下是ESP8266WiFiAP库

/**
 * 建立一个AP热点
 * @param ssid              SSID账号 (max 63 char，最大63个字符).
 * @param passphrase        密码(对于WPA2加密类型最少8个字符，对于开放网络设置为NULL)
 * @param channel           WiFi 通道数字, 1 - 13.默认是1
 * @param ssid_hidden       WiFI是否需要隐藏 (0 = broadcast SSID, 1 = hide SSID)，通过它设置别人是否能看到你的WiFi网络
 * @param max_connection    最大的同时连接数  1 - 4.当超过这个数，再多的station想连接也只能等待
 * @param bool 返回设置soft-AP的结果
 */
bool softAP(const char* ssid, const char* passphrase = NULL, int channel = 1, int ssid_hidden = 0, int max_connection = 4);

//开放网络（所谓开放网络也就是，不需要密码，只需要知道AP名字就可以了）
WiFi.softAP(ssid);
//校验式网络（需要输入账号密码）,通道为1，wifi不隐藏，最大连接数=4
WiFi.softAP(ssid, password);
//校验式网络（需要输入账号密码）,通道为2，wifi隐藏，最大连接数=4
WiFi.softAP(ssid, password,2,1);

/**
 * 配置AP信息
 * @param local_ip      AP ip地址
 * @param gateway       网关IP地址
 * @param subnet        子网掩码
 * @note soft-AP 建立的网络，默认的IP地址是192.168.4.1.
 */
bool softAPConfig(IPAddress local_ip, IPAddress gateway, IPAddress subnet);

/**
 * 获取连接到当前softAP的station或者client的数目
 * @return Stations 数目
 */
uint8_t softAPgetStationNum();
//实例代码
Serial.printf("Stations connected to soft-AP = %d\n", WiFi.softAPgetStationNum());
//Stations connected to soft-AP = 2

/**
 * 关闭AP
 * @param wifioff disable mode? true会调用 WiFi.enableAP(false);
 * @return one value of wl_status_t enum
 */
bool softAPdisconnect(bool wifioff = false);

/**
 * 获取softAP的ip地址
 * @return IPAddress softAP IP
 */
IPAddress softAPIP();
//实例代码
Serial.print("Soft-AP IP address = ");
Serial.println(WiFi.softAPIP());
//Soft-AP IP address = 192.168.4.1

/**
 * 获取softAP MAC 地址.
 * @param mac   pointer to uint8_t array with length WL_MAC_ADDR_LENGTH
 * @return      pointer to uint8_t*
 */
uint8_t* softAPmacAddress(uint8_t* mac);
//实例代码
uint8_t macAddr[6];
WiFi.softAPmacAddress(macAddr);
Serial.printf("MAC address = %02x:%02x:%02x:%02x:%02x:%02x\n", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
//MAC address = 5e:cf:7f:8b:10:13

/**
 * 获取softAP MAC 地址.
 * @return String mac
 */
String softAPmacAddress(void);
//实例代码
Serial.printf("MAC address = %s\n", WiFi.softAPmacAddress().c_str());
//MAC address = 5E:CF:7F:8B:10:13

/**
 * Get the configured(Not-In-Flash) softAP SSID name.
 * @return String SSID.
 */
String softAPSSID() const;

/**
 * Get the configured(Not-In-Flash) softAP PSK or PASSWORD.
 * @return String psk.
 */
String softAPPSK() const;


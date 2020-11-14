#include "WiFi.h"
#include "esp_wifi.h"
#include <WiFi.h>
#include <WebServer.h>

double rssi = 0;
String ssid;
String device_Name = "Device 0";
String Tmp_Output = "";
String Tmp_user_Output = "";
const char* APssid     = "ESP32-USR00";
const char* APpassword = "123456789";

const char* STAssid = "ORBI12";
const char* STApassword =  "coolmango116";

double ESP_AP[8] = {-90,-90,-90,-90,-90,-90,-90,-90};
double ESP_USR[3] = {-90,-90,-90}
WiFiServer server1(80);
WebServer server2(80);


uint8_t LED1pin = 4;
bool LED1status = LOW;

uint8_t LED2pin = 5;
bool LED2status = LOW;

unsigned long tl_timer; // Init timer.


void setup()
{
    Serial.begin(115200);

    // Set WiFi to Station and Access Point Mode
    WiFi.mode(WIFI_AP_STA);

    // As STA, connect to local router, receive local IP
    WiFi.begin(STAssid, STApassword);
    Serial.println("Connecting");
    while(WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());

    //Set Web Server as STA
    server2.on("/", handle_OnConnect);
    server2.on("/led1on", handle_led1on);
    server2.on("/led1off", handle_led1off);
    server2.on("/led2on", handle_led2on);
    server2.on("/led2off", handle_led2off);
    server2.onNotFound(handle_NotFound);

    server2.begin();
    Serial.println("HTTP server started");
    
    // As AP, boardcast WiFi signal
    Serial.println("Creating Accesspoint");
    WiFi.softAP(APssid, APpassword);
    Serial.print("IP address:\t");
    Serial.println(WiFi.softAPIP());
    delay(100);

    Serial.println("Setup done");
    server1.begin();

    tl_timer = millis();
}

void loop()
{    
    server2.handleClient();
    if(LED1status)
    {digitalWrite(LED1pin, HIGH);}
    else
    {digitalWrite(LED1pin, LOW);}
    
    if(LED2status)
    {digitalWrite(LED2pin, HIGH);}
    else
    {digitalWrite(LED2pin, LOW);}
    
 

    unsigned long scan_gap = millis();
    if(scan_gap == tl_timer+5000){
        WiFiClient client = server1.available();   // Listen for incoming clients
        Serial.println("scan start");
        int n = WiFi.scanNetworks();
        Serial.println("scan done");
        if (n == 0) {
          Serial.println("no networks found");
        } else {
          Serial.print(n);
          Serial.println(" networks found");
          for(int i=0;i<8;i++){
            ESP_AP[i] = -90;
          }
          Tmp_Output = "";
          Tmp_user_Output = "";
          for (int i = 0; i < n; ++i) {
              rssi = WiFi.RSSI(i);
              ssid = WiFi.SSID(i);
              if(rssi >= -90 && ssid.length() == 10 && ssid.substring(0,8) == "ESP32-AP" ){
                int temp_ind = (ssid.substring(8,10)).toInt();
                Serial.print(temp_ind);
                ESP_AP[temp_ind] = rssi;
                ESP_USR[temp_ind] = rssi;
                Serial.print(": SSID: ");
                Serial.print(ssid);
                Serial.print(" RSSI: ");
                Serial.print(rssi);
                Serial.print("\n");
              }
              if(rssi >= -90 && ssid.length() == 11 && ssid.substring(0,9) == "ESP32-USR" ){
                int temp_ind = (ssid.substring(9,11)).toInt();
                Serial.print(temp_ind);
                ESP_USR[temp_ind] = rssi;
                // Serial.print(": SSID: ");
                // Serial.print(ssid);
                // Serial.print(" RSSI: ");
                // Serial.print(rssi);
                // Serial.print("\n");
              }
              delay(10);
          }
          for(int i=0;i<8;i++){
            Tmp_Output += String(ESP_AP[i])+",";
          }
          for(int i=0;i<3;i++){
            Tmp_user_Output += String(ESP_USR[i])+",";
          }
        }
       tl_timer = millis();
       Serial.println("");
    }
    
    
}


void handle_OnConnect() {
  LED1status = LOW;
  LED2status = LOW;
  Serial.println("GPIO4 Status: OFF | GPIO5 Status: OFF");
  server2.send(200, "text/html", SendHTML(LED1status,LED2status)); 
}

void handle_led1on() {
  LED1status = HIGH;
  Serial.println("GPIO4 Status: ON");
  server2.send(200, "text/html", SendHTML(true,LED2status)); 
}

void handle_led1off() {
  LED1status = LOW;
  Serial.println("GPIO4 Status: OFF");
  server2.send(200, "text/html", SendHTML(false,LED2status)); 
}

void handle_led2on() {
  LED2status = HIGH;
  Serial.println("GPIO5 Status: ON");
  server2.send(200, "text/html", SendHTML(LED1status,true)); 
}

void handle_led2off() {
  LED2status = LOW;
  Serial.println("GPIO5 Status: OFF");
  server2.send(200, "text/html", SendHTML(LED1status,false)); 
}

void handle_NotFound(){
  server2.send(404, "text/plain", "Not found");
}

String SendHTML(uint8_t led1stat,uint8_t led2stat){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<meta http-equiv=\"refresh\" content=\"5\">";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>LED Control</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr +=".button {display: block;width: 80px;background-color: #3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr +=".button-on {background-color: #3498db;}\n";
  ptr +=".button-on:active {background-color: #2980b9;}\n";
  ptr +=".button-off {background-color: #34495e;}\n";
  ptr +=".button-off:active {background-color: #2c3e50;}\n";
  ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<p>Device ID:";
  ptr += device_Name;
  ptr += "</p>\n";
  ptr +="<p>ESP_AP[";
  ptr += Tmp_Output;
  ptr += "]</p>\n";
  ptr +="<p>ESP_USER[";
  ptr += Tmp_user_Output;
  ptr += "]</p>\n";
    ptr +="<h3>Using Station(STA) Mode</h3>\n";

  ptr += "<form action=\"/action_page.php\">";
  ptr += "<label for=\"fname\">First name:</label>";
  ptr += "<input type=\"text\" id=\"fname\" name=\"fname\"><br><br>";
  ptr += "<label for=\"lname\">Last name:</label>";
  ptr += "<input type=\"text\" id=\"lname\" name=\"lname\"><br><br>";
  ptr += "<input type=\"submit\" value=\"Submit\">";
  ptr += "</form>";
  

  
   if(led1stat)
  {ptr +="<p>LED1 Status: ON</p><a class=\"button button-off\" href=\"/led1off\">OFF</a>\n";}
  else
  {ptr +="<p>LED1 Status: OFF</p><a class=\"button button-on\" href=\"/led1on\">ON</a>\n";}

  if(led2stat)
  {ptr +="<p>LED2 Status: ON</p><a class=\"button button-off\" href=\"/led2off\">OFF</a>\n";}
  else
  {ptr +="<p>LED2 Status: OFF</p><a class=\"button button-on\" href=\"/led2on\">ON</a>\n";}

  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}

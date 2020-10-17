#include "WiFi.h"
#include "esp_wifi.h"


const char* ssid     = "ESP32-Access-Point";
const char* password = "123456789";
WiFiServer server(80);

void setup()
{
    Serial.begin(115200);

    // Set WiFi to station mode and disconnect from an AP if it was previously connected
    WiFi.mode(WIFI_AP_STA);
    Serial.println("Creating Accesspoint");
    WiFi.softAP(ssid, password);
    Serial.print("IP address:\t");
    Serial.println(WiFi.softAPIP());
    delay(100);

    Serial.println("Setup done");
    server.begin();
}

void loop()
{
    WiFiClient client = server.available();   // Listen for incoming clients
    Serial.println("scan start");

    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
    Serial.println("scan done");
    if (n == 0) {
        Serial.println("no networks found");
    } else {
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < n; ++i) {
            // Print SSID and RSSI for each network found
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.print(")");
            Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
            delay(10);
        }
    }
    Serial.println("");

    // Wait a bit before scanning again
    delay(5000);
}

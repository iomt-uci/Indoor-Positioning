/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

// Libraries for SD card
#include "FS.h"
#include "SD.h"
#include <SPI.h>

// Libraries for WiFi
#include "WiFi.h"

// Define CS pin for the SD card module
#define SD_CS 5


// Initial Timestamp
unsigned long init_time = millis();

// Save reading number on RTC memory
RTC_DATA_ATTR int readingID = 0;
double rssi = 0;
String room_num = "0.0"; // CT Room
String ssid;
String dataMessage;

void setup() {
  // Start serial communication for debugging purposes
  Serial.begin(115200);
  Serial.print("init time");
  Serial.println(init_time);

  // Initialize WiFi
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  
  Serial.println("Setup done");

  // Initialize SD card
  SD.begin(SD_CS);  
  if(!SD.begin(SD_CS)) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }
  Serial.println("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("ERROR - SD card initialization failed!");
    return;    // init failed
  }

  // If the data.txt file doesn't exist
  // Create a file on the SD card and write the data labels
  File file = SD.open("/RSSI_data.txt");
  if(!file) {
    Serial.println("File doens't exist");
    Serial.println("Creating file...");
    writeFile(SD, "/RSSI_data.txt", "ReadingID, SSID, RSSI, Room_Num\r\n");
  }
  else {
    Serial.println("File already exists");  
  }
  file.close();

}

void loop() {
  // The ESP32 will be in deep sleep
  // it never reaches the loop()
  unsigned long end_time = millis();
  if(end_time < init_time + 300000){
    load_Data();
    Serial.print("end time");
    Serial.println(end_time);
  }
  
}

// Function to get RSSI
void load_Data(){
  int n = WiFi.scanNetworks();
    Serial.println("scan done");
    if (n == 0) {
        Serial.println("no networks found");
    } else {
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < n; ++i) {
            // Print SSID and RSSI for each network found
            
            rssi = WiFi.RSSI(i);
            ssid = WiFi.SSID(i);
            if(rssi >= -90 && ssid.length() == 10 && ssid.substring(0,8) == "ESP32-AP" ){
              Serial.print(i + 1);
              Serial.print(": ");
              Serial.print(ssid);
              Serial.print(" (RSSI:");
              double dis = pow(10,((abs(-rssi)-52)/(2*10)));
              Serial.print(rssi);
              Serial.print(": distance: ");
              Serial.print(dis);
              Serial.print("m");
              Serial.print(")");
              Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
              logSDCard();
            }
            
            delay(10);
        }
    }
    Serial.println("");

    // Wait a bit before scanning again
    readingID ++;
    delay(1000);
}

// Write the sensor readings on the SD card
void logSDCard() {
  dataMessage = String(readingID) + "," + ssid + "," + String(rssi) + "," + room_num + "\r\n";
  Serial.print("Save data: ");
  Serial.println(dataMessage);
  appendFile(SD, "/RSSI_data.txt", dataMessage.c_str());
}

// Write to the SD card (DON'T MODIFY THIS FUNCTION)
void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if(file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

// Append data to the SD card (DON'T MODIFY THIS FUNCTION)
void appendFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if(!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if(file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

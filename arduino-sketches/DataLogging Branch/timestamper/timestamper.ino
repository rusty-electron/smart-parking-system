#include <ESP8266WiFi.h>
#include <NTPtimeESP.h>
#include <SD.h>
#include <SPI.h>

const char* ssid     = "oppo";
const char* password = "mrparrot";
char BUFFER[15];

NTPtime NTPch("europe.pool.ntp.org");   // Choose server pool as required

/*
 * The structure contains following fields:
 * struct strDateTime
{
  byte hour;
  byte minute;
  byte second;
  int year;
  byte month;
  byte day;
  byte dayofWeek;
  boolean valid;
};
 */
 
strDateTime dateTime;
String dateString="";
String OutputString="";
File newFile;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Booted");
  Serial.println("Connecting to Wi-Fi");

  WiFi.mode(WIFI_STA);
  WiFi.begin (ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.println("WiFi connected");
  
  /*******************
   SD Card Check Code
    */
 
  // see if the card is present and can be initialized:
  if (!SD.begin(D2)) {
    Serial.println("Card failed, or not present");
  }else{
    Serial.println("SD Card Found");
  }
}
void loop() {
  dateTime = NTPch.getNTPtime(4.5, 1);

  if(dateTime.valid){
    
    byte actualHour = dateTime.hour;
    byte actualMinute = dateTime.minute;
    byte actualsecond = dateTime.second;
    int actualyear = dateTime.year;
    byte actualMonth = dateTime.month;
    byte actualday =dateTime.day;
    //byte actualdayofWeek = dateTime.dayofWeek;
    
    //dateString = String(actualday) + String(actualMonth) + String(actualyear) + ".csv";
    OutputString = String(actualHour) + ":" + String(actualMinute) + ":" + String(actualsecond) + "," + String(101);

    sprintf(BUFFER,"%02u%02u%04u.csv",actualday,actualMonth,actualyear);
    
    if (SD.exists(BUFFER)) {
      Serial.println("[Log] Datafile Found");
    }else{
      Serial.println("[Log] Creating new File");
    }
  
    newFile = SD.open(BUFFER, FILE_WRITE);
    if(newFile){
      Serial.println("[Log] Writing to file:");

      OutputString.toCharArray(BUFFER, sizeof(BUFFER));
      
      newFile.println(OutputString);
      Serial.println(OutputString);
      newFile.close();
    }
    Serial.println();
    delay(2000);
  }
}

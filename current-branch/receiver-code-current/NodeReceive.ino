#include <ESP8266WiFi.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

const char* ssid     = "oppo";
const char* password = "mrparrot";
const char* host = "electronweb.000webhostapp.com";

const char* foundStr = "PRESENCE";
const char* notStr = "ABSENCE";
char* flag;

RF24 radio(D2, D1); // CE, CSN

const byte address[6] = "00001";

void setup() {
  Serial.begin(9600);

  //Wifi Code
  delay(100);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password); 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Netmask: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("Gateway: ");
  Serial.println(WiFi.gatewayIP());
  
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    char text[32] = "";
    radio.read(&text, sizeof(text));

    //For sending data to server
    flag = strstr(text,foundStr);
    if(flag){
      
      WiFiClient client;
      const int httpPort = 80;
      if (!client.connect(host, httpPort)) {
        Serial.println("Connection failed!");
      }

      String url = "/api/update.php?id=1&dist=0&data=1";
      Serial.print("Requesting URL: ");
      Serial.println(url);
      
      client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                   "Host: " + host + "\r\n" + 
                   "Connection: close\r\n\r\n");
      delay(500);
    }

    flag = strstr(text,notStr);
    if(flag){
      
      WiFiClient client;
      const int httpPort = 80;
      if (!client.connect(host, httpPort)) {
        Serial.println("Connection failed!");
      }

      String url = "/api/update.php?id=1&dist=0&data=0";
      Serial.print("Requesting URL: ");
      Serial.println(url);
      
      client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                   "Host: " + host + "\r\n" + 
                   "Connection: close\r\n\r\n");
      delay(500);
    }
    
    Serial.println(text);
  }
}

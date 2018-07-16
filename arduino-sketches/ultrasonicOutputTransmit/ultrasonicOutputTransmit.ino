#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <NewPing.h>

RF24 radio(10, 8); // CE, CSN
const byte address[6] = "00001";
     
#define TRIGGER_PIN  7
#define ECHO_PIN     6
#define MAX_DISTANCE 350
#define SIZE 30

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
unsigned short dataSet[SIZE], dataIdx=0;
unsigned short mean,sum,currentValue;

void sonicOutput(){
      sum=0;
      if(dataIdx<SIZE){
        dataIdx++;
      }else{
        dataIdx=0;
      }
      currentValue = sonar.ping_cm();
      dataSet[dataIdx]=currentValue;

      for(int i=0;i<SIZE;i++){
        sum+=dataSet[i];
      }
      mean = sum/SIZE;
      delay(50);
}

void setup() { 
  dataSet[SIZE] = {0};
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_LOW);
  radio.stopListening();
}

void loop() {
  sonicOutput();
  char text[10];
  String str = String(mean) + " cm";
  str.toCharArray(text,10);
  radio.write(&text, sizeof(text));
  Serial.println(text);
  delay(50);
}

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h> 
#include <NewPing.h>

RF24 radio(10, 8); // CE, CSN
const byte address[6] = "00001";
     
#define TRIGGER_PIN  7
#define ECHO_PIN     6
#define MAX_DISTANCE 350
#define SIZE 20

#define STR_SIZE 30

#define addr 0x0D

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
unsigned short dataSet[SIZE], dataIdx=0;
unsigned short mean,sum,currentValue;

//**New Algorithm Inits**
long b_new=0, b_prev;
unsigned long runTime;

unsigned long distTimer;
unsigned int carPresent = 0;
long b_store =0;
unsigned int b_count = 0;
unsigned int startFlag = 0;

char text[60];
String str = "";

//**flags
int checkMode = 0;

#define fieldLimit 10
#define distLimit 20

void sonicOutput(){
      sum=0;
      if(dataIdx<SIZE){
        dataIdx++;
      }else{
        dataIdx=0;
      }
      currentValue = sonar.ping_cm();
      if(sonar.ping()==0)
        currentValue=350;
      dataSet[dataIdx]=currentValue;

      for(int i=0;i<SIZE;i++){
        sum+=dataSet[i];
      }
      mean = sum/SIZE;
      delay(50);
}

void detectionFunction(){
  int diff = b_new - b_prev;
  diff = abs(diff);
  if(diff > fieldLimit){
    Serial.println("Anomaly Detected in Local Field");
    Serial.print("B_NEW: ");
    Serial.print(b_new);
    Serial.print(", B_PREV: ");
    Serial.println(b_prev);
    
    //--------------------------------------------------
    str = "Anomaly -- B_NEW: "+String(b_new);
    str.toCharArray(text,STR_SIZE);
    radio.write(&text, sizeof(text));

    str = "B_PREV: "+String(b_prev);
    str.toCharArray(text,STR_SIZE);
    radio.write(&text, sizeof(text));
    //--------------------------------------------------

    Serial.print("Waiting for Distance Response");

    //--------------------------------------------------
    str = "Waiting for Distance Response";
    str.toCharArray(text,STR_SIZE);
    radio.write(&text, sizeof(text));
    //--------------------------------------------------
    
    checkMode = 1; 
    distTimer = millis();
  }
}

void setup() { 
  dataSet[SIZE] = {0};
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_LOW);
  radio.stopListening();

  Wire.begin();
  Wire.beginTransmission(addr); //start talking
  Wire.write(0x0B); // Tell the HMC5883 to Continuously Measure
  Wire.write(0x01); // Set the Register
  Wire.endTransmission();
  Wire.beginTransmission(addr); //start talking
  Wire.write(0x09); // Tell the HMC5883 to Continuously Measure
  Wire.write(0x1D); // Set the Register
  Wire.endTransmission();

  runTime = millis();
}

void loop() {
  long x, y, z;
  sonicOutput();

  Wire.beginTransmission(addr);
  Wire.write(0x00); //start with register 3.
  Wire.endTransmission();

  //Read the data.. 2 bytes for each axis.. 6 total bytes
  Wire.requestFrom(addr, 6);
  if (6 <= Wire.available()) {
    x = Wire.read(); //MSB  x
    x |= Wire.read() << 8; //LSB  x
    z = Wire.read(); //MSB  z
    z |= Wire.read() << 8; //LSB z
    y = Wire.read(); //MSB y
    y |= Wire.read() << 8; //LSB y
  }
  x/=10;
  y/=10;
  z/=10;
  
  b_new = sqrt(x*x + y*y + z*z); 
  
  if(runTime > 6000){
    Serial.println("C");
    //--------------------------------------------------
    str = "C";
    str.toCharArray(text,STR_SIZE);
    radio.write(&text, sizeof(text));
    //--------------------------------------------------
    if(startFlag==0){
      b_prev = b_store/b_count;
      Serial.print("B_PREV: ");
      Serial.println(b_prev);
      Serial.print("READINGS TAKEN: ");
      Serial.println(b_count);

      //--------------------------------------------------
      str = "B_PREV: "+String(b_prev)+", READINGS TAKEN: "+String(b_count);
      str.toCharArray(text,STR_SIZE);
      radio.write(&text, sizeof(text));
      //--------------------------------------------------
      
      startFlag=1;
    }
    detectionFunction();
  }else{
    if(runTime <= 6000 && runTime >= 2500){
       b_store+=b_new;
       b_count++;
    }
    runTime = millis();
  }

  if(checkMode == 1){
    if((millis()-distTimer)>6000){
    checkMode = 0;
    
    Serial.print("DISTANCE: ");
    Serial.println(mean);

    //--------------------------------------------------
    str = "DISTANCE: "+String(mean);
    str.toCharArray(text,STR_SIZE);
    radio.write(&text, sizeof(text));
    //--------------------------------------------------
    
    if(mean<distLimit){
      if(carPresent==0){
        Serial.println("**CAR PRESENCE DETECTED**");

        //--------------------------------------------------
        str = "**CAR PRESENCE DETECTED**";
        str.toCharArray(text,STR_SIZE);
        radio.write(&text, sizeof(text));
        //--------------------------------------------------
        
        carPresent=1;
        delay(2000);
       }else{
        Serial.println("**FALSE VARIATION**");

        //--------------------------------------------------
        str = "**FALSE VARIATION**";
        str.toCharArray(text,STR_SIZE);
        radio.write(&text, sizeof(text));
        //--------------------------------------------------
        
       }
     }else{
      if(carPresent==1){
        Serial.println("**CAR ABSENCE DETECTED**");

        //--------------------------------------------------
        str = "**CAR ABSENCE DETECTED**";
        str.toCharArray(text,STR_SIZE);
        radio.write(&text, sizeof(text));
        //--------------------------------------------------
        
        carPresent=0;
        delay(2000);
       }else{
        Serial.println("**FALSE VARIATION**");

        //--------------------------------------------------
        str = "**FALSE VARIATION**";
        str.toCharArray(text,STR_SIZE);
        radio.write(&text, sizeof(text));
        //--------------------------------------------------
        
       }
     }
   }
  }
  
  String str = String(mean) + ", " + String(b_new);
  str.toCharArray(text,STR_SIZE);
  radio.write(&text, sizeof(text));
  Serial.println(text);
  delay(100);
}

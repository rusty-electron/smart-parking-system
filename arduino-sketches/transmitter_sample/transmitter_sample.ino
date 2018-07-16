#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(10, 8); // CE, CSN

const byte address[6] = "00001";
void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_LOW);
  radio.stopListening();
}

void loop() {
  const char text[] = "How are you!";
  radio.write(&text, sizeof(text));
  Serial.println("Transmission Done!");
  delay(1000);
}

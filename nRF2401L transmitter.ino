#include <SPI.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001";

void setup() {
  Serial.begin(19200);
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_LOW);
}

void loop() {
  char textToSend[] = "Hello, receiver!"; // Message to send

  radio.write(&textToSend, sizeof(textToSend));

  Serial.println("Message sent: Hello, receiver!");

  delay(1000); // Delay between transmissions
}

#include <SPI.h>
#include <RF24.h>

RF24 radio(46, 47); // CE, CSN
const byte address[6] = "00001";

void setup() {
  Serial.begin(19200);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    char receivedText[32] = ""; // Initialize a character array to store received text

    radio.read(&receivedText, sizeof(receivedText));

    Serial.print("Received message: ");
    Serial.println(receivedText);
  }
}

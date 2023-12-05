#include <math.h>     // Provides mathematical functions
#include <SPI.h>      // Allows communication with devices that support the Serial Peripheral Interface (used by nRF24L01 module)
#include <nRF24L01.h> // These libraries are used for wireless communication using the NRF24L01 module
#include <RF24.h>     // These libraries are used for wireless communication using the NRF24L01 module
#include <Wire.h>     // For I2C communication with the LPS25HB pressure sensor.
#include <SparkFun_LPS25HB_Arduino_Library.h> // Click here to get the library: http://librarymanager/All#SparkFun_LPS25HB
#include <TCA9548A.h> // Library for TCA9548A multiplexer

LPS25HB pressureSensor; // Create an object of the LPS25HB class
TCA9548A multiplexer;   // Define the multiplexer address (A0-A2 pins connected to GND for default address 0x70)

const int size = 4;           // Adjust the size for the number of force sensors you will have
const byte muxAddress = 0x70; // I2C address of the TCA9548A multiplexer
RF24 radio(7,8);              // CE, CSN
const byte address[6] = "00001";
float Pressure[size];
float Base_Pressure = 60;      

unsigned long previous_time_send = 0;
unsigned long previous_time_print = 0;
const unsigned long send_interval = 50;
const unsigned long print_interval = 500;

void setup()
{
  Serial.begin(115200);             // Initializes the serial communication at a baud rate of 115200 bits per second for debugging and printing data to the serial monitor.
  radio.begin();                    // Sets up the communication module (RF24 Radio) and prepares it for operation
  radio.openWritingPipe(address);   // Sets the NRF24L01 to use the specified address for writing (transmitting) data.
  radio.setPALevel(RF24_PA_MIN);    // Sets the power amplifier level for the radio module to the minimum level. This helps to reduce power consumption and limit the range of the communication.
  radio.stopListening();            // Puts the NRF24L01 module in the transmitting mode (as opposed to the receiving mode)
  Serial.println("Pressure Sensor for Weston Robotics");
  Serial.println();

  // These lines of code are used to initialize the I2C communication protocol and set the clock speed for the I2C bus, respectively.
  Wire.begin();           // Initializes the I2C (Inter-Integrated Circuit) communication protocol
  Wire.setClock(100000);  // Sets the clock speed to 400 kHz, which is a common fast mode for I2C communication

  Wire.begin();

  // Initialize the TCA9548A multiplexer
  Wire.beginTransmission(muxAddress);
  Wire.write(0x01);       // Configure to enable all channels
  Wire.endTransmission();

  // Initialize the LPS25HB sensors 
  for (int i = 0; i < size; i++)
  {
    Wire.beginTransmission(muxAddress);
    Wire.write(1 << i);                 // Select the i-th channel
    Wire.endTransmission();

    pressureSensor.begin();             // Initialize the sensor on the selected channel
    delay(10);                          // Give some time for the sensor to initialize
  }
  
  // If the begin() function returns true, it indicates that the LPS25HB sensor has been successfully initialized and is ready to be used.
  pressureSensor.begin();               // Begin using default values when using the Qwiic system with the LPS25HB right out of the box
                                        // pressureSensor.begin(Wire2);                                // By passing in only one argument you specify which Wire port to use - here we specify the Wire2 port
                                        // pressureSensor.begin(Wire, LPS25HB_I2C_ADDR_ALT);           // In order to use the alternate address you *must* pass in both parameters like this
                                        // pressureSensor.begin(LPS25HB_I2C_ADDR_ALT);                 // This line would fail because begin() expects the first argument to be a Wire port, not an I2C address

  if (pressureSensor.isConnected() == false)                  // The library supports some different error codes such as "DISCONNECTED"
  {
    Serial.println("LPS25HB not found with your settings: "); // Alert the user that the device cannot be reached
    Serial.println("Troubleshooting: ");
    Serial.println("  Ensure the correct I2C address is used (in begin() function)");
    Serial.println("  Ensure the correct Wire port is used (in begin() function)");
    Serial.println("  Ensure the sensor is connected correctly");
    Serial.println("Reset the board or re-upload to try again.");
    Serial.println("");
    while (1);
  }
}

void loop()
{ 
  unsigned long current_time = millis();
  int data_size = sizeof(Pressure) / sizeof(Pressure[0]);   // Represents the number of Force Sensitive Resistor (fsr) sensors connected to the Arduino.
  
  for (int i = 0; i < data_size; i++)
  { 
    Wire.beginTransmission(muxAddress); 
    Wire.write(1 << i);
    Wire.endTransmission();
    
    Pressure[i] = pressureSensor.getPressure_hPa() / 10.0;  // Convert hPa to kPa
    Pressure[i] = Pressure[i] - Base_Pressure;              // This line subtracts the Base_Pressure from the measured pressure to get the difference in pressure from the base value. This is done to calibrate the pressure data if needed
    
    if (abs(Pressure[i]) < 1)                               // To resolve drift (the tendency of the force sensor to provide small non-zero values even when no force is applied)
    {
      Pressure[i] = 0;
    }

    Serial.print("LPS25HB #");                 // Prints out which sensor is measuring the pressure value 
    Serial.print(i);
    Serial.print("Pressure: ");                // Prints out the information related to the pressure values exerted by the fsr sensors
    Serial.println(Pressure[i]);
  }
  
  radio.write(&Pressure, sizeof(Pressure));    // The radio.write() function is used to send data over the NRF24L01 wireless communication module. It takes two arguments: the data to be sent (&Force, a pointer to the Force array), and the size of the data to be sent (sizeof(Force))
  delay(40);
  
    if (pressureSensor.isConnected() == false) // The library supports some different error codes such as "DISCONNECTED"
  {
    Serial.println("Sensor Disconnected");     // Alert the user that the device cannot be reached
    // while (1);
  }
}

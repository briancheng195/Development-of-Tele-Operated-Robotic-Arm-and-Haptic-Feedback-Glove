#include <Arduino.h>
#include <math.h>              // Provides mathematical functions
#include <SPI.h>               // Allows communication with devices that support the Serial Peripheral Interface (used by nRF24L01 module)
#include <nRF24L01.h>          // These libraries are used for wireless communication using the NRF24L01 module
#include <RF24.h>              // These libraries are used for wireless communication using the NRF24L01 module
#include <DFRobot_MPX5700.h>   // A library for interfacing with the DFRobot MPX5700 pressure sensor
#include <TCA9548A.h>          // Library for TCA9548A multiplexer
#define I2C_MUX_ADDRESS 0x70   // I2C address of the TCA9548A multiplexer

DFRobot_MPX5700 mpx570[4];     // Create an array of pressure sensor objects for four sensors

const unsigned long event_interval_printdata = 1000;
unsigned long previous_time_printdata = 0;
const int pressure_sampling_size = 5;

double PWM_Min = 0.0;
double PWM_Max = 255.0;
double Pressure_min = 0.0;
double Pressure_max = 10.0;     

const int size = 4;                // Number of Fingers/Air Sacs
int Relay[size] = {9};             // Relay pins
int PWM_Control[size] = {2,3,4,5}; // Adjust accordingly to size and pins, can use one to control both pump and vacuum eventually
int PWM[size] = {0};               // Initialize the PWM array
int Pump[size] = {22,24,26,28};    // Adjust accordingly to size and pins
double Air_Pressure[size] = {0};   // Initialize the air pressure array
double Gauge_Pressure[size] = {0}; // Initialize the gauge pressure array
double Atmos_Pressure = 96.48;     // According to Air Pressure Sensor (Roughly, will differ from sensor to sensor)
float Pressure[size];              // The array can store 1 floating-point value for pressure measurements. (the number of elements that the array can hold)

bool Transmission_Status;
RF24 radio(46, 47);                // CE, CSN
const byte address[6] = "00001";

void Transmission_PWM_Extrapolation()                           // It calls the function to receive data wirelessly. It maps the force values to appropriate PWM values for controlling the air pumps 
{
  if (radio.available())                                        // radio.available() is used to check if there is any data in that buffer waiting to be read. It returns a boolean value, which is true if there is data available, and false if there is no data/ radio.available() is used to check if there is any data in that buffer waiting to be read. It returns a boolean value, which is true if there is data available, and false if there is no data
  {
    Transmission_Status = 1;                                    // Indicate that data is available and has been received from the wireless communication channel.
    radio.read(&Pressure, sizeof(Pressure));                    // Reads data from the NRF24L01 module's buffer and stores it in the pressure array.
    int Pressure_Size = sizeof(Pressure) / sizeof(Pressure[0]); // Pressure_Size represents the number of elements in the pressure array (the number of fingers)
    
    for (int i = 0; i < Pressure_Size; i++)
    {
      if ((Pressure[i] < Pressure_min) || (Pressure[i] < 0.01)) // To resolve drift (the tendency of the force sensor to provide small non-zero values even when no force is applied)
      {
        PWM[i] = PWM_Min;   
      }
      else if (Pressure[i] > Pressure_max)
      {
        PWM[i] = PWM_Max;
      }
      else 
      {
        PWM[i] = map(Pressure[i], Pressure_min, Pressure_max, PWM_Min, PWM_Max); // Linear Interpolation
      }
    }
  }
}

void Air_Pump_Direct()                                  // This function essentially controls the air pumps based on the PWM values received from the wireless communication channel
{
  if (Transmission_Status == 1)
  {
    int Motor_Status[size];                             // Motor_Status is initially set to 0 
    int Pump_Size = sizeof(Pump) / sizeof(Pump[0]);

    for (int i = 0; i < Pump_Size; i++)
    {
      if (PWM[i] > 0.01)
      {
        digitalWrite(Relay[i], LOW);
        digitalWrite(Pump[i], HIGH);
        analogWrite(PWM_Control[i], PWM[i]);            // To set the PWM value for the pump control using the PWM_Control pin
        Motor_Status[i] = 1;                            // The pump is active 
      }
      else if (PWM[i] < 0.01)
      {
        digitalWrite(Relay[i], HIGH);
        digitalWrite(Pump[i], LOW);
        analogWrite(PWM_Control[i], PWM[i]);            // To set the PWM value for the pump control using the PWM_Control pin
        Motor_Status[i] = 2;                            // The pump is inactive
      }
    }

    unsigned long current_time_printdata = millis();    // Retrives the current time in milliseconds from running the Arduino program
    if (current_time_printdata - previous_time_printdata >= event_interval_printdata)
    {
      for (int j = 0; j < size; j++)                    // This loop iterates through each finger or air sac (based on the value of size).
      // It prints information related to the Pressure, PWM value, air pressure, gauge pressure, and motor status for each finger.
      {
        Serial.print("Finger ");
        Serial.print(j);
        Serial.print(" Pressure: ");
        Serial.print(Pressure[j]);
        Serial.println("kPa");
        Serial.print("PWM Motor: ");
        Serial.println(j);
        Serial.print("PWM: ");
        Serial.println(PWM[j]);
     
        if (Motor_Status[j] == 1)                       // If the pump is active 
        {
          Serial.println("Pumping Air");
          Serial.println("___________________");
        }
        else if (Motor_Status[j] == 2)                  // If the pump is inactive 
        {
          Serial.println("Releasing Air");
          Serial.println("___________________");
        }
      }
      delay(3000);
      previous_time_printdata = current_time_printdata; // The code saves the current time as the new "previous time" to be used in the next iteration or event occurrence.
    }
  }
}

void setup()  // Initializing the serial communication, setting pin modes for controlling the motors (air pumps), initializing the pressure sensors, and configuring the RF24 radio module for wireless communication
{
  Serial.begin(115200); // This line initialises the serial communication at a baud rate of 115200 bits per second. It allows you to communicate with the Arduino board over the USB serial port.
  
  for (int i = 0; i < size; i++)
  {
    pinMode(PWM_Control[i], OUTPUT);  // This pin is used to control the speed of the motor using PWM
    pinMode(Pump[i], OUTPUT);         // This pin is used to turn off motor (air pump) ON and OFF
    pinMode(Relay[i], OUTPUT);        // A relay is often used to control the direction of airflow in the pneumatic system. By setting this pin to LOW, the air pump might be set to pump air into the system, while setting it to HIGH could release air from the system.
  }
  radio.begin();                      // Sets up the communication module (RF24 Radio) and prepares it for operation
  radio.openReadingPipe(0, address);  // Configures the radio module to listen on a specific address for incoming data
  radio.setPALevel(RF24_PA_MIN);      // Sets the power amplifier level for the radio module to the minimum level. This helps to reduce power consumption and limit the range of the communication.
  radio.startListening();             // Puts the radio module into listening mode, allowing it to receive data from the RF24 radio (receiving mode)
}

void loop()
{
  Transmission_PWM_Extrapolation();   // It calls the function to receive data wirelessly. It maps the pressure values to appropriate PWM values for controlling the air pumps 
  Air_Pump_Direct();                  // This function essentially controls the air pumps based on the PWM values received from the wireless communication channel
}

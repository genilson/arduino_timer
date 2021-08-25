/*

   Timer/Counter on Atmega328p (Arduino UNO)

   This sketch is for the master arduinos of the I2C bus,
   which will be accessed by the masters attached to the
   hosts's USBs

   When the host attached to the USB sends a command asking
   for the counter, this master arduino requests the counter
   value to the slave and returns it to the host.

   Author: Genilson Israel da Silva  Date: November, 2020

*/

// Include Arduino Wire library for I2C
#include <Wire.h>

// Define Slave I2C Address
#define SLAVE_ADDR 9

// Define Slave answer size
#define ANSWERSIZE 4

// Define clock for I2C bus (default is 100 KHz)
#define CLOCK 400000L

// Baud rate for I2C communication
#define B_RATE 115200

// A union allows to access each byte of counter on
// an array position
union Buffer
{
  unsigned long counter;
  byte counterBytes[4];
};

int req = 0;

Buffer buffer;

void setup() {

  // Initialize I2C communications as Master
  Wire.begin();

  //Setting the clock for the I2C bus
  Wire.setClock(CLOCK);

  // Setup serial monitor bauld rate
  Serial.begin(B_RATE);
}

void loop() {
  //If an 'r' is received, gets counter value and returns it.
  if (Serial.available() > 0) {
    req = Serial.read();
    if (req == 'r') {
      // Read response from Slave
      // Read 4 bytes of unsigned long
      if (Wire.requestFrom(SLAVE_ADDR, ANSWERSIZE) != ANSWERSIZE){
        Serial.println("Failed to retrieve data");
      }

      while (Wire.available() > 0) {
        buffer.counterBytes[0] = Wire.read();
        buffer.counterBytes[1] = Wire.read();
        buffer.counterBytes[2] = Wire.read();
        buffer.counterBytes[3] = Wire.read();
      }

      // Print to Serial Monitor
      Serial.println(buffer.counter);
    } else {
      if (req != '\n') {
        Serial.println("Unrecognized command received");
      }
    }
  }
}

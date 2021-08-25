/*

   Timer/Counter on Atmega328p (Arduino UNO)

   This sketch is for the slave arduino of the I2C bus,
   which will be accessed by the masters attached to the
   hosts's USBs

   The counter uses CTC (Clear Time on Compare) interruptions
   and sends the value of the counter to client arduinos via
   I2C when requested

   Author: Genilson Israel da Silva  Date: November, 2020

*/

#include <Wire.h>
#include <Wire.h>

// Define Slave I2C Address
#define SLAVE_ADDR 9

// Define Slave answer size
#define ANSWER_SIZE 4

// Baud rate for I2C communication
#define B_RATE 115200

const uint16_t T1_init = 0;

// Interrupt every 20 microseconds, or 0.00002s
// (time*crystal/prescaler) - 1, using prescaler 64
const uint16_t T1_comp = 4;

volatile unsigned long microseconds = 0;

union Buffer
{
  unsigned long counter;
  byte counterBytes[4];
};

Buffer buffer;

// --- Interruption Service Routine ---
ISR(TIMER1_COMPA_vect)
{

  TCNT1 = T1_init;      //restart TIMER1
  ++microseconds;

} //end ISR


void setup()
{

  noInterrupts();
  microseconds = 0;
  TCCR1A = 0; // set entire TCCR1A register to 0
  TCCR1B = 0; // same for TCCR1B

  //Initializing registers
  TCNT1 = T1_init;
  OCR1A = T1_comp;

  // turn on CTC mode
  TCCR1B |= (1 << WGM12);

  //Prescaler 1:64
  //CS12 CS11  CS10
  // 0    1     1
  TCCR1B &= ~(1 << CS12);
  TCCR1B |=  (1 << CS11);
  TCCR1B |=  (1 << CS10);


  //Enable Interrupt on Timer1
  TIMSK1 = (1 << OCIE1A);
  interrupts();

  // Initialize I2C communications as Slave
  Wire.begin(SLAVE_ADDR);

  // Function to run when data requested from master
  Wire.onRequest(requestEvent);

  // Setup Serial Monitor
  Serial.begin(B_RATE);

} //end setup

void requestEvent() {
  // Counts to the maximum value of 4.294.967.295
  // which multiplied by 20 corresponds to 1,431.65
  // minutes, or 23.86 hours, when the counter will
  // roolover

  buffer.counter = microseconds;
  // Send response back to Master
  Wire.write(buffer.counterBytes, ANSWER_SIZE);
  Serial.println(buffer.counter);
}


void loop()
{

} //end loop

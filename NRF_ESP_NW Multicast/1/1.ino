/*
  Arduino Wireless Network - Multiple NRF24L01 Tutorial
  == Example 01 - Servo Control / Node 01 - Servo motor ==
*/
#include <RF24.h>
#include <RF24Network.h>
#include <SPI.h>
//#include <Servo.h>
RF24 radio(2,15);               // nRF24L01 (CE,CSN)
RF24Network network(radio);      // Include the radio in the network
int x=16;
const uint16_t this_node = 00;  
const uint16_t node02 = 01;// Address of our node in Octal format ( 04,031, etc)
//Servo myservo;  // create servo object to control a servo
void setup() 
{
  pinMode(x,OUTPUT);
  Serial.begin(115200);
  SPI.begin();
  radio.begin();
  network.begin(90, this_node); //(channel, node address)
  //myservo.attach(3);   // (servo pin)
}
void loop() 
{
  unsigned long ditoo = 100;
   uint8_t   level=01;
  network.update();
   unsigned long incomingData;
   while ( network.available() ) 
    {
    RF24NetworkHeader header;
    network.peek(header);
    network.read(header, &incomingData, sizeof(incomingData));
    switch(header.type)
    {
      case 'A':
      Serial.println("One");
      break;
      case 'B':
      Serial.println("Two");
      break;
      case 'C':
      Serial.println("Three");
      break;
      case 'D':
      Serial.println("Four");
      break;
    }
    }
    RF24NetworkHeader  header1;
    network.multicast(header1,&ditoo,sizeof(ditoo),level);
}

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
const uint16_t this_node = 01;  
const uint16_t node02 = 00;// Address of our node in Octal format ( 04,031, etc)
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
  network.update();
  //network.txTimeout = 500;//must be different for different nodes, and for leaf nodes only that only tx n not rx
   unsigned long incomingData;
   unsigned long ditoo = 101;
   while ( network.available() ) 
    {
      RF24NetworkHeader header;
      network.peek(header);
      network.read(header, &incomingData, sizeof(incomingData));
      if(incomingData == 100)
      {
          digitalWrite(x,HIGH);
          delay(100);
          digitalWrite(x,LOW);
          delay(100);
      }
    }
  RF24NetworkHeader header1(node02,'A'); // (Address where the data is going)
  bool ok = network.write(header1, &ditoo, sizeof(ditoo)); // Send the data
  delay(100);
  
}

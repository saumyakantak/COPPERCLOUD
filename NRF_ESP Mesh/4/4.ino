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
const uint16_t this_node = 03;  
const uint16_t node02 = 04;// Address of our node in Octal format ( 04,031, etc)
//Servo myservo;  // create servo object to control a servo
void setup() {
  pinMode(x,OUTPUT);
  Serial.begin(115200);
  SPI.begin();
  radio.begin();
  network.begin(90, this_node); //(channel, node address)
  //myservo.attach(3);   // (servo pin)
}
void loop() {
  network.update();
   unsigned long incomingData;
   unsigned long ditoo = 104;
  while ( network.available() ) {     // Is there any incoming data?
    RF24NetworkHeader header;
    network.read(header, &incomingData, sizeof(incomingData));// Read the incoming data
    Serial.println(incomingData);
    //myservo.write(incomingData);  // tell servo to go to a particular angle
  }
  if(incomingData==103)
  {
    digitalWrite(x,HIGH);
    delay(100);
    digitalWrite(x,LOW);
  }
  RF24NetworkHeader header(node02); // (Address where the data is going)
  bool ok = network.write(header, &ditoo, sizeof(ditoo)); // Send the data
  delay(100);
}

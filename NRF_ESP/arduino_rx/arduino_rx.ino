    /*
      Arduino Wireless Network - Multiple NRF24L01 Tutorial
      == Example 01 - Servo Control / Node 01 - Servo motor ==
    */
    #include <RF24.h>
    #include <RF24Network.h>
    #include <SPI.h>
   // #include <Servo.h>
    RF24 radio(2, 15);               // nRF24L01 (CE,CSN)
    RF24Network network(radio);      // Include the radio in the network
    const uint16_t this_node = 01;   // Address of our node in Octal format ( 04,031, etc)
    //Servo myservo;  // create servo object to control a servo
    void setup() {
      SPI.begin();
      Serial.begin(9600);
      radio.begin();
      network.begin(90, this_node); //(channel, node address)
     // myservo.attach(3);   // (servo pin)
    }
    void loop() {
      network.update();
      while ( network.available() ) {     // Is there any incoming data?
        RF24NetworkHeader header;
        String incomingData;
        network.read(header, &incomingData, sizeof(incomingData)); // Read the incoming data
        Serial.println(incomingData);  // tell servo to go to a particular angle
      }
    }

 #include <ESP8266WiFi.h>
 #include<StubCopperCube.h>
 #include<EEPROM.h>
#include <ESP8266Ping.h>
#include <SPI.h> // Only needed for Arduino 1.6.5 and earlier
//#include <SSD1306Spi.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

/* #### Update code version for every committed change #### */
const char* codeName    = "internet speed check";
const char* codeVersion = "0.1";

// This is standard code needed to make this code re-usable across multiple tbb(s)
boolean debug = true; // false for PROD
CopperCube cube(codeName, codeVersion, debug);

const char* remote_host = "www.google.com";

long lastMsg = 0;
char msg[50];
int value = 0;

String avg_time_ms;
long i;

void setupCube() {
  cube.initDevice(true);                      // true = connectivity required
  cube.setMqttCallback(receiveMessage);  // receiveMessage() is called when there is an incoming mqtt packet
  cube.connect();                             // establish wifi and mqtt connections
}

void setup() {
  //pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setupCube();
  executeDeviceSetup(); // device-specific setup outsourced to this method
}

void loop() {

  cube.loop(); delay(100);
  executeDeviceLoop();  // device-specific loop code outsourced to this method
}

// callback method for incoming mqtt packets. this method is only called by library of the packett is addressed to this device
void receiveMessage(char* topic, const JsonDocument& payload) {
  // print incoming message:
  serializeJson(payload, Serial);

  //read payload items from JsonDocument object:
  const char* cmd = payload["command"];
  const char* mt  = payload["msgtype"];
  // check if a particular key is present in the payload:
  if(cmd == NULL) {Serial.println("No command attribute");}
  else{
    Serial.println(cmd);
    // switch relay on
  }
}

void executeDeviceSetup() {
  // some setup here, example: pin mode etc
  // optional, if any extra topics (in addition to command) are required
  cube.subscribeToTopic("anyothertopic"); 
  delay(1000);
}

void executeDeviceLoop() {
// some device-specific loop code here
  Serial.print("Pinging host ");  // Just for troubleshot purpose.
     Serial.println(remote_host);
    if(Ping.ping(remote_host)) 
    {
    Ping.ping(remote_host, 10);  //10 time ping to google, You can change value to higher or lower
    i= Ping.averageTime();
    avg_time_ms = Ping.averageTime(); // reading string and Int for easy display integration.
    Serial.println(i);
    }
    //snprintf (msg, 50, "hello world #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(i);
    String yy=String(i);
    cube.sendData("PingTime",i , "cube/data");      // default topic: cube/data

  if (i < 99)  // It is in general term, Please change as per your requirement
 {
    Serial.println("Pingtime :");
  Serial.println(avg_time_ms + " ms");
  Serial.println("Internet condition");
 Serial.println("Good");
 }
 else if (i > 100 && i < 199)  // It is in general term, Please change as per your requirement
 {
    Serial.println("Pingtime :");
  Serial.println(avg_time_ms + " ms");
  Serial.println("Internet condition");
 Serial.println("Bad");
 } 
    
  else 
  {
  Serial.println("Error :(");
  Serial.println("Internet condition");
  Serial.println("Offline");
  }
  
  //display.display();
  delay(1000);  // Every minutes
  //display.clear();
}

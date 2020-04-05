/*
 Basic ESP8266 MQTT example

 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.

 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.

 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"

*/
 #include <ESP8266WiFi.h>
#include <ESP8266Ping.h>
#include <SPI.h> // Only needed for Arduino 1.6.5 and earlier
//#include <SSD1306Spi.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.

const char* ssid     = "iot-coppercloud";
const char* password = "coppercloud";
const char* mqtt_server = "3.214.158.175";

const char* remote_host = "www.google.com";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

String avg_time_ms;
int i;


void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
     // client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("20209999/cube/data");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  //pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  Serial.print("Pinging host ");  // Just for troubleshot purpose.
     Serial.println(remote_host);
    if(Ping.ping(remote_host)) 
    {
    Ping.ping(remote_host, 10);  //10 time ping to google, You can change value to higher or lower
    i= Ping.averageTime();
    avg_time_ms = Ping.averageTime(); // reading string and Int for easy display integration.
    Serial.println(i);

    if (!client.connected()) {
    reconnect();
  }
  client.loop();

    //snprintf (msg, 50, "hello world #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(i);
    String yy=String(i);
    client.publish("20209999/cube/data", yy.c_str());

  if (i < 99)  // It is in general term, Please change as per your requirement
 {
    Serial.println("Pingtime :");
  Serial.println(avg_time_ms + " ms");
  Serial.println("Internet condition");
 Serial.println("Good");
 }
  if (i > 100 && i < 199)  // It is in general term, Please change as per your requirement
 {
    Serial.println("Pingtime :");
  Serial.println(avg_time_ms + " ms");
  Serial.println("Internet condition");
 Serial.println("Bad");
 }
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

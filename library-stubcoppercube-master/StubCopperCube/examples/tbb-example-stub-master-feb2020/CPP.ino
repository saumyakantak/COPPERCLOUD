/*
 * COPYRIGHT 2019 © COPPERCLOUD IOTECH PVT LTD. ALL RIGHTS RESERVED
 *
 * WARNING! Commercial Software, All Use Must Be Licensed 
 * This software is protected by Copyright and Intellectual Property Rigths Law and International 
 * Treaties. Unauthorized use, duplication, reverse engineering, any form of redistribution, or use -
 * in part or in whole - other than by prior, express, printed and signed license for use 
 * provided by CopperCloud IOTech Pvt Ltd, is subject
 * to civil and criminal prosecution. 
 * If you have received this file in error, please notify copyright holder (iot@coppercloud.in) 
 * and destroy this and any other copies as instructed.
 */

#include "StubCopperCube.h"

/* ######## Update lib version for every committed change ########## */
const char libCode[]    = "StubCopperCube";
const char libVersion[] = "0.1";
/* ################################################################# */

char devCode[30]; char devVersion[10];

DeploymentConfig deployment;
ConnectionParams opConnection;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

String msgPrefix = "";

void (*fpAction)(char* topic, const JsonDocument& payload) = 0;

CopperCube::CopperCube(const char* code, const char* version, boolean debug) {
    _debug = debug;
    strcpy(devCode, code); strcpy(devVersion, version);
    sprintf(deployment.deviceID,"%ld", getChipID());
    printCubeConfig();
}

void CopperCube::printCubeConfig() {
    if(_debug) {
        Serial.begin(115200); 
        Serial.println(""); Serial.println("Serial Debugging enabled");
        Serial.println("Firmware:");
        Serial.println("________________________________________");
        Serial.println("      Code      |     Library    "); 
        Serial.println("----------------------------------------");
        Serial.print(devCode); Serial.print(" v"); Serial.print(devVersion);Serial.print(" | ");
        Serial.print(libCode); Serial.print(" v"); Serial.println(libVersion);
        Serial.println("--------------------------------------------");

        Serial.println("");
        Serial.println("Deployment config:");
        Serial.println("_____________________");
        Serial.println(" Depl ID | Device ID ");
        Serial.println("---------------------");
        Serial.print(deployment.deplID);    Serial.print(" | ");
        Serial.println(deployment.deviceID);
        Serial.println("---------------------");

        Serial.println("");
        Serial.println("Operational connection params:");
        Serial.println("_________________________________________________________");
        Serial.println("  Wifi SSID  | Wifi Psswd | Mqtt Server | Mqtt Poprt ");
        Serial.println("---------------------------------------------------------");
        Serial.print(opConnection.wifiSSID);   Serial.print(" | ");
        Serial.print(opConnection.wifiPsswd);  Serial.print(" | ");
        Serial.print(opConnection.mqttServer); Serial.print(" | ");
        Serial.println(opConnection.mqttPort);
        Serial.println("---------------------------------------------------------");
    }
}

// todo - find better way to return as c_str
const char* CopperCube::getClientID() {return String(getChipID()).c_str();}
uint32_t    CopperCube::getDeviceID() {return getChipID();}
uint32_t    CopperCube::getChipID() {return ESP.getChipId();}
char*       CopperCube::getDeploymentID() {return deployment.deplID;}
String      CopperCube::getMacAddress() {return WiFi.macAddress();}
String      CopperCube::getMessagePrefix() {return msgPrefix;}
void        CopperCube::setMqttCallback(CUBE_CALLBACK_SIGNATURE) {fpAction = callback;}
long        CopperCube::getRSSI() {return WiFi.RSSI();}

void CopperCube::setWiFiCreds(const char* ssid, const char* psswd) {
    strcpy(opConnection.wifiSSID, ssid);
    strcpy(opConnection.wifiPsswd, psswd);
}

void CopperCube::subscribeToTopic(const char* topic) {
    String fullTopic = ""; // todo - is this extra step necessary
    fullTopic = fullTopic + getDeploymentID() + "/" + topic;
    mqttClient.subscribe(fullTopic.c_str());
}

void CopperCube::publishOnTopic(const char* topic, const char* message, boolean retain=false) {
    String fullTopic = "";
    fullTopic = fullTopic + getDeploymentID() + "/" + topic;
    mqttClient.publish(fullTopic.c_str(), message, retain);
    if(_debug) {
        Serial.print("  "); Serial.print("Published message: "); Serial.println(message);
        Serial.print("  "); Serial.print("To Topic: "); Serial.println(fullTopic);
    }
}

void CopperCube::publishOnTopic(const char* topic, String message, boolean retain=false) {
    publishOnTopic(topic, message.c_str(), retain); // invoke base version of method
}

// todo - test wifi and mqtt reconnection. also, does pi reconnect after losing n/w?
void CopperCube::loop() {
    if(!mqttClient.connected()) {
        if(_debug) {
            Serial.println(); Serial.print("  MQTT Disconnected. Reconnecting ...");
        }
        mqttConnection(); // reconnect if mqtt connection lost
    }
    mqttClient.loop();
    delay(1);
}
    
void CopperCube::initDevice(boolean connectivity=true) {
    _needConnectivity = connectivity; // default t o yes - need wifi & mqtt
    setStandardMessages();
}

void CopperCube::setStandardMessages() {
    // todo - replace String with c_str
    msgPrefix = String("\"deplid\":\"") + getDeploymentID() +               
                String("\",\"deviceid\":\"") + getDeviceID() + "\",";
    
    // lwt message template
    String msg = "";
    msg = "{" + msgPrefix + String("\"msgtype\":\"lwt\"}");
    msg.toCharArray(_msgLwt, 256);
}

void CopperCube::connect() {
    if(_needConnectivity) {wifiConnection(); mqttConnection();}
}

void CopperCube::wifiConnection() {
    boolean result = false;
    if(_debug) {
        Serial.print("Connecting to Wifi with these creds: ");
        Serial.print(opConnection.wifiSSID); Serial.print(" | "); Serial.println(opConnection.wifiPsswd);
    }
    
    // attempt connection with operational/onsite wifi network
    result = connectToWifi(opConnection.wifiSSID, opConnection.wifiPsswd);
    if(result) {
        if(_debug) {Serial.print("  "); Serial.println("Connected to OP WIFI NETWORK");}
    }

    // by this point, the cube is connected, one way or another
    if(result) {
        if(_debug) {
            Serial.print("  "); Serial.println("WiFi network acquired:");
            Serial.print(opConnection.wifiSSID); Serial.print(" | "); Serial.println(opConnection.wifiPsswd);
        }
    }
}


boolean CopperCube::connectToWifi(char* ssid, char* psswd) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, psswd);
    uint8_t connAttempts = 0;
    while ((WiFi.status() != WL_CONNECTED) && (connAttempts < 20)) {
        connAttempts++;
        if(_debug) {Serial.print(".");}
        delay(500);
    }
    if(_debug) {
        Serial.print("  "); Serial.print("WiFi.status()) = "); Serial.println(WiFi.status());
    }

    if(WiFi.status() != WL_CONNECTED) {
        if(_debug) {Serial.print("  "); Serial.print("Could not connect to Wifi: "); Serial.println(ssid);}
        return false;
    }
    else {
        if(_debug) {Serial.print("  "); Serial.print("Connected to Wifi: "); Serial.println(ssid);}
        return true;
    }
}


void CopperCube::mqttConnection() {
    if(mqttClient.connected()) {mqttClient.disconnect();}
    mqttClient.setServer(opConnection.mqttServer, opConnection.mqttPort);
    mqttClient.setCallback([this] (char* topic, byte* payload, unsigned int length) {this->mqttCallback(topic, payload, length);});
  
    // Loop until we're reconnected
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqttClient.connect(getClientID())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      sendBirthMessage();      // ... and resubscribe
      //mqttClient.subscribe(_topicCmdChannel);
      subscribeToTopic(_topicCmdChannel);
    } 
    else {
        Serial.print("failed, rc=");
        Serial.print(mqttClient.state());
        Serial.println(" try again in 5 seconds");
        delay(5000);
    }
  }
}

/* function to return start address in eeprom where client code can start storing devive attributes. all addresses before this address are RESERVED for core library code and attributes */
int32_t CopperCube::getDeviceDataAddress() {return _deviceDataAddr;}

void CopperCube::mqttCallback(char* topic, byte* payload, unsigned int length){
    char callbackPayload[256] = "";
    for (int i = 0; i < length; i++) {
        //Serial.print((char)payload[i]); 
        callbackPayload[i] = (char)payload[i];
    }
    if(_debug) {
        Serial.print("  ");
        Serial.print("Message arrived on topic [");
        Serial.print(topic); Serial.print("]: ");
        Serial.println(callbackPayload);
    }
        
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, callbackPayload);
    if (error){
        const char* error = "Json parsing failed. Probably bad json string.";
        if(_debug) {Serial.println(); Serial.println(error);}
        sendException(_badEnvelope, error);
        return;
    }
    else{
        if(_debug) {serializeJsonPretty(doc, Serial); Serial.println("");}
        if( 0 != fpAction ) {
            if(_debug) {Serial.print("  "); Serial.println("Invoking callback function in client program.");}
            (*fpAction)(topic, doc);
        }
    }
}

void CopperCube::sendBirthMessage(){
    StaticJsonDocument<256> doc;
    doc["msgtype"]  = "birth";
    doc["code"]     = String(devCode) + " v" + String(devVersion);
    doc["lib"]      = String(libCode) + " v" + String(libVersion);
    doc["rssi"]     = getRSSI();
    sendGenericMessage(doc, _topicBirth);
}

void CopperCube::sendEvent(const char* eventType, const char* event, const char* topic){
    StaticJsonDocument<256> doc;
    doc["msgtype"]   = "event";
    doc["eventtype"] = eventType;
    doc["event"]     = event; 
    sendGenericMessage(doc, topic);
}

void CopperCube::sendData(const char* dataType, double data, const char* topic){
    StaticJsonDocument<256> doc;
    doc["msgtype"]  = "data";
    doc["datatype"] = dataType;
    doc["data"]     = data; 
    sendGenericMessage(doc, topic);
}

void CopperCube::sendData(const char* dataType, long data, const char* topic){
    StaticJsonDocument<256> doc;
    doc["msgtype"]  = "data";
    doc["datatype"] = dataType;
    doc["data"]     = data; 
    sendGenericMessage(doc, topic);
}


void CopperCube::sendState(const char* stateType, const char* state, const char* topic){
    StaticJsonDocument<256> doc;
    doc["msgtype"]   = "state";
    doc["statetype"] = stateType;
    doc["state"]     = state; 
    sendGenericMessage(doc, topic);
}


void CopperCube::sendException(const char* exceptionType, const char* exception, const char* topic){
    StaticJsonDocument<256> doc;
    doc["msgtype"]   = "exception";
    doc["exptype"]   = exceptionType;
    doc["exception"] = exception;
    
    sendGenericMessage(doc, topic);
}

void CopperCube::sendDisplay(const char* dataType, double data, const char* topic){
    StaticJsonDocument<256> doc;
    doc["msgtype"]   = "display";
    doc["datatype"]  = dataType;
    doc["data"]      = data;
    
    sendGenericMessage(doc, topic);
}

void CopperCube::sendGenericMessage(JsonDocument& doc, const char* topic) {

    doc["deplid"]    = getDeploymentID();
    doc["deviceid"]  = getDeviceID();
   
    char output[256];
    serializeJson(doc, output);
    publishOnTopic(topic, String(output), false);
}

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
 * and destroy this and 
 * any other copies as instructed.
 */


#ifndef coppercube_h
#define coppercube_h

#include "Arduino.h"
#include <ArduinoJson.h>

// includes brought over from the .cpp
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>


#define CUBE_CALLBACK_SIGNATURE void (*callback)(char* topic, const JsonDocument& payload)  //(char* topic, byte* payload, unsigned int length) // todo - add signature and return value

class CopperCube
{
  public:
    CopperCube(const char* code, const char* version, boolean debug=false);  // debug, ronin flags
    void printCubeConfig();
    void initDevice(boolean connectivity); // wether connectivity is needed
    void connect();
    void loop();

    void sendEvent(const char* eventType, const char* event, const char* topic="cube/event");
    void sendData(const char* dataType, double data, const char* topic="cube/data");
    void sendData(const char* dataType, long data, const char* topic="cube/data");
    void sendState(const char* stateType, const char* state, const char* topic="cube/state");
    void sendException(const char* exceptionType, const char* exception, const char* topic="cube/exception");
    void sendGenericMessage(JsonDocument& doc, const char* topic);
    void sendDisplay(const char* dataType, double data, const char* topic="cube/display");


    const char* getClientID();
    int32_t getDeviceDataAddress();
    long getRSSI();

    // pubsub & callback methods
    void setMqttCallback(CUBE_CALLBACK_SIGNATURE);
    void subscribeToTopic(const char* topic);
    void publishOnTopic(const char* topic, const char* message, boolean retain);
    void publishOnTopic(const char* topic, String message, boolean retain);
    void setWiFiCreds(const char* ssid, const char* psswd);

  private:
    const char* _badEnvelope = "bad envelope";
  
    // get addressing attribs
    uint32_t getChipID();
    uint32_t getDeviceID();
    char* getDeploymentID();
    String   getMacAddress();

    void wifiConnection(); //todo - remove
    boolean connectToWifi(char* ssid, char* psswd);
    void mqttConnection();
    String getMessagePrefix();
    void setStandardMessages(); // todo - is this required
    void sendBirthMessage();
    void mqttCallback(char* topic, byte* payload, unsigned int length);
    boolean _debug  = false;
    boolean _needConnectivity = true;
    
    void setWiFiCreds();

    const char* _topicCmdChannel    = "cmd/command";
    const char* _topicBirth         = "cube/birth";
    const char* _topicLwt           = "cube/lwt";
    const char* _topicException     = "cube/exception";
    // todo - are these required?
    char _msgLwt[256];
    uint8_t _deplDataAddr = 00;           // store 90 bytes of deployment ID
    uint8_t _connParamsAddr = 100;  // store 90 byes of op connection params
    uint32 _deviceDataAddr = 300;     // start address of device specific attributes
};

struct DeploymentConfig {
    // bharat - rec: industry name, multiple groups
    char deplID[10] = "20200000";
    char deviceID[10];
};

struct ConnectionParams {
    char wifiSSID[30]   = "iot-coppercloud";
    char wifiPsswd[30]  = "coppercloud";
    char mqttServer[30] = "3.214.158.175";
    uint16_t mqttPort    = 1883;
};

#endif

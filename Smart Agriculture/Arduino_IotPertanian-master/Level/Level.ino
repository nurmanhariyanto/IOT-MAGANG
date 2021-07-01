
/*
 * Edit By Asep Trisna Setiawan
 * Bandung 15 03 2020
 */


#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <EasyNTPClient.h>
#include <WiFiUdp.h>
#include <FS.h> //this needs to be first, or it all crashes and burns...
#include <Ticker.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include "config.h"
//WiFiClient espClient;
//PubSubClient client(espClient);
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
int FloatSensor  = D1;
int FloatSensor2 = D2;
int FloatSensor3 = D3;
int Buzzer       = D4;
int buttonState  = 1;
int buttonState2 = 1;
int buttonState3 = 1;
const int LED = 13;
char routingkey2[40] ="waterlev";
byte   mac[6];
char   msg[50];
String Level;
void setup_wifi() {
  WiFi.macAddress(MAC_array);
  for (int i = 0; i < sizeof(MAC_array) - 1; ++i) {
    sprintf(MAC_char, "%s%02x:", MAC_char, MAC_array[i]);
  }
  sprintf(MAC_char, "%s%02x", MAC_char, MAC_array[sizeof(MAC_array) - 1]);
  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
  String(mqtt_port).toCharArray(smqtt_port, 5);
  WiFiManagerParameter custom_mqtt_port("port", "mqtt port", smqtt_port, 5);
  WiFiManagerParameter custom_mqtt_user("user", "mqtt user", mqtt_user, 40);
  WiFiManagerParameter custom_mqtt_password("password", "mqtt password", mqtt_password, 40);
  WiFiManagerParameter custom_mqtt_keywords1("keyword1", "mqtt keyword1", MAC_char, 40);
  //WiFiManagerParameter custom_mqtt_keywords2("keyword2", "mqtt keyword2", mqtt_keywords2, 40);
  WiFiManager wifiManager;
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  wifiManager.addParameter( & custom_mqtt_server);
  wifiManager.addParameter( & custom_mqtt_port);
  wifiManager.addParameter( & custom_mqtt_user);
  wifiManager.addParameter( & custom_mqtt_password);
  wifiManager.addParameter(&custom_mqtt_keywords1);
  if (!wifiManager.autoConnect(MAC_char, "password")) {
    Serial.println("failed to connect and hit timeout");
    delay(2000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(2000);
  }
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
  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(LED, HIGH);   // Turn the LED on (Note that HIGH is the voltage level
  } else {
    digitalWrite(LED, LOW);  // Turn the LED off by making the voltage LOW
  }
}
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...%s");
    Serial.println(mqtt_server);
    if (client.connect(MAC_char, mqtt_user, mqtt_password)) {
      Serial.println("connected");
      Serial.println(MAC_char);
      client.subscribe(MAC_char);
     
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      if (client.state() == 4) ESP.restart();
      else {
        Serial.println(" try again in 5 seconds");
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  pinMode(FloatSensor,  INPUT_PULLUP);
  pinMode(FloatSensor2, INPUT_PULLUP);
  pinMode(FloatSensor3, INPUT_PULLUP);
  pinMode(Buzzer,   OUTPUT);
  digitalWrite(Buzzer,HIGH);
  delay(100);
  }
void loop(){
  if (!client.connected()) {
    reconnect();
  }
  String pubmsg = "";
  float speed;
  char time[32];
  char date[32];
  
  client.loop();
    // read the value from the sensor
  int value = digitalRead (buttonState);     
  int value2 = digitalRead (buttonState2); 
  int value3 = digitalRead (buttonState3); 
  Serial.println(value);
  delay(5000);
   buttonState  = digitalRead(FloatSensor);  // read the value of float sensor
   buttonState2 = digitalRead(FloatSensor2);  // read the value of float sensor
   buttonState3 = digitalRead(FloatSensor3);  // read the value of float sensor

  if ((buttonState == HIGH)&&(buttonState2 == HIGH)&&(buttonState3 == HIGH)){
    Level = "Empty";                                         
    Serial.println( "WATER LEVEL - LOW");
    digitalWrite(Buzzer, HIGH);
                
   
  }else if ((buttonState == HIGH)&&(buttonState2 == LOW)&&(buttonState3 == LOW)){
     Level = "Medium"; 
    Serial.println( "WATER LEVEL - MEDIUM" );                     
    digitalWrite(Buzzer, LOW);           
  
  }
  else if ((buttonState == LOW)&&(buttonState2 == LOW)&&(buttonState3 == LOW)){
    Level = "Full";
    Serial.println( "WATER LEVEL - HIGH" ); // if the value is LOW                       // the level is low
    digitalWrite(Buzzer, HIGH);     
    
  }
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["MAC"] = MAC_char;
  root["Level_Air"] = Level;

  JsonArray& data = root.createNestedArray("data");
  JsonArray& deviceId = root.createNestedArray("deviceId");
  data.add(value);
  data.add(value2);
  data.add(value3 );
  deviceId.add("Sensor_WATER_001"); //device id Sensor WATER LEVEL
  Serial.print("Publish message: ");
  root.printTo(pubmsg);
  String jenis_iot = "Sensor_Water_Level";
  String longitude = "0";
  String latitude = "0";
  String data_1 = MAC_char;
  int data_2 = value;
  int data_3 = value2;
  int data_4 = value3;
  String timestamp_device = "0";
  String kode_device = "0";
  String publisheddata = "#" + jenis_iot + "#" + longitude + "#" + latitude + "#" + data_1 + "#" + data_2 + "#" + data_3 + "#" + data_4 + "#" + timestamp_device + "#" + kode_device;
  Serial.println(publisheddata);
  Serial.println(pubmsg);
  client.publish(routingkey2,(char*) pubmsg.c_str());
  delay(10000);
}

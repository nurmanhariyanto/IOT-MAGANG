/*
    Develop by      : Nurman Hariyanto
    Email           : nurman.hariyanto13@gmail.com
    Project         : Homeautomation
    Version         : 3.0
*/
//library
#include <FS.h> //this needs to be first, or it all crashes and burns...
#include <Ticker.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
//#include <SPI.h>
//#include "MFRC522.h"
#include <PubSubClient.h>
#include "config.h"

int Status = D4;  // Digital pin D6
int sensor = D2;  // Digital pin D7
int sensorState = 0;
int prevState = 0;

void setup() {

  Serial.begin(115200);
  pinMode(sensor, INPUT_PULLUP);   // declare sensor as input
  pinMode(Status, OUTPUT);  // declare LED as output
  Serial.println(F("Booting...."));
  //read config wifi,mqtt dan yang lain
  ReadConfigFile();
  setup_wifi();
  SaveConfigFile();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

}

//setup wifi
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
  WiFiManagerParameter custom_mqtt_keywords1("keyword1", "mqtt keyword1", mqtt_keywords1, 40);
  ///WiFiManagerParameter custom_mqtt_keywords2("keyword2", "mqtt keyword2", mqtt_keywords2, 40);
  WiFiManager wifiManager;
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  wifiManager.addParameter( & custom_mqtt_server);
  wifiManager.addParameter( & custom_mqtt_port);
  wifiManager.addParameter( & custom_mqtt_user);
  wifiManager.addParameter( & custom_mqtt_password);
  wifiManager.addParameter(&custom_mqtt_keywords1);
  //wifiManager.addParameter( & custom_mqtt_keywords2);
  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
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


//calback
void callback(char * topic, byte * payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println();
}
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...%s");
    Serial.println(mqtt_server);
    // Attempt to connect
    if (client.connect(MAC_char, mqtt_user, mqtt_password)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish(mqtt_keywords1, "halo smarthome");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      if (client.state() == 4) ESP.restart();
      else {
        Serial.println(" try again in 5 seconds");
        // Wait 5 seconds before retrying
        //delay(10000);
      }
    }
  }
}

//loop publish
void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  String jenis_iot = "Sensor_Pir";
  String macAdd = MAC_char;
  String data1 = ("detected");
  int data2 = 0;
  int data3 = 0;
  String kode_device = "D92232B5FF";
  sensorState = digitalRead(sensor);
  if (sensorState == HIGH) {
    if (prevState != sensorState) {
      digitalWrite (Status, HIGH);
      Serial.println("Motion detected!");
      String pubmsg =  "#" + jenis_iot + "#" + macAdd + "#" + data1 + "#" + data2 + "#" + data3 + "#" + kode_device;
      char datapub[200];
      pubmsg.toCharArray(datapub, sizeof(datapub));
      client.publish(mqtt_keywords1, datapub);
      Serial.println("Success send to RMQ");
      delay(1000);
    }
  }
  else {
    digitalWrite (Status, LOW);
    Serial.println("Motion absent!");
    delay(1000);
  }

}

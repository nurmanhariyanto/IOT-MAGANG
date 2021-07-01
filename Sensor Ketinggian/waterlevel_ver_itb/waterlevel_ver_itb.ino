/*
    Develop by      : Nurman Hariyanto
    Email           : nurman.hariyanto13@gmail.com
    Project         : Homeautomation
    Version         : 3.0
*/
//library
/*
   Library
   Version:
   <ESP8266WiFi.h> 1.0.0
   <DNSServer.h> 1.1.0
   <ESP8266WebServer.h> 1.0.0
   <WiFiManager.h> 0.12.0
   <ArduinoJson.h> 5.13.2
   <PubSubClient.h> 2.6.0
*/
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

const int pinHigh = D1;
const int pinMedium = D2;
const int pinLow = D3;
const int Buzzer = D4;



// variables will change:
//int sensorApiState = 0;
int statusPin[3] = {0, 0, 0};
String sensorStatus[4] = {"A0", "B0", "C0","D0"};
String typeDevice = "Water Level";


/*
   Setup pin and load config file
*/
void setup() {

  pinMode(pinHigh, INPUT);
  pinMode(pinMedium, INPUT);
  pinMode(pinLow, INPUT);
  pinMode(Buzzer, OUTPUT);
  digitalWrite(pinHigh, HIGH);
  digitalWrite(pinMedium, HIGH);
  digitalWrite(pinLow, HIGH);
  digitalWrite(Buzzer, LOW);
  Serial.begin(115200);
  Serial.println(F("Booting...."));
  //read config wifi,mqtt dan yang lain
  ReadConfigFile();
  setup_wifi();
  SaveConfigFile();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  delay(100);
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
 // WiFiManagerParameter custom_mqtt_user("user", "mqtt user", mqtt_user, 40);
  //WiFiManagerParameter custom_mqtt_password("password", "mqtt password", mqtt_password, 40);
  WiFiManagerParameter custom_mqtt_keywords1("keyword1", "mqtt keyword1", mqtt_keywords1, 40);
  //WiFiManagerParameter custom_mqtt_keywords2("keyword2", "mqtt keyword2", mqtt_keywords2, 40);
  WiFiManager wifiManager;
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  wifiManager.addParameter( & custom_mqtt_server);
  wifiManager.addParameter( & custom_mqtt_port);
  //wifiManager.addParameter( & custom_mqtt_user);
 // wifiManager.addParameter( & custom_mqtt_password);
  wifiManager.addParameter(&custom_mqtt_keywords1);
  // wifiManager.addParameter( & custom_mqtt_keywords2);
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
    if (client.connect(MAC_char)) {
      Serial.println("connected");

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

//loop publish dan reconnect jika terputus dari mqtt server
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  String routeKey = String(mqtt_keywords1);
  statusPin[0] = digitalRead(pinHigh);
  statusPin[1] = digitalRead(pinMedium);
  statusPin[2] = digitalRead(pinLow);
  char dataStatusHigh[300];
  char dataStatusMedium[300];
  char dataStatusLow[300];
  char dataStatusveryLow[300];
  //test print serial
  Serial.println(statusPin[0]);
  //test print route key
  Serial.println(routeKey);
  String typeDevice = "Water Level";
  String macAdd = MAC_char;
  String data2 = "0";
    String data3 = "0";
      String data4 = "0";
        String data5 = "0";
        String kodeDevice = "123456";
  

  //pompa nyala
  if ((statusPin[0] == HIGH) && (statusPin[1] == HIGH) && (statusPin[2] == HIGH))
  {
    digitalWrite(Buzzer, HIGH);
    sensorStatus[3] = "D1";
    String checkveryLow = String("#" + typeDevice + "#" +macAdd + "#" + sensorStatus[3] + "#" + data2 + "#" + data3 + "#" + data4 + "#" + data5 + "#" + kodeDevice);

    checkveryLow.toCharArray(dataStatusveryLow, sizeof(dataStatusveryLow));
    Serial.println(dataStatusveryLow);
    if (client.publish(mqtt_keywords1, dataStatusveryLow) == true) {
      Serial.println("Success sending message");
      Serial.println(dataStatusveryLow);
    } else {
      Serial.println("Error sending message");
    }
    delay(1000);
  }

  //switch 3
  if ((statusPin[0] == HIGH) && (statusPin[1] == HIGH) && (statusPin[2] == LOW))
  {
    sensorStatus[2] = "C1";
    String checkLow = String("#" + typeDevice + "#" +macAdd + "#" + sensorStatus[2] + "#" + data2 + "#" + data3 + "#" + data4 + "#" + data5 + "#" + kodeDevice);

    checkLow.toCharArray(dataStatusLow, sizeof(dataStatusLow));
    Serial.println(dataStatusLow);
    if (client.publish(mqtt_keywords1, dataStatusLow) == true) {
      Serial.println("Success sending message");
      Serial.println(dataStatusLow);
    } else {
      Serial.println("Error sending message");
    }
    delay(1000);
  }

  //switch 2
  if ((statusPin[0] == HIGH) && (statusPin[1] == LOW) && (statusPin[2] == LOW))
  {

    sensorStatus[1] = "B1";
    String checkMedium = String("#" + typeDevice + "#" +macAdd + "#" + sensorStatus[1] + "#" + data2 + "#" + data3 + "#" + data4 + "#" + data5 + "#" + kodeDevice);

    checkMedium.toCharArray(dataStatusMedium, sizeof(dataStatusMedium));
    Serial.println(dataStatusMedium);
    if (client.publish(mqtt_keywords1, dataStatusMedium) == true) {
      Serial.println("Success sending message");
      Serial.println(dataStatusMedium);
    } else {
      Serial.println("Error sending message");
    }
    delay(1000);
  }

 //switch 1
  if ((statusPin[0] == LOW) && (statusPin[1] == LOW) && (statusPin[2] == LOW))
  {
    digitalWrite(Buzzer, HIGH);
    sensorStatus[0] = "A1";
    String checkHigh = String("#" + typeDevice + "#" +macAdd + "#" + sensorStatus[0] + "#" + data2 + "#" + data3 + "#" + data4 + "#" + data5 + "#" + kodeDevice);

    checkHigh.toCharArray(dataStatusHigh, sizeof(dataStatusHigh));
    Serial.println(dataStatusHigh);
    if (client.publish(mqtt_keywords1, dataStatusHigh) == true) {
      Serial.println("Success sending message");
      Serial.println(dataStatusHigh);
    } else {
      Serial.println("Error sending message");
    }
   
  }

   delay(5000);
  //}
}

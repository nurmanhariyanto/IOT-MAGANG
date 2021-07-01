#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <EasyNTPClient.h>
#include <WiFiUdp.h>
//#include <NTPClient.h>
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
WiFiUDP udp;
EasyNTPClient ntpClient(udp, "pool.ntp.org", 25200); 
StaticJsonBuffer<200> jsonBuffer;
void writeString(char add,String data);
String read_String(char add);
char payloadTemp[200];
int soilSensor = A0;
int powerPin = 15;
String commandMessage1 = "1"; //message pesan subscribe
String commandMessage2 = "2"; //message pesan publish
String typeDevice = "Soil Sensor";
char routingkey2[40] ="soilsensor";
String waktu;
const int soil_sensor = A0;
const int LED = 13;
String Kondisi;
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
   pinMode(soil_sensor, INPUT);
  pinMode(LED, OUTPUT);
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
 
}

void loop() {
  String pubmsg = "";
  float speed;
  char time[32];
  char date[32];
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  int adc = analogRead(soil_sensor);     // read the value from the sensor
  float tegangan = adc * (5.0 / 1023.0);       // convertion formula to volt
  Serial.print("ADC:");
  Serial.print(adc);
  Serial.print(" | ");
  Serial.print("tegangan:");
  Serial.println(tegangan, 2);
  delay(1000);
  
  if (tegangan >= 3.7) {
    Serial.println("Kering");
    Kondisi="Kering";
  } else {
    Serial.println("Basah");
    Kondisi="Basah";
  }
   float unixtime = ntpClient.getUnixTime();
   Serial.println(unixtime);
  
  delay(20000); // wait for 20 seconds before refreshing.
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["MAC"] = MAC_char;
  root["Tegangan_Tanah"] =tegangan ;
  root["Kondisi_Tanah"] =Kondisi;
  JsonArray& data = root.createNestedArray("data");
  JsonArray& deviceId = root.createNestedArray("deviceId");
  data.add(adc);
  deviceId.add("Sensor_SOIL_003"); //device id Sensor Soil
  Serial.print("Publish message: ");
  root.printTo(pubmsg);
  String jenis_iot = "Sensor_Soil_001";
  String longitude = "0";
  String latitude = "0";
  String data_1 = MAC_char;
  float  data_2 = tegangan;
  String data_3 = "001";
  String data_4 = "0";
  float  timestamp_device = unixtime;
  String kode_device = "0";
    
  String publisheddata = "#" + jenis_iot + "#" + longitude + "#" + latitude + "#" + data_1 + "#" + data_2 + "#" + data_3 + "#" + data_4 + "#" + timestamp_device + "#" + kode_device;
  Serial.println(publisheddata);
  Serial.println(pubmsg);
  client.publish(routingkey2, pubmsg.c_str());
}

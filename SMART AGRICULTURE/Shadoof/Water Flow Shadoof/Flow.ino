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
#include <NTPClient.h>
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
const char* routingkey2 = "pubwaterflow";
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 25200);

int TURBINE;      //pengukuran SINYAL data yang bersifat incremental
int waterflow = D3; //nama alias pada pin 2
int Calc;
const int LED = 13;

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
void speedrpm ()    //fungsi penghitungan dan interrupt
{
  TURBINE++; //bersifat incrementing (dengan mode falling edge)
}
  byte indikator = 13;
  byte sensorInt = 0;
  byte flowsensor = D3;

  float konstanta = 4.5; // konstanta flow meter
  volatile byte pulseCount;
  float debit;
  unsigned int flowmlt;
  unsigned long totalmlt;
  unsigned long oldTime;
void setup() {
  //Inisialisasi port serial
  Serial.begin(115200);

  pinMode(indikator, OUTPUT);
  digitalWrite(indikator, HIGH);

  pinMode(flowsensor,INPUT);
  digitalWrite(flowsensor, HIGH);
  setup_wifi();
//  printMACAddress();
  client.setServer(mqtt_server, 1883);
//  client.setCallback(callback);
  timeClient.begin();
  pulseCount = 0;
  debit = 0.0;
  flowmlt = 0;
  totalmlt = 0;
  oldTime = 0;

  attachInterrupt(sensorInt, pulseCounter, FALLING);
 
}

void loop() {
 
  if (!client.connected()){
    reconnect();}
//  client.loop();
  if((millis() - oldTime) > 1000){
    detachInterrupt(sensorInt);
    debit = ((1000.0 / (millis() - oldTime)) *pulseCount)/
    konstanta;
    oldTime = millis();
    flowmlt = (debit / 120) *1000;
    totalmlt += flowmlt;

    unsigned int frac;

    Serial.print("Debit air: ");
    Serial.print(int(debit));
    Serial.print("L/min");
    Serial.print("\t");

    Serial.print("Volume: ");
    Serial.print(totalmlt);
    Serial.println("mL");

    pulseCount = 0;
    attachInterrupt(sensorInt,pulseCounter,FALLING);
  }
    String pubmsg="";
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["MAC"] = MAC_char;
  root["Debit_air"] =debit ;
  root["Volume_air"]=totalmlt;
  Serial.print("Publish message: ");
  root.printTo(pubmsg);
  Serial.println(pubmsg);
  client.publish(routingkey2,(char*)pubmsg.c_str());
  delay (1000);
  
}
  void pulseCounter(){
   pulseCount++;
  
  }

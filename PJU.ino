#include <PZEM004Tv30.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>

const char* wifiSsid              = "LSKKHomeAuto";
const char* wifiPassword          = "1234567890";
const char* mqttHost              = "rmq2.pptik.id";
const char* mqttUserName          = "/smkmerdekabandung:smkmerdekabandung";
const char* mqttPassword          = "qwerty";
//const char* mqttClient          = "PJU";
const char* mqttQueuePJU          = "PJU";

String deviceGuid     = "6626b0b5-a868-4f7e-9ebb-b2d68e124a35";
PZEM004Tv30 pzem(14,12); // Software Serial pin 11 (RX) & 12 (TX)


WiFiClient espClient;
PubSubClient client(espClient);
byte mac[6]; //array temp mac address
String MACAddress;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * Set up WiFi connection
 */
 void setup_wifi(){
  delay(10);
  //We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to :");
  Serial.println(wifiSsid);
  WiFi.begin(wifiSsid, wifiPassword);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
 }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * Function for Get Mac Address from microcontroller
 */
 
String mac2String(byte ar[]) {
  String s;
  for (byte i = 0; i < 6; ++i)
  {
    char buf[3];
    sprintf(buf, "%2X", ar[i]);
    s += buf;
    if (i < 5) s += ':';
  }
  return s;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * Function for Print Mac Address 
 */
 void printMACAddress() {
  WiFi.macAddress(mac);
  MACAddress = mac2String(mac);
  Serial.println(MACAddress);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



/*
 * Function for Get message payload from MQTT rabbit mq
 */
void callback(char* topic, byte* payload, unsigned int length){
  char message[5]; //variable for temp payload message
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Messagge :");
  for(int i = 0;i < length;i++){
    Serial.print((char)payload[i]);
    message[i] = (char)payload[i]; //initiate value from payload to message variable
    
  }
  
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * Function for Reconnecting to MQTT/RabbitMQ 
 */
void reconnect() {
  // Loop until we're reconnected
  printMACAddress();
  const char* CL;
  CL = MACAddress.c_str();
  Serial.println(CL);
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(CL, mqttUserName, mqttPassword)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      ESP.restart();
      delay(5000);

    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  printMACAddress();
  client.setServer(mqttHost, 1883);
  client.setCallback(callback);
}

void loop() {
   if (!client.connected()) {
    reconnect();
  }
   
   float voltage = pzem.voltage();
    String convertvoltage = String(voltage);
    char dataToMQTTa[50];
    convertvoltage.toCharArray (dataToMQTTa, sizeof(dataToMQTTa));
   if(voltage != NAN){
       Serial.print("Voltage: ");
       Serial.print(voltage);
       Serial.println("V");
   } else {
       Serial.println("Error reading voltage");
   }
   
   float current = pzem.current();
    String convertcurrent = String(current);
    char dataToMQTTb[50];
    convertcurrent.toCharArray (dataToMQTTb, sizeof(dataToMQTTb));
   if(current != NAN){
       Serial.print("Current: ");
       Serial.print(current);
       Serial.println("A");
   } else {
       Serial.println("Error reading current");
   }
       
   float power = pzem.power();
    String convertpower = String(power);
    char dataToMQTTc[50];
    convertpower.toCharArray(dataToMQTTc, sizeof(dataToMQTTc));
   if(current != NAN){
       Serial.print("Power: ");
       Serial.print(power);
       Serial.println("W");
   } else {
       Serial.println("Error reading power");
   }


   float energy = pzem.energy();
    String convertenergy = String(energy);
    char dataToMQTTd[50];
    convertenergy.toCharArray (dataToMQTTd, sizeof(dataToMQTTd));
   if(current != NAN){
       Serial.print("Energy: ");
       Serial.print(energy,3);
       Serial.println("kWh");
   } else {
       Serial.println("Error reading energy");
   }
 
   float frequency = pzem.frequency();
    String convertfrequency = String(frequency);
    char dataToMQTTe[50];
    convertfrequency.toCharArray (dataToMQTTe, sizeof(dataToMQTTe));
   if(current != NAN){
       Serial.print("Frequency: ");
       Serial.print(frequency, 1); 
       Serial.println("Hz");
   } else {
       Serial.println("Error reading frequency");
   }

String dataPJU = String(deviceGuid + "#" + voltage + "#" + current + "#" + power + "#" + energy + "#" + frequency);
client.publish(mqttQueuePJU, dataPJU.c_str());
client.loop();
   delay(5100);
}

/*
    Develop by      : Fahmi Nurfadilah 
    Email           : fahmi.nurfadilah1412@gmail.com
    Updated by      : Vitradisa Pratama
    Email           : vitradisa@pptik.itb.ac.id
    Updated by      : Nurman Hariyanto
    Email           : nurman.hariyanto13@gmail.com
    Project         : IoT General
    Version         : 1.2
*/

#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

// Update these with values suitable for your network.

const char* ssid = "LSKKHomeAuto";
const char* password = "1234567890";
const char* mqtt_server = "167.205.7.226";
const char* mqtt_user = "/bluelight:bluelight";
const char* mqtt_pass = "bluelight123!";
const char* CL = "LSKK-BL-SENSOR-BANJIR-02";
const char* mqtt_pub_topic = "bluelightin";

char msg[100];

WiFiClient espClient;
PubSubClient client(espClient);

byte mac[6];
String MACAddress;

const int echoPin = D1;
const int trigPin = D2;
const int LED = D3;
const int LEDPin = 0;
int loop_count  = 0 ; //loop count loop
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
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

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

void printMACAddress() {
  WiFi.macAddress(mac);
  MACAddress = mac2String(mac);
  Serial.println(MACAddress);
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
  // Loop until we're reconnected
  printMACAddress();
  const char* CL;
  CL = MACAddress.c_str();
  Serial.println(CL);
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(CL, mqtt_user, mqtt_pass)) {
      Serial.println("connected");
      digitalWrite(LED, HIGH);
    } else {
      digitalWrite(LED, LOW);
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      ESP.restart();
      delay(5000);
    }
  }
}

void setup()
{
  Serial.begin(115200);
  setup_wifi();
  printMACAddress();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  pinMode(LED, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(LEDPin, OUTPUT);
  watchdogSetup();
}

void watchdogSetup(void) {
  cli();
  ESP.wdtDisable();
}
void loop() {
  String pubmsg1 = "";
  String pubmsg2 = "";
  String pubmsg3 = "";
  String pubmsg4 = "";
  /* Berikut siklus trigPin atau echo pin yang digunakan
    untuk menentukan jarak objek terdekat dengan memantulkan
    gelombang suara dari itu. */
  int maximumRange = 200; //kebutuhan akan maksimal range
  int minimumRange = 00; //kebutuhan akan minimal range
  long duration, distance; //waktu untuk kalkulasi jarak
  digitalWrite(trigPin, LOW); delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration / 58.2;
  String jenis_iot = "Input-Sensor-Banjir";
  String mac = MACAddress;
  String data_1 = ("detected");
  int data_2 = (distance);
  String siaga1 = ("Siaga 1");
  String siaga2 = ("Siaga 2");
  String siaga3 = ("Siaga 3");
  String siaga4 = ("Banjir");
  String aman = ("aman");
  String kode_device = "D92232B5FF";
  Serial.print("Publish message: ");


  //perhitungan untuk dijadikan jarak
  
  for (int i = 0; i <= loop_count; i++) {
    if (!client.connected()) {
      reconnect();
    }
    client.loop();

  }
  if (distance >= maximumRange || distance <= minimumRange)
  {
    Serial.println("-1"); digitalWrite(LEDPin, HIGH);
  }
  if (distance >= 200)
  {
    Serial.println("Aman");
      String pubmsg5 = "#" + jenis_iot + "#" + mac + "#" + data_1 + "#" + data_2 + "#" + aman + "#" + kode_device;
    //harus di convert dulu ke charbyte
    char dataStatus5[200];
    pubmsg5.toCharArray(dataStatus5, sizeof(dataStatus5));

    client.publish(mqtt_pub_topic, dataStatus5);
    Serial.println(dataStatus5);


  }
  else if (distance >= 180)
  {
    Serial.println("Siaga 1");
    String pubmsg1 = "#" + jenis_iot + "#" + mac + "#" + data_1 + "#" + data_2 + "#" + siaga1 + "#" + kode_device;
    //harus di convert dulu ke charbyte
    char dataStatus1[200];
    pubmsg1.toCharArray(dataStatus1, sizeof(dataStatus1));

    client.publish(mqtt_pub_topic, dataStatus1);
    Serial.println(dataStatus1);

  }
  else if (distance >= 120)
  {
    Serial.println("Siaga 2");
    String pubmsg2 = "#" + jenis_iot + "#" + mac + "#" + data_1 + "#" + data_2 + "#" + siaga2 + "#" + kode_device;
    //harus di convert dulu ke charbyte
    char dataStatus2[200];
    pubmsg2.toCharArray(dataStatus2, sizeof(dataStatus2));

    client.publish(mqtt_pub_topic, dataStatus2);
    Serial.println(dataStatus2);
  }
  else if (distance >= 60)
  {
    Serial.println("Siaga 3");
    String pubmsg3 = "#" + jenis_iot + "#" + mac + "#" + data_1 + "#" + data_2 + "#" + siaga3 + "#" + kode_device;
    //harus di convert dulu ke charbyte
    char dataStatus3[200];
    pubmsg3.toCharArray(dataStatus3, sizeof(dataStatus3));

    client.publish(mqtt_pub_topic, dataStatus3);
    Serial.println(dataStatus3);
  }

  else if (distance < 60)
  {
    Serial.println("Banjir");
    String pubmsg4 = "#" + jenis_iot + "#" + mac + "#" + data_1 + "#" + data_2 + "#" + siaga4 + "#" + kode_device;
    //harus di convert dulu ke charbyte
    char dataStatus4[200];
    pubmsg4.toCharArray(dataStatus4, sizeof(dataStatus4));

    client.publish(mqtt_pub_topic, dataStatus4);
    Serial.println(dataStatus4);
  }
  /*Kirim jarak ke komputer menggunakan Serial protokol, dan
    menghidupkan LED OFF untuk menunjukkan membaca sukses. */
  Serial.println(distance);
  digitalWrite(LEDPin, LOW);
 loop_count++;
  ESP.wdtFeed();
  Serial.print(loop_count);
  Serial.print(". Watchdog fed in approx. ");
  Serial.print(loop_count * 200);
  Serial.println(" milliseconds.");


  delay(1000);
}

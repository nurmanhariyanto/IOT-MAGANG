/* 
 *  PLEASE DONT DELETE AND EDIT 
 *  Develop by          : Nurman Hariyanto
 *  Email               : nurman.hariyanto13@gmail.com
 *  Project             : PKL-ONLINE Relay Pump
 *  Version             : 1.0
 *  Description         : This code for control relay on and off, displays status relay to serial monior 
 *  Microcontroller     : Wemos Mini ESP8266
 *                        NodeMCU ESP8266
 *  Device              : Relay 5v                  
 */




/*
 * YOU CAN EDIT NOW
 */


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/*
 * Device parameter
 * -Guid -> Id Device (unique) you can generate from here (https://www.uuidgenerator.net/version1) 
 * -status device -> save last state from the pump is on or off (1 = on , 0 = off) 
 * -pin microcontroller 
 */
String deviceGuid                = "8d346f0e-0f83-43eb-b5d9-c2d1ca5bbeeb"; //You can change this guid with your guid 
int devicePin                    = D1; //this is a Digital Pin for output 

void setup(){
  pinMode(devicePin,OUTPUT);
  digitalWrite(devicePin,LOW); //Initiate relay with LOW
  Serial.begin(115200);
}


void loop(){
  Serial.print("Device ID :");
  Serial.println(deviceGuid);
  Serial.print("Status : ");
  digitalWrite(devicePin,HIGH);
  Serial.println("ON"); 
  Serial.println("======================================================"); 
  delay(5000); //will off in 5 sec


  Serial.print("Device ID :");
  Serial.println(deviceGuid);
  Serial.print("Status : ");
  digitalWrite(devicePin,LOW);
  Serial.println("OFF"); 
  Serial.println("======================================================"); 
  delay(5000); // will on in 5 sec 
  
}

/* 
 *  PLEASE DONT DELETE AND EDIT 
 *  Develop by          : Nurman Hariyanto
 *  Email               : nurman.hariyanto13@gmail.com
 *  Project             : PKL-ONLINE Soil Moisture
 *  Version             : 1.0
 *  Description         : This code for get data from soil moisture and display data to serial monitor 
 *  Microcontroller     : Wemos Mini ESP8266
 *                        NodeMCU ESP8266
 *  Device              : Soil Moisture Sensor Module                   
 */




/*
 * YOU CAN EDIT NOW
 */


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/*7
 * Device parameter
 * -Guid -> Id Device (unique) you can generate from here (https://www.uuidgenerator.net/version1) 
 * -status device -> save last state from the pump is on or off (1 = on , 0 = off) 
 * -pin microcontroller 
 */
String deviceGuid                = "8d346f0e-0f83-43eb-b5d9-c2d1ca5bbeeb"; //You can change this guid with your guid 
int devicePin                    = A0; //this is a analog pin 

void setup(){
  pinMode(devicePin,INPUT);
  Serial.begin(115200);
}


void loop(){
  int dataSoil= analogRead(devicePin);
  Serial.print("Device ID :");
  Serial.println(deviceGuid);
  Serial.print("Value from soil moisture : ");
  Serial.println(dataSoil); 
  Serial.println("======================================================"); 
  delay(5000);
  
}

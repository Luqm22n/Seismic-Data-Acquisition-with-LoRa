#include "Arduino.h"
#include "LoRa_E32.h"
 
LoRa_E32 e32ttl(&Serial2);
 
void setup() {
Serial.begin(9600);
delay(500);
e32ttl.begin();
Serial.println("Start receiving!");
}
 
void loop() {
if (e32ttl.available() > 1) {
Serial.println("Message received!");
ResponseContainer rc = e32ttl.receiveMessage();
if (rc.status.code != 1) {
Serial.println(rc.status.getResponseDescription());
} else {
Serial.println("Device : ESP32");
Serial.print("Message : ");
Serial.println(rc.data);
Serial.print("Status : ");
Serial.println(rc.status.getResponseDescription());
Serial.println();
}
}
delay(100);
}
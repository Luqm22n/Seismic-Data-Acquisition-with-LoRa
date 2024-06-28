#include <HardwareSerial.h>;
HardwareSerial SerialAT(Serial2);
 
void setup() {
Serial.begin(9600);
SerialAT.begin(9600,SERIAL_8N1);
}
 
void loop() { 
if (Serial.available()) {
SerialAT.write(Serial.read());
}
}
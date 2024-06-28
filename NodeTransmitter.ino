#include <HardwareSerial.h> //serial LoRa
#include <Wire.h>
int i = 0;

HardwareSerial SerialAT(USART2); // port Serial LoRa

 
void setup() {
  Serial.begin(9600);
  SerialAT.begin(9600,SERIAL_8N1);//Serial LoRa 8 bit data tanpa paritas 1 stop bit
}
 
void loop() { 

  
  while (i <= 360 ){
    char nilai[10];
    sprintf(nilai, "%d", i);//karakter integer dirubah ke Char
    SerialAT.write(nilai);//mengirim Char ke serial LoRa
    Serial.println(nilai);//mengirim Char ke serial Monitor
    delay(1000);//mengirim karakter dengan delay setiap 1 detik
    i++;//menambahkan nilai ke variabel i setiap looping
    
  }
  
}
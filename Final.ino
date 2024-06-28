#include <ADS1256.h>
#include <HardwareSerial.h> //serial LoRa
#include <Wire.h>
#include <SD.h>
#include <I2C_RTC.h>
#include <SPI.h>              //Library SPI
#include <Wire.h>             //Library Wire
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

double i = 0;
char nilai[10];
ADS1256 A(PA0, PA1, 0, PA4, 2.500); //DRDY, RESET, SYNC(PDWN), CS, VREF(float). 
HardwareSerial SerialAT(USART2); // port Serial LoRa
const int chipSelect = PB0;//SD Card 
static DS3231 RTC;// Inisiasi RTC

//Insiasi GPS
static const int RXPin = PA9, TXPin = PA10;
static const uint32_t GPSBaud = 9600;
float Latitude;
float Longitude;

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
HardwareSerial ss(RXPin, TXPin);

long rawConversion = 0; 
float voltageValue = 0; 

int singleEndedChannels[8] = {SING_0, SING_1, SING_2, SING_3, SING_4, SING_5, SING_6, SING_7}; 
int differentialChannels[4] = {DIFF_0_1, DIFF_2_3, DIFF_4_5, DIFF_6_7}; 
int inputChannel = 0; 
char inputMode = ' '; 

int pgaValues[7] = {PGA_1, PGA_2, PGA_4, PGA_8, PGA_16, PGA_32, PGA_64}; 
int pgaSelection = 0; 

int drateValues[16] =
{
  DRATE_30000SPS,
  DRATE_15000SPS,
  DRATE_7500SPS,
  DRATE_3750SPS,
  DRATE_2000SPS,
  DRATE_1000SPS,
  DRATE_500SPS,
  DRATE_100SPS,
  DRATE_60SPS,
  DRATE_50SPS,
  DRATE_30SPS,
  DRATE_25SPS,
  DRATE_15SPS,
  DRATE_10SPS,
  DRATE_5SPS,
  DRATE_2SPS
}; 

int drateSelection = 0; 

String registers[11] =
{
  "STATUS",
  "MUX",
  "ADCON",
  "DRATE",
  "IO",
  "OFC0",
  "OFC1",
  "OFC2",
  "FSC0",
  "FSC1",
  "FSC2"
};

int registerToRead = 0; 
int registerToWrite = 0; 
int registerValueToWrite = 0; 

void setup() {
  Serial.begin(115200); 
  SerialAT.begin(9600,SERIAL_8N1);//Serial LoRa 8 bit data tanpa paritas 1 stop bit
  ss.begin(GPSBaud);
  while (!Serial)
  {
    ; 
  }

  A.InitializeADC(); 
  //mengatur nilai PGA
  A.setPGA(PGA_1);  //0b00000000 - DEC: 0
  //--------------------------------------------

  //mengatur mode dan channel input
  A.setMUX(SING_0); //0b01100111 - DEC: 103
  //--------------------------------------------

  //memngatur DRATE
  A.setDRATE(DRATE_30000SPS); //0b00010011 - DEC: 19
  //--------------------------------------------
  delay(3000);

  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");

  RTC.begin();

  RTC.setHourMode(CLOCK_H12);
  //RTC.setHourMode(CLOCK_H24);

  if (RTC.getHourMode() == CLOCK_H12)
  {
    //RTC.setMeridiem(HOUR_PM);
  }

  RTC.setDay(12);
  RTC.setMonth(5);
  RTC.setYear(2024);

  RTC.setHours(19);
  RTC.setMinutes(50);
  RTC.setSeconds(56);

  RTC.setWeek(1);

  //RTC.setDate(22,07,29);
  //RTC.setTime(22,10,20);
}

void loop() {
 while (ss.available() > 0){
    gps.encode(ss.read());
    if (gps.location.isUpdated()){
      Latitude = gps.location.lat(), 6;
      Longitude = gps.location.lng(), 6;
    }
  }
  
  i = (A.convertToVoltage(A.readSingleContinuous()), 6);
  sprintf(nilai, "%d", i);//karakter integer dirubah ke Char
  Serial.print(A.convertToVoltage(A.readSingleContinuous()), 6);
  Serial.print("Latitude : "  ); 
  Serial.print(Latitude,8);
  Serial.print("Longitude : "  ); 
  Serial.println(Longitude,8);
  SerialAT.write(nilai);//mengirim ADC Read Char ke serial LoRa
  // datalogger
  File dataFile = SD.open("datalog1.txt", FILE_WRITE);
  switch (RTC.getWeek())
  {
    case 1:
      //Serial.print("SUN");
      dataFile.print(String("SUN"));
      break;
    case 2:
      //Serial.print("MON");
      dataFile.print(String("MON"));
      break;
    case 3:
      //Serial.print("TUE");
      dataFile.print(String("TUE"));
      break;
    case 4:
      //Serial.print("WED");
      dataFile.print(String("WED"));
      break;
    case 5:
      //Serial.print("THU");
      dataFile.print(String("THU"));
      break;
    case 6:
      //Serial.print("FRI");
      dataFile.print(String("FRI"));
      break;
    case 7:
      //Serial.print("SAT");
      dataFile.print(String("SAT"));
      break;
  }
  //Serial.print(" ");
  dataFile.print(String(" "));
  
  //Serial.print(RTC.getDay());
  dataFile.print(String(RTC.getDay()));
  //Serial.print("-");
  dataFile.print(String("-"));
  //Serial.print(RTC.getMonth());
  dataFile.print(String(RTC.getMonth()));
  //Serial.print("-");
  dataFile.print(String("-"));
  //Serial.print(RTC.getYear());
  dataFile.print(String(RTC.getYear()));

  //Serial.print(" ");
  dataFile.print(String(" "));

  //Serial.print(RTC.getHours());
  dataFile.print(String(RTC.getHours()));
  //Serial.print(":");
  dataFile.print(String(":"));
  //Serial.print(RTC.getMinutes());
  dataFile.print(String(RTC.getMinutes()));
  //Serial.print(":");
  dataFile.print(String(":"));
  //Serial.print(RTC.getSeconds());
  dataFile.print(String(RTC.getSeconds()));
  //Serial.print(" ");
  dataFile.print(String(" "));

  if (RTC.getHourMode() == CLOCK_H12)
  {
    if(RTC.getMeridiem() == HOUR_AM){
      //Serial.println(" AM");
      dataFile.print(String(" AM"));
    }
    if (RTC.getMeridiem() == HOUR_PM){
      //Serial.println(" PM"); 
      dataFile.print(String(" PM"));
    }
  }
  dataFile.print(String(" "));
  dataFile.print(String(Nilai));
  dataFile.print(String(" "));
  Serial.print("Latitude : "  ); 
  Serial.print(Latitude,8);
  dataFile.print(String(" "));
  Serial.print("Longitude : "  ); 
  Serial.println(Longitude,8);
  dataFile.close();
  delay(1000);
}
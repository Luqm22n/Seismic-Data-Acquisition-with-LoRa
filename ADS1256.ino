#include <ADS1256.h>

ADS1256 A(16, 17, 0, 5, 2.500); //DRDY, RESET, SYNC(PDWN), CS, VREF(float). 

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

void setup()
{
  Serial.begin(115200); 

  while (!Serial)
  {
    ; 
  }

  Serial.println("melakukan inisiasi ADS 1256...");

  A.InitializeADC(); 

  Serial.println("Selesai inisialisasi ADS 1256");

  //mengatur nilai PGA
  A.setPGA(PGA_1);  //0b00000000 - DEC: 0
  //--------------------------------------------

  //mengatur mode dan channel input
  A.setMUX(SING_0); //0b01100111 - DEC: 103
  //--------------------------------------------

  //memngatur DRATE
  A.setDRATE(DRATE_30000SPS); //0b00010011 - DEC: 19
  //--------------------------------------------

  //menampilkan nilai pengaturan ke serial Monitor
  Serial.print("PGA: ");
  Serial.println(A.readRegister(IO_REG));
  delay(100);
  //--
  Serial.print("MUX: ");
  Serial.println(A.readRegister(MUX_REG));
  delay(100);
  //--
  Serial.print("DRATE: ");
  Serial.println(A.readRegister(DRATE_REG));
  delay(100);

  delay(3000);
}
/*
case 's': //menghentikan pembacaan data
case 'L': //melakukan self calibration
case 'G': //pembacaan 1 kanal singgel endded
case 'C': //pembacaan 8 kanal singgel endded
case 'E': //pembacaan 1 kanal Differensial
case 'D': //pembacaan 4 kanal Differensial
case 'B': //test kecepatan konversi
case 'T': //menguji pengiriman perintah ke serial
case 'a': //menguji pembacaan single ended 1 kali loop
case 'M': //set MUX
case 'P': //Set PGA
case 'F': //Set sampling frequency
case 'R': //read register
case 'W': //Write register
**/
void loop()
{
  if (Serial.available() > 0)
  {
    char commandCharacter = Serial.read(); 

    switch (commandCharacter) 
    {
      case 's': //menghentikan pembacaan data
        A.stopConversion();
        break;
      //--------------------------------------------------------------------------------------------------------
      case 'L': //melakukan self calibration
        A.sendDirectCommand(SELFCAL);
        break;
      //--------------------------------------------------------------------------------------------------------
      case 'G': //pembacaan 1 kanal singgel endded
        while (Serial.read() != 's') //menghentikan pembacaan 
        {
          Serial.println(A.convertToVoltage(A.readSingleContinuous()), 6);
        }
        A.stopConversion();
        break;
      //--------------------------------------------------------------------------------------------------------
      case 'C': //pembacaan 8 kanal singgel endded 
        while (Serial.read() != 's') //menghentikan pembacaan
        {
          for (int i = 0; i < 8; i++)
          {
            Serial.print(A.convertToVoltage(A.cycleSingle()), 4); 
            Serial.print("\t");
          }
          Serial.println();
        }
        A.stopConversion();
        break;
      //--------------------------------------------------------------------------------------------------------
      case 'D': //pembacaan 4 kanal Differensial
        while (Serial.read() != 's') //menghentikan pembacaan
        {
          for (int j = 0; j < 4; j++)
          {
            Serial.print(A.convertToVoltage(A.cycleDifferential()), 4);
            Serial.print("\t"); 
          }
          Serial.println(" ");
        }
        A.stopConversion();
        break;
      //--------------------------------------------------------------------------------------------------------
      case 'E': //pembacaan 1 kanal Differensial
        A.setMUX(differentialChannels[1]);
        while (Serial.read() != 's') //menghentikan pembacaan
        {
          Serial.println(A.convertToVoltage(A.cycleDifferential()), 6);

        }
        A.stopConversion();
        break;
      //--------------------------------------------------------------------------------------------------------
      case 'B': //test kecepatan konversi
        {
          long numberOfSamples = 15000; 
          long finishTime = 0;
          long startTime = micros();

          for (long i = 0; i < numberOfSamples; i++)
          {
            A.readSingleContinuous();            
          }

          finishTime = micros() - startTime; 

          A.stopConversion();

          //Printing the results
          Serial.print("Total conversion time for 150k samples: ");
          Serial.print(finishTime);
          Serial.println(" us");

          Serial.print("Sampling rate: ");
          Serial.print(numberOfSamples * (1000000.0 / finishTime), 3);
          Serial.println(" SPS");
        }
        break;
      //--------------------------------------------------------------------------------------------------------
      case 'T': //menguji pengiriman perintah ke serial
        Serial.println("pengujian pengiriman perintah");
        break;
      //--------------------------------------------------------------------------------------------------------
      case 'a': //menguji pembacaan single ended 1 kali loop

        rawConversion = A.readSingle(); 
        voltageValue = A.convertToVoltage(rawConversion); 

        //Printing the results
        Serial.print("Single-ended conversion result: ");
        Serial.println(voltageValue, 8); 
        break;
      //--------------------------------------------------------------------------------------------------------
      case 'M': //set MUX
        {
          while (!Serial.available());
          inputMode = Serial.read(); 

          if (inputMode == 's') //single endded
          {
            while (!Serial.available());
            inputChannel = Serial.parseInt();
            A.setMUX(singleEndedChannels[inputChannel]);
            //contoh: masukan karakter  "Ms1" ke serial terminal untuk melakukan pembacaan single endded pada channel 1 
          }

          if (inputMode == 'd') //differensial
          {
            while (!Serial.available());
            inputChannel = Serial.parseInt();
            A.setMUX(differentialChannels[inputChannel]);
            //contoh: masukan karakter  "Md1" ke serial terminal untuk melakukan pembacaan differensial Diff_0_1 
          }
        }
        break;
      //--------------------------------------------------------------------------------------------------------
      case 'P': //Set PGA
        {
          pgaSelection = Serial.parseInt();
          while (!Serial.available());
          A.setPGA(pgaValues[pgaSelection]);
          // Contoh : P4 untuk mengatur PGA = 16 
        }
        break;
      //--------------------------------------------------------------------------------------------------------
      case 'F': //Set sampling frequency
        {
          while (!Serial.available());
          drateSelection = Serial.parseInt(); 
          delay(100);
          Serial.print("DRATE is selected as: ");
          Serial.println(drateValues[drateSelection]); 
          delay(100);
          A.setDRATE(drateValues[drateSelection]);
          delay(100);
          Serial.print("DRATE is set to ");
          Serial.println(A.readRegister(DRATE_REG)); 
          // contoh : F0 untuk DRATE = 30000 SPS 
        }
        break;
      //--------------------------------------------------------------------------------------------------------
      case 'R': //read register
        {
          while (!Serial.available());
          registerToRead = Serial.parseInt(); //membaca jumlah register pada serial port
          Serial.print("Value of ");
          Serial.print(registers[registerToRead]);
          Serial.print(" register is: ");
          Serial.println(A.readRegister(registerToRead));
          //Contoh : "R2" untuk membaca register pada alamat 2 yang merupakan register ADCON
          //Note: Nilai dicetak sebagai angka desimal
        }
        break;
      //--------------------------------------------------------------------------------------------------------
      case 'W': //Write register
        {
          while (!Serial.available());
          registerToWrite = Serial.parseInt(); //Bagian ini membaca nomor register dari port serial
          while (!Serial.available());
          registerValueToWrite = Serial.parseInt(); //Bagian ini membaca nilai register dari port serial

          A.writeRegister(registerToWrite, registerValueToWrite);
          //Contoh: “W1 35” akan menulis 35 (“00100011”) pada register 1 yang merupakan register MUX.
          //Ini akan membuat input sebagai DIFF_2_3 (A2(+) dan A1(-))
        }
        break;
        //--------------------------------------------------------------------------------------------------------
    }
  }
}

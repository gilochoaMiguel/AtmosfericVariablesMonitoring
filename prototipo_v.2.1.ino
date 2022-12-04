//necessary libraries for code
  #include <Wire.h>

//library for the DS3231 Serial clock
  #include "RTClib.h"

//library for the dht humidity and temperature sensor
  #include "DHT.h"

//Libraries for the SD memory card module
#include <SPI.h>
#include <SD.h>
File myFile;
String num[32] = {"00", "01", "02", "03", "04", "05",  "06", "07","08","09","10","11","12","13","14","15","16","17","18","19","20","21","22","23","24","25","26","27","28","29","30","31"};

String test_file = "Data.txt";

//Dht humidity and temperature sensor initialization
#define DHTTYPE DHT22
const int DHTPin = 5;                                  //digital pin asigned for input (can change)
DHT dht(DHTPin, DHTTYPE);


// RTC_DS1307 Serial clock initialization;
RTC_DS3231 rtc;

//YL-69 Soil humidity sensor initialization
int rainPin = A0;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void printData(DateTime date,float h, float t, int sm);

 uint8_t today = 0, reg_day = 0;
float h,t;
int soilMoisture;

void setup() {
  Wire.begin();
  Serial.begin(9600);
  delay(1000);
  
  pinMode(rainPin, INPUT);

  dht.begin();

 if (!rtc.begin()) {
      Serial.println(F("No se encuentra el RTC"));
      while (1);
   }
  
   // Si se ha perdido el suministro eléctrico, fijar fecha y hora
   if (rtc.lostPower()) {
      // Fijar a fecha y hora (poner la de compilación del sketch)
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
   }
Serial.print("Initializing SD card...");
if (!SD.begin(10)) {
Serial.println("initialization failed!");
}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  DateTime hoy = rtc.now();
  today = hoy.day();

  h = dht.readHumidity();
  t = dht.readTemperature();
  soilMoisture=analogRead(rainPin);
  soilMoisture=(soilMoisture-1020)/-6.8;
  if(today==reg_day)
  { 
   printData(hoy,h,t,soilMoisture);
   delay(1200000);  
  }
  else{
    Serial.println("end of day");
    reg_day=hoy.day();
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void printData(DateTime date,float h, float t, int sm)
{
   
   myFile = SD.open("test.txt", FILE_WRITE);

   Serial.print(date.hour(), DEC);
   Serial.print(':');
   Serial.print(date.minute(), DEC);
   Serial.print("\t humidity: \t");
   Serial.print(h);
   Serial.print("\t temperature: \t");
   Serial.print(t);
   Serial.print("\t soil moisture%: \t");
   Serial.print(sm);
   Serial.print('%');
   Serial.println();
}

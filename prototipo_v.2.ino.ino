/*
 * brief: Este es un sistema version 2.1 del prototipo de recaudacion de variables atmosfericas para 
 *        PAP Restauración del bosque escuela con el ITESO. este sistema utiliza los modulos: 
 *        
 *        DHT22 Sensor de humedad y temperatura atmosferica
 *        DS3231 Reloj de presicion serial
 *        YL-38 y YL-69 Sensor analógico de humedad de suelo
 *        Modulo de tarjeta SD para guardar los datos
 * 
 * 
 * programador: Héctor Miguel Gil Ochoa
 * correo: ie708428@iteso.mx
 * 
 * fecha: 08/Noviembre/2022
 */


//librerías necesarias para codificación
  #include <Wire.h>

//Libreria para el reloj serial DS3231
  #include "RTClib.h"

//Libreria para el sensor dht22 de humedad y temperatura
  #include "DHT.h"

//Librerias e inicialización para el modulo de memoria SD
#include <SPI.h>
#include <SD.h>
File myFile;
String monthname[13]={"00","jan","feb","mar","apr","may","jun","jul","aug","sep","oct","nov","dec"}; //Variables para los nombres de los archivos
String filename;

//Inicialización del sensor dht22 
#define DHTTYPE DHT22
const int DHTPin = 5;                        //DIGITAL PIN asigned for input (can change)//////////////////////////////////////////
DHT dht(DHTPin, DHTTYPE);


// Inicialización del reloj RTC_DS1307
RTC_DS3231 rtc;                              //Check diagram for connection

//Inicialización del sensor YL-69/YL-38 de humedad de suelo
int rainPin = A0;                            //ANALOG PIN asigned for input (can change)//////////////////////////////////////////


//iniciaización de la funcion de impresión de datos 
void printData(DateTime date,float h, float t, int sm);

//Inicialización de las varuiables de control local del sistema
uint8_t today = 0, reg_day = 0;
float h,t;
int soilMoisture;

/*
 * brief: inicialización del sistema y comprobación de conecciones.
 */
void setup() {
  Serial.begin(9600);
  delay(1000);          //espera a que el Serial se inicialice
  
  pinMode(rainPin, INPUT);

  dht.begin();

 if (!rtc.begin()) {
      Serial.println("No se encuentra el RTC");
      while (1);        //Si no se encuentra el reloj no inicia el sensado
   }
  
   // Si se ha perdido el suministro eléctrico, fijar fecha y hora
   if (rtc.lostPower()) {
      // Fijar a fecha y hora (poner la de compilación del sketch)
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
       
      // Fijar a fecha y hora específica. En este ejemplo el 2021-01-01 a las 00:00:00
      // rtc.adjust(DateTime(2020, 1, 1, 0, 0, 0));
   }

   Serial.print("Initializing SD card...");

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);              //Si no se encuentra la memoria no inicia el sensado
  }
  Serial.println("initialization done.");


}

/*
 * brief: Funcionamiento cíclico del sistema.
 */
 
void loop() {
  //Variable para leer la fecha y hora actual.
  DateTime now = rtc.now();
  today = now.day();

  //se leen y acondicionan las variables ambientales
  h = dht.readHumidity();
  t = dht.readTemperature();
  soilMoisture=analogRead(rainPin);
  soilMoisture=(soilMoisture-1020)/-6.8;

   printData(now,h,t,soilMoisture,myFile);
   Serial.print("print in"+now.day()+' '+now.month()+'\n');
  
   delay(1200000);    //Espera el tiempo de sensado en ms (20 minutos) 20 minutos*60 000 ms = 1 200 000
}

/*
 * 
 * brief: esta funcion guarda los datos especificadops en la memoria SD
 * 
 * output:  void
 * 
 * input:   DateTime  date    : esta variable recibe una estructura del tipo RTC_DS3231 rtc.now() para calcular fecha y hora.
 *          float     h       : esta variable de punto flotante recibe el valor en porcentaje de la humedad mbiental actual.  
 *          float     t       : esta variable de punto flotante recibe el valor en grados Celsios de la temperatura ambiental actual.
 *          int       sm      : esta variable entera recibe el valor en porcentaje de humedad de suelo actual.
 *          File      myFyle  : esta variable recibe una estructura para el uso del modulo de memoria SD
 * 
 */
void printData(DateTime date,float h, float t, int sm, File myFile)
{
   
    filename=monthname[date.month()]+".txt";
    myFile = SD.open(filename, FILE_WRITE);
    myFile.print(date.day(), DEC);
    myFile.print(" de ");
    myFile.print(monthname[date.month()]+'\t');
    myFile.print(date.hour(), DEC);
    myFile.print(':');
    myFile.print(date.minute(), DEC);
    myFile.print("\t humidity: \t");
    myFile.print(h);
    myFile.print("%\t temperature: \t");
    myFile.print(t);
    myFile.print("\t soil moisture%: \t");
    myFile.print(sm);
    myFile.print('%');
    myFile.println();
    myFile.close();
}

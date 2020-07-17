#include <SPI.h>      // Incluye libreria para interfaz SPI
#include <LoRa.h>     // Incluye libreria para el manejo del modulo LoRa RF96
#include <Wire.h>     // Incluye libreria para interfaz I2C
#include <RTClib.h>   // Incluye libreria para el manejo del modulo RTC
#include "max6675.h"

///////////////////////////////////////////////
// Presion

int valor1 = A3;
int val1 = 0;
int presionA = 0;

int valor2 = A6;
int val2 = 0;
int presionB = 0;

int valor3 = A7;
int val3 = 0;
int presionC = 0;


//////////////////////////////////////////////
int thermoDO = 4;
int thermoCS = 7;
int thermoCLK = 6;

int thermoCSB = 8;
int thermoCSC = 9;


MAX6675 thermocoupleA(thermoCLK, thermoCS, thermoDO);
MAX6675 thermocoupleB(thermoCLK, thermoCSB, thermoDO);
MAX6675 thermocoupleC(thermoCLK, thermoCSC, thermoDO);

int sensorCount[] = {0, 0, 0};
float sensorAverage[] = {0.0, 0.0, 0.0};

float average(int pos, float sensorData) {
  if (sensorCount[pos] < 3) {
    sensorAverage[pos] = sensorAverage[pos] + sensorData;
    sensorCount[pos] = sensorCount[pos] + 1;
    return 0.0;
  } else {
    float valFinal = sensorAverage[pos] + sensorData;
    sensorAverage[pos] = 0.0;
    sensorCount[pos] = 0;
    return valFinal / 4;
  }
}


RTC_DS3231 rtc;     // Crea objeto del tipo RTC_DS3231


void setup() {

  Serial.begin(9600);


  if (!rtc.begin()) {       // Si falla la inicializacion del modulo
    Serial.println("¡Fallo al inicializar Modulo RTC!");  // Muestra mensaje de error
    while (1);       // Bucle infinito que detiene ejecucion del programa
  }
  if (!LoRa.begin(915E6)) {  //Si falla la inicializacion del modulo
    Serial.println("¡Fallo al inicializar Modulo LoRa!"); // Muestra mensaje de error
    while (1); // Bucle infinito que detiene ejecucion del programa
  }

  delay(500);
}
void loop() {


  DateTime fecha = rtc.now();// Funcion que devuelve fecha y horario en formato
  //   DateTime y asigna a variable fecha
  char buf2[] = "DD/MM/YYYY hh:mm:ss";
 
  ///////////////////////////////////
  val1 = analogRead(valor1);
  presionA = map(val1, 0, 1023, 0, 2000);

  val2 = analogRead(valor2);
  presionB = map(val2, 0, 1023, 0, 2000);

  val3 = analogRead(valor3);
  presionC = map(val3, 0, 1023, 0, 2000);
  /////////////////////////////////////////

  // Serial.print(fecha.toString(buf2));
  // Serial.print(",");

  float sensor1 = average(0, thermocoupleA.readCelsius());
  float sensor2 = average(1, thermocoupleB.readCelsius());
  float sensor3 = average(2, thermocoupleC.readCelsius());

  if (sensor1 != 0.0 || sensor2 != 0.0 || sensor3 != 0.0) {
    /* Serial.print(sensor1);
    Serial.print(",");
    Serial.print(sensor2);
    Serial.print(",");
    Serial.print(sensor3);
    Serial.print(",");
    Serial.print(presionA);
    Serial.print(",");
    Serial.print(presionB);
    Serial.print(",");
    Serial.println(presionC); */
    // String Data = (String)"{\"date\":\""+fecha.toString(buf2)+"\",\"sensorsP\":[{\"name\":\"s4\",\"type\":\"p\",\"val\":"+presionA+"},{\"name\":\"s5\",\"type\":\"p\",\"val\":"+presionB+"},{\"name\":\"s6\",\"type\":\"p\",\"val\":"+presionC+"}],";
    // String Data2 = (String)"\"sensorsT\":[{\"name\":\"s1\",\"type\":\"t\",\"val\":"+sensor1+"},{\"name\":\"s2\",\"type\":\"t\",\"val\":"+sensor2+"},{\"name\":\"s3\",\"type\":\"t\",\"val\":"+sensor3+"}]}";
    String Data = (String)""+fecha.toString(buf2)+"|"+presionA+"|"+presionB+"|"+presionC+"|"+sensor1+"|"+sensor2+"|"+sensor3+"|";
    Serial.println(Data);
    // Serial.println(Data2);
    // Serial.println();
        
    LoRa.beginPacket();

    LoRa.println(Data);
    // LoRa.println(Data2);
    // LoRa.println();
    /* LoRa.print( fecha.toString(buf2) );
    LoRa.print(",");
    LoRa.print(sensor1);
    LoRa.print(",");
    LoRa.print(sensor2);
    LoRa.print(",");
    LoRa.print(sensor3);
    LoRa.print(",");
    LoRa.print(presionA);
    LoRa.print(",");
    LoRa.print(presionB);
    LoRa.print(",");
    LoRa.println(presionC); */
    // LoRa.println();
    LoRa.endPacket();

  }

  delay(1000);
}

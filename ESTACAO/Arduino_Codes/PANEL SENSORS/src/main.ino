#include "ACS712.h"

float currentDC =0;
float batt_lvl = 0;
unsigned long contador= 0;


  int pinBatteryVoltage = A1;
  ACS712 sensor(ACS712_30A, A0);
  float vout = 0.0;
  float vin = 0.0;
  float R1 = 30000.0; //
  float R2 = 7500.0; //
  int value = 0;





    float get_battery_level()
    {
    value = analogRead(pinBatteryVoltage);
    vout = (value * 4.88) / 1023.0; // see text
    vin = vout / (R2/(R1+R2));
      //REGRESSÃO LINEAR FEITA PRA CORRIGIR O EFEITO DO SENSOR DE LUZ DA SHIELD
    //vin = ((1.566*vin) + 0.07800);
    return vin;


    }
  //Calculates each of the variables that wunderground is expecting
  void calcWeather()
  {
      //Calc battery level
      batt_lvl = get_battery_level();


      //sensor.setZeroPoint(510);
      currentDC = sensor.getCurrentDC();
  }


  void printWeather()
  {
      calcWeather(); //Go calc all the various sensors

      Serial.println();
      Serial.print('@');
        Serial.print("|");
          Serial.print(contador++);
        Serial.print("|");

      // Serial.print(",batt_lvl=");
      Serial.print(batt_lvl, 4);
      Serial.print("|");


      Serial.print(currentDC, 4);
      Serial.println("|");

    }
    void setup()
    {
        Serial.begin(115200);
        Serial.println("Weather Shield Example");
        sensor.setZeroPoint(511);
    }


    void loop(){
        //Keep track of which minute it is

            printWeather();


      delay(1000);


    }

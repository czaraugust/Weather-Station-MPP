#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_ADS1015.h>

#define ANALOG_PIN_0 36
int analog_value = 0;
Adafruit_ADS1115 ads(0X48);  /* Use this for the 16-bit version */

void setup()
{
  Serial.begin(115200);
  ads.setGain(GAIN_TWOTHIRDS);
  ads.begin();
  delay(1000); // give me time to bring up serial monitor
  Serial.println("ESP32 Analog IN Test");
}

void loop()
{

  float factor = 0.1875;

    int16_t results;
    results = ads.readADC_Differential_0_1();

    float mv =(results * factor)/1000;
    Serial.print("TENSAO: ");
    Serial.println(mv,10);
  // analog_value = analogRead(ANALOG_PIN_0);
  // Serial.println(analog_value);
   delay(1000);
}

// ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
// ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
// ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
// ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
// ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
// ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_ADS1015.h>
Adafruit_ADS1115 ads(0X48);  /* Use this for the 16-bit version */
void setup() {
  Serial.begin(9600);
  ads.setGain(GAIN_SIXTEEN);
  ads.begin();
       // put your setup code here, to run once:
}

void loop() {
  //float adc0, adc1, adc2, adc3;

float factor = 0.0078125;

  int16_t results;
  results = ads.readADC_Differential_0_1();
  
  float mv =results * factor;

  float irradiancia = mv *(1/(1.69/100));

  Serial.print("TENSAO: ");
  Serial.print(mv,10);
  Serial.print("   IRRADIANCIA: ");
  Serial.println(irradiancia);
  delay(1000);

}

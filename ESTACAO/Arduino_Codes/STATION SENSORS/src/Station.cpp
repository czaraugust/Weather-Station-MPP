#include "Station.h"

Station::Station(){
  this->raintime=0;
  this->dailyrainin =0;
  this->WIND_DIR_AVG_SIZE =120;
  this->ONE_WIRE_BUS =24;
  this->WSPEED =3;
  this->RAIN = 2;
  this->STAT1 =7;
  this->STAT2 =8;
  this->WDIR = A0;
  this->lastWindCheck = 0;
  this->lastWindIRQ =0;
  this->windClicks =0;
  this-> winddir =0;
  this-> windspeedmph=0;
  this->windgustmph=0;
  this->windgustdir=0;
  this->windspdmph_avg2m =0;
  this->winddir_avg2m =0;
  this->windgustmph_10m =0;
  this->windgustdir_10m =0;
  this->humidity =0;
  this->tempf =0;
  this->rainin =0;
  this->pressure =0;
  // OneWire ourWire(ONE_WIRE_BUS);
  // DallasTemperature sensors(&ourWire);



}


 void Station::rainIRQ()
// Count rain gauge bucket tips as they occur
// Activated by the magnet and reed switch in the rain gauge, attached to input D2
{
    raintime = millis(); // grab current time
    raininterval = raintime - rainlast; // calculate interval between this and last event

    if (raininterval > 10) // ignore switch-bounce glitches less than 10mS after initial edge
    {
        dailyrainin += 0.011; //Each dump is 0.011" of water
        rainHour[minutes] += 0.011; //Increase this minute's amount of rain

        rainlast = raintime; // set up for next event
    }
}

 void Station::wspeedIRQ()
// Activated by the magnet in the anemometer (2 ticks per rotation), attached to input D3
{
    if (millis() - lastWindIRQ > 10) // Ignore switch-bounce glitches less than 10ms (142MPH max reading) after the reed switch closes
    {
        lastWindIRQ = millis(); //Grab the current time
        windClicks++; //There is 1.492MPH for each click per second.
    }
}

void Station::setupStation(){



    Serial.println("Weather Shield Example");

    pinMode(STAT1, OUTPUT); //Status LED Blue
    pinMode(STAT2, OUTPUT); //Status LED Green

    pinMode(WSPEED, INPUT_PULLUP); // input from wind meters windspeed sensor
    pinMode(RAIN, INPUT_PULLUP); // input from wind meters rain gauge sensor

    myPressure.begin(); // Get sensor online
    myPressure.setModeBarometer(); // Measure pressure in Pascals from 20 to 110 kPa
    myPressure.setOversampleRate(7); // Set Oversample to the recommended 128
    myPressure.enableEventFlags(); // Enable all three pressure and temp event flags
    sensors.begin();

    //Configure the humidity sensor
    myHumidity.begin();
    //lightMeter.begin();
    seconds = 0;
    lastSecond = millis();
    Serial.println("Weather Shield online!");
    digitalWrite(STAT1, HIGH);

}


float Station:: get_wind_speed()
{
    float deltaTime = millis() - lastWindCheck; //750ms

    deltaTime /= 1000.0; //Covert to seconds

    float windSpeed = (float)windClicks / deltaTime; //3 / 0.750s = 4

    windClicks = 0; //Reset and start watching for new wind
    lastWindCheck = millis();

    windSpeed *= 1.492; //4 * 1.492 = 5.968MPH

    /* Serial.println();
     Serial.print("Windspeed:");
     Serial.println(windSpeed);*/

    return(windSpeed);
}

int Station::get_wind_direction()
{
    unsigned int adc;

    adc = analogRead(WDIR); // get the current reading from the sensor

    // The following table is ADC readings for the wind direction sensor output, sorted from low to high.
    // Each threshold is the midpoint between adjacent headings. The output is degrees for that ADC reading.
    // Note that these are not in compass degree order! See Weather Meters datasheet for more information.

    if (adc < 380) return (113);
    if (adc < 393) return (68);
    if (adc < 414) return (90);
    if (adc < 456) return (158);
    if (adc < 508) return (135);
    if (adc < 551) return (203);
    if (adc < 615) return (180);
    if (adc < 680) return (23);
    if (adc < 746) return (45);
    if (adc < 801) return (248);
    if (adc < 833) return (225);
    if (adc < 878) return (338);
    if (adc < 913) return (0);
    if (adc < 940) return (293);
    if (adc < 967) return (315);
    if (adc < 990) return (270);
    return (-1); // error, disconnected?
}


void Station::loopStation(){
  if(millis() - lastSecond >= 1000) {
        digitalWrite(STAT1, HIGH); //Blink stat LED

        lastSecond += 1000;

        //Take a speed and direction reading every second for 2 minute average
        if(++seconds_2m > 119) seconds_2m = 0;

        //Calc the fnd speed and direction every second for 120 second to get 2 minute average
        float currentSpeed = get_wind_speed();
        windspeedmph = currentSpeed*1.60934; //update global variable for windspeed when using the printWeather() function
        //float currentSpeed = random(5); //For testing
        int currentDirection = get_wind_direction();
        windspdavg[seconds_2m] = (int)currentSpeed;
        winddiravg[seconds_2m] = currentDirection;
        //if(seconds_2m % 10 == 0) displayArrays(); //For testing

        //Check to see if this is a gust for the minute
        if(currentSpeed > windgust_10m[minutes_10m])
        {
            windgust_10m[minutes_10m] = currentSpeed;
            windgustdirection_10m[minutes_10m] = currentDirection;
        }

        //Check to see if this is a gust for the day
        if(currentSpeed > windgustmph)
        {
            windgustmph = currentSpeed;
            windgustdir = currentDirection;
        }

        if(++seconds > 59)
        {
            seconds = 0;

            if(++minutes > 59) minutes = 0;
            if(++minutes_10m > 9) minutes_10m = 0;

            rainHour[minutes] = 0; //Zero out this minute's rainfall amount
            windgust_10m[minutes_10m] = 0; //Zero out this minute's gust
        }

      }
}

void Station::calcWeather()
{
    //Calc winddir
    winddir = get_wind_direction();

    //Calc windspeed
    //windspeedmph = get_wind_speed(); //This is calculated in the main loop on line 179

    //Calc windgustmph
    //Calc windgustdir
    //These are calculated in the main loop

    //Calc windspdmph_avg2m
    float temp = 0;
    for(int i = 0 ; i < 120 ; i++)
        temp += windspdavg[i];
    temp /= 120.0;
    windspdmph_avg2m = temp;

    //Calc winddir_avg2m, Wind Direction
    //You can't just take the average. Google "mean of circular quantities" for more info
    //We will use the Mitsuta method because it doesn't require trig functions
    //And because it sounds cool.
    //Based on: http://abelian.org/vlf/bearings.html
    //Based on: http://stackoverflow.com/questions/1813483/averaging-angles-again
    long sum = winddiravg[0];
    int D = winddiravg[0];
    for(int i = 1 ; i < WIND_DIR_AVG_SIZE ; i++)
    {
        int delta = winddiravg[i] - D;

        if(delta < -180)
            D += delta + 360;
        else if(delta > 180)
            D += delta - 360;
        else
            D += delta;

        sum += D;
    }
    winddir_avg2m = sum / WIND_DIR_AVG_SIZE;
    if(winddir_avg2m >= 360) winddir_avg2m -= 360;
    if(winddir_avg2m < 0) winddir_avg2m += 360;

    //Calc windgustmph_10m
    //Calc windgustdir_10m
    //Find the largest windgust in the last 10 minutes
    windgustmph_10m = 0;
    windgustdir_10m = 0;
    //Step through the 10 minutes
    for(int i = 0; i < 10 ; i++)
    {
        if(windgust_10m[i] > windgustmph_10m)
        {
            windgustmph_10m = windgust_10m[i];
            windgustdir_10m = windgustdirection_10m[i];
        }
    }

    //Calc humidity
    humidity = myHumidity.readHumidity();
    //float temp_h = myHumidity.readTemperature();
    //Serial.print(" TempH:");
    //Serial.print(temp_h, 2);

    //Calc tempf from pressure sensor
    tempf = myPressure.readTemp();
    //Serial.print(" TempP:");
    //Serial.print(tempf, 2);

    //Total rainfall for the day is calculated within the interrupt
    //Calculate amount of rainfall for the last 60 minutes
    rainin = 0;
    for(int i = 0 ; i < 60 ; i++)
        rainin += rainHour[i];

    //Calc pressure
    pressure = myPressure.readPressure()/1000;

    //Calc dewptf

    //Calc light level


}


float Station::getTemp(int sensor){
OneWire ds(sensor);


byte data[12];
byte addr[8];

if ( !ds.search(addr)) {
//no more sensors on chain, reset search
ds.reset_search();

return -1000;
}

if ( OneWire::crc8( addr, 7) != addr[7]) {
Serial.println("CRC is not valid!");
return -1001;
}

if ( addr[0] != 0x10 && addr[0] != 0x28) {
Serial.print("Device is not recognized");
return -1002;
}

ds.reset();
ds.select(addr);
ds.write(0x44,1);

byte present = ds.reset();
ds.select(addr);
ds.write(0xBE);


for (int i = 0; i < 9; i++) {
data[i] = ds.read();
}

ds.reset_search();

byte MSB = data[1];
byte LSB = data[0];

float TRead = ((MSB << 8) | LSB);
float Temperature = TRead / 16;

return Temperature;

}



void Station::printWeather()
{
    calcWeather(); //Go calc all the various sensors

    Serial.println();
    Serial.print('@');

    Serial.print("|");
    // Serial.print("$,winddir=");
    Serial.print(winddir);
    Serial.print("|");
    //EM KM/H
    Serial.print(windspeedmph, 2);
    Serial.print("|");
    Serial.print(humidity, 1);
    Serial.print("|");
    // Serial.print(",tempf=");
    Serial.print(tempf, 1);
    Serial.print("|");
    // Serial.print(",rainin=");
    Serial.print(rainin, 2);
    Serial.print("|");
    // Serial.print(",dailyrainin=");
    Serial.print(dailyrainin, 2);
    Serial.print("|");
    // Serial.print(",pressure=");
    Serial.print(pressure, 3);
    Serial.print("|");


    sensors.requestTemperatures();
    sensors.getTempCByIndex(0);
    Serial.print(sensors.getTempCByIndex(0));
    //Serial.print(getTemp(sensorTemp2));
    Serial.print("|");

    Serial.print(sensors.getTempCByIndex(1));
    //Serial.print(getTemp(sensorTemp1));
    Serial.print("|");

    /*

      @ | DIR_VENTO º | VEL_VENTO | UMIDADE | TEMPE | CHUVA_HR | CHUVA_DIA | PRESS | TEMP1 | TEMP2

    */


  //  Serial.print(" ");

  }

#ifndef Station_h
#define Station_h
#include <Arduino.h>
#include "SparkFunMPL3115A2.h" //Pressure sensor - Search "SparkFun MPL3115" and install from Library Manager
#include "SparkFunHTU21D.h" //Humidity sensor - Search "SparkFun HTU21D" and install from Library Manager
#include<OneWire.h>
#include <DallasTemperature.h>




class Station{
public:
  Station(DallasTemperature sensors);
  MPL3115A2 myPressure;
  HTU21D myHumidity;
  void rainIRQ();
  void wspeedIRQ();
  void setupStation();
  void loopStation();
  float get_wind_speed();
  int get_wind_direction();
  void calcWeather();
  float getTemp(int sensor);
  void printWeather();

  int WIND_DIR_AVG_SIZE,ONE_WIRE_BUS;
  volatile unsigned long raintime, rainlast, raininterval, rain, lastWindIRQ;
  long lastSecond,lastWindCheck;
  byte WSPEED;
  byte RAIN ;
  byte STAT1;
  byte STAT2;
  byte WDIR;
  byte seconds; //When it hits 60, increase the current minute
  byte seconds_2m; //Keeps track of the "wind speed/dir avg" over last 2 minutes array of data
  byte minutes; //Keeps track of where we are in various arrays of data
  byte minutes_10m; //Keeps track of where we are in wind gust/dir over last 10 minutes array of data
  byte windClicks = 0;
  byte windspdavg[120];
  int winddiravg[120]; //120 ints to keep track of 2 minute average
  float windgust_10m[10]; //10 floats to keep track of 10 minute max
  int windgustdirection_10m[10]; //10 ints to keep track of 10 minute max
  volatile float rainHour[60]; //60 floating numbers to keep track of 60 minutes of rain


  int winddir ;
  float windspeedmph ;
  float windgustmph ;
  int windgustdir ;
  float windspdmph_avg2m ;
  int winddir_avg2m ;
  float windgustmph_10m ;
  int windgustdir_10m ;
  float humidity ;
  float tempf ;
  float rainin;
  volatile float dailyrainin;
  float pressure;
  DallasTemperature sensors;



};
#endif

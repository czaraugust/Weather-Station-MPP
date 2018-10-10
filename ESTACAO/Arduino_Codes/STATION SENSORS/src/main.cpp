#include  <Arduino.h>
#include  <Wire.h>
#include  "Station.h"
#include <RTClib.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_ADS1015.h>
#define ONE_WIRE_BUS 24
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
Station station(sensors);


Adafruit_ADS1115 ads(0X48);  /* Use this for the 16-bit version */
float factor = 0.0078125;

const int chipSelect = 4;

File dataFile;

int anterior =0;
int minutoanterior = 0;

RTC_DS1307 rtc;
//DateTime now = rtc.now();
int day;
int month;
int year;
int hour;
int minutes;
int seconds;
int dayOfWeek;
String weekDay;
char WeekDays[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};


String getTime() {
  DateTime now = rtc.now();
  String time = "";
  now = rtc.now();
  day = now.day();
  month = now.month();
  year = now.year();
  dayOfWeek = now.dayOfTheWeek();
  weekDay = WeekDays[dayOfWeek];
  hour = now.hour();
  minutes = now.minute();
  seconds = now.second();

  time = day;
  time += "/";
  time +=month;
  time += "/";
  time += year;
  time += "/";
  time += " ";
  time += hour;
  time += ":";
  time += minutes;
  time += ":";
  time += seconds;
  time += " ";
  return time;

}

void isrRain(){
  station.rainIRQ();
}

void isrSpeed(){

  station.wspeedIRQ();
}


String getData(){
  String data = getTime();
  data += station.printWeather();
  data += String(ads.readADC_Differential_2_3()*factor/(1.69/100),5);


  return data;

}



void setup(){
  ads.setGain(GAIN_SIXTEEN);
  ads.begin();

    Serial.begin(115200);

    station.setupStation();


    attachInterrupt(0, isrRain , FALLING);
    attachInterrupt(1, isrSpeed , FALLING);
    interrupts();

    if (! rtc.begin()) { // SE O RTC NÃO FOR INICIALIZADO, FAZ
      Serial.println("DS1307 não encontrado"); //IMPRIME O TEXTO NO MONITOR SERIAL
      while(1); //SEMPRE ENTRE NO LOOP
    }
    if (rtc.isrunning()) { //SE RTC NÃO ESTIVER SENDO EXECUTADO, FAZ
      Serial.println("DS1307 rodando!"); //IMPRIME O TEXTO NO MONITOR SERIAL
      //REMOVA O COMENTÁRIO DE UMA DAS LINHAS ABAIXO PARA INSERIR AS INFORMAÇÕES ATUALIZADAS EM SEU RTC
      //rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //CAPTURA A DATA E HORA EM QUE O SKETCH É COMPILADO
      //rtc.adjust(DateTime(2018, 8, 29, 17, 35, 00)); //(ANO), (MÊS), (DIA), (HORA), (MINUTOS), (SEGUNDOS)
    }
    pinMode(SS, OUTPUT);
    if (!SD.begin(52)) {
      Serial.println("Card failed, or not present");
      // don't do anything more:
      while (1) ;
    }
    getTime();
    String path = "";
    path += day;
    path += "_";
    path += month;
    // path += "_";
    // path += year;
    path += ".csv";
    dataFile = SD.open(path, FILE_WRITE);
    dataFile.println("DIA/MES/ANO HORA:MINUTO:SEGUNDO|DIR VENTO|VEL VENTO|UMIDADE|TEMP AMB|CHUVA HR|CHUVA DIA|PRESSAO|TEMP PAINEL 1| TEMP PAINEL 2|IRRADIANCIA");
    dataFile.flush();
    dataFile.close();
    anterior = seconds;
    minutoanterior = minutes;
}

void loop() {

  station.loopStation();
  getTime();
  if (seconds !=  anterior){
    String data =getData();

    anterior = seconds;


    String path = "";
    path += day;
    path += "_";
    path += month;
    path += ".csv";
    dataFile = SD.open(path, FILE_WRITE);
    dataFile.println(data);
    dataFile.flush();
    dataFile.close();
    Serial.println(data);Serial.println(" ");



  }
}

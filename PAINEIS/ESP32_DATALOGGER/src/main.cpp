// ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
// ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
// ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
// ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
// ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
// ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV
#include <Arduino.h>
//#include <WiFiClient.h>
#include <Wire.h>
#include <RTClib.h>
#include <SPI.h>
//#include "FS.h"
#include <SPI.h>
#include <SD.h> //INSTALL 161


#include <Adafruit_ADS1015.h>

// const char* ssid = "EASY_CISCO";
// const char* password = "tv123456";
// //const char* mqtt_server = "test.mosquitto.org";
// const char* mqtt_server = "192.168.0.100";
// const char* mainTopic = "transdutores";


//WiFiClient espClient;
//PubSubClient client(espClient);


RTC_DS1307 rtc;
int day;
int month;
int year;
int hour;
int minutes;
int seconds;
int dayOfWeek;
String weekDay;
char WeekDays[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
const int chipSelect = 17;


Adafruit_ADS1115 ads(0X48);  /* Use this for the 16-bit version */
float_t factor = 0.18750;


int anterior =0;
int minutoanterior = 0;

// void setup_wifi() {
//   Serial.println();
//   Serial.print("Connecting to ");
//   Serial.println(ssid);
//   delay(100);
//
//   WiFi.mode(WIFI_STA);
//   WiFi.begin(ssid, password);
//   while (WiFi.status() != WL_CONNECTED) {
//       delay(500);
//       Serial.print(".");
//   }
//
//
//   // We start by connecting to a WiFi network
//
//
//   int connRes = WiFi.waitForConnectResult();
//   Serial.print ( "connRes: " );
//   Serial.println ( connRes );
//     Serial.println("Ready");
//     Serial.print("IP address: ");
//     Serial.println(WiFi.localIP());
//
//
// }
//
// void callback(char* topic, byte* payload, unsigned int length) {
//
// }
//
// void reconnect() {
//
//   if (WiFi.status() != WL_CONNECTED){
//     Serial.println("Rec wifi");
//     //ESP.restart();
//     setup_wifi();
//   }
//   else{
//     Serial.println("WIFI OK!");
//   }
//   // Loop until we're reconnected
//   if (!client.connected()) {
//     Serial.print("Attempting MQTT connection...");
//     // Create a random client ID
//     String clientId = "ESP32";
//     //clientId += String(WiFi.macAddress(), HEX);
//     // Attempt to connect
//     if (client.connect(clientId.c_str())) {
//       Serial.println("connected");
//       // Once connected, publish an announcement...
//       client.publish(mainTopic, "HI!");
//       // ... and resubscribe
//       client.subscribe("devicesub");
//     } else {
//       Serial.print("failed, rc=");
//       Serial.print(client.state());
//       Serial.println(" try again in 5 mseconds");
//       // Wait 5 seconds before retrying
//       //delay(100);
//     }
//   }
// }






DateTime now = rtc.now();
String getTime() {
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


  // time = day;
  // time += "/";
  // time +=month;
  // time += "/";
  // time += year;
  // time += "/";
  // time += " ";
  time += hour;
  time += ":";
  time += minutes;
  time += ":";
  time += seconds;
  time += " ";
  return time;



}

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void createDir(fs::FS &fs, const char * path){
    Serial.printf("Creating Dir: %s\n", path);
    if(fs.mkdir(path)){
        Serial.println("Dir created");
    } else {
        Serial.println("mkdir failed");
    }
}

void removeDir(fs::FS &fs, const char * path){
    Serial.printf("Removing Dir: %s\n", path);
    if(fs.rmdir(path)){
        Serial.println("Dir removed");
    } else {
        Serial.println("rmdir failed");
    }
}

void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    //Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message)){
        //Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
    file.close();
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("File renamed");
    } else {
        Serial.println("Rename failed");
    }
}

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\n", path);
    if(fs.remove(path)){
        Serial.println("File deleted");
    } else {
        Serial.println("Delete failed");
    }
}

void testFileIO(fs::FS &fs, const char * path){
    File file = fs.open(path);
    static uint8_t buf[512];
    size_t len = 0;
    uint32_t start = millis();
    uint32_t end = start;
    if(file){
        len = file.size();
        size_t flen = len;
        start = millis();
        while(len){
            size_t toRead = len;
            if(toRead > 512){
                toRead = 512;
            }
            file.read(buf, toRead);
            len -= toRead;
        }
        end = millis() - start;
        Serial.printf("%u bytes read for %u ms\n", flen, end);
        file.close();
    } else {
        Serial.println("Failed to open file for reading");
    }


    file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }

    size_t i;
    start = millis();
    for(i=0; i<2048; i++){
        file.write(buf, 512);
    }
    end = millis() - start;
    Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
    file.close();
}





String getData(){
String  tensao =   String(ads.readADC_Differential_0_1() * 250.00000*factor/1000.00000,5);
String corrente = String(ads.readADC_Differential_2_3() * 2.00000*factor/1000.00000,5);


  String data = getTime();

  data += "|";
  data += tensao;
  data += "|";
  data += corrente;
  data += '\n';
  return data;

}


void setup() {
   Serial.begin(115200);
   //pinMode(SS, OUTPUT);

//
  //setup_wifi();
  // client.setServer(mqtt_server, 1883);
  // client.setCallback(callback);
  delay(1000);

   ads.setGain(GAIN_TWOTHIRDS);

   ads.begin();
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
     // Initialize RTC
     if (! rtc.begin()) { // SE O RTC NÃO FOR INICIALIZADO, FAZ
       Serial.println("DS1307 não encontrado"); //IMPRIME O TEXTO NO MONITOR SERIAL
       while(1); //SEMPRE ENTRE NO LOOP
     }
     if (! rtc.isrunning()) { //SE RTC NÃO ESTIVER SENDO EXECUTADO, FAZ
       Serial.println("DS1307 rodando!"); //IMPRIME O TEXTO NO MONITOR SERIAL
       //REMOVA O COMENTÁRIO DE UMA DAS LINHAS ABAIXO PARA INSERIR AS INFORMAÇÕES ATUALIZADAS EM SEU RTC
       //rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //CAPTURA A DATA E HORA EM QUE O SKETCH É COMPILADO
       //rtc.adjust(DateTime(2019, 2, 6, 17, 35, 00)); //(ANO), (MÊS), (DIA), (HORA), (MINUTOS), (SEGUNDOS)
     }


      if(!SD.begin()){
          Serial.println("Card Mount Failed");
          return;
      }
      uint8_t cardType = SD.cardType();

      if(cardType == CARD_NONE){
          Serial.println("No SD card attached");
          return;
      }

      Serial.print("SD Card Type: ");
      if(cardType == CARD_MMC){
          Serial.println("MMC");
      } else if(cardType == CARD_SD){
          Serial.println("SDSC");
      } else if(cardType == CARD_SDHC){
          Serial.println("SDHC");
      } else {
          Serial.println("UNKNOWN");
      }

      getTime();


      String data = getData();
      String path = "/TRANSDUTORES/";
      path += day;
      path += "_";
      path += month;
      path += ".csv";

      appendFile(SD, path.c_str(), " HORA:MINUTO:SEGUNDO |  TENSAO | CORRENTE \n");
      anterior = seconds;
      minutoanterior = minutes;

 }

void loop(){
// //  ArduinoOTA.handle();
// if (!client.connected()) {
//      Serial.println(client.state());
//      reconnect();
//    }
// client.loop();


String data = getData();
//client.publish(mainTopic, data.c_str());
if (seconds !=  anterior){


    anterior = seconds;
    String path = "/TRANSDUTORES/";
    path += day;
    path += "_";
    path += month;
    path += "_";
    path += year;
    path += ".csv";

    if (hour ==0 && minutes == 0 && seconds == 0 ){
      writeFile(SD, path.c_str(), "HORA:MINUTO:SEGUNDO |  TENSAO | CORRENTE \n");
    }
    appendFile(SD, path.c_str(), data.c_str());



    Serial.println(data);
  }
  if(minutes != minutoanterior ){
    minutoanterior = minutes;
    ///Serial.println("PUBLICOU");
      ///client.publish(mainTopic, data.c_str());
  }
}

#include <Arduino.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>

const char* ssid = "EASY";
const char* password = "tv123456";
const char* mqtt_server = "192.168.0.168";
const char* mainTopic = "station";


WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi() {

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);


  delay(100);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  int connRes = WiFi.waitForConnectResult();
  Serial.print ( "connRes: " );
  Serial.println ( connRes );


  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname("station");

    // No authentication by default
//  ArduinoOTA.setPassword("admin");

    // Password can be set with it's md5 value as well
    // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
    // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

    ArduinoOTA.onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH) {
        type = "sketch";
      } else { // U_SPIFFS
        type = "filesystem";
      }

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    });
    ArduinoOTA.onEnd([]() {
      Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) {
        Serial.println("Auth Failed");
      } else if (error == OTA_BEGIN_ERROR) {
        Serial.println("Begin Failed");
      } else if (error == OTA_CONNECT_ERROR) {
        Serial.println("Connect Failed");
      } else if (error == OTA_RECEIVE_ERROR) {
        Serial.println("Receive Failed");
      } else if (error == OTA_END_ERROR) {
        Serial.println("End Failed");
      }
    });
    ArduinoOTA.begin();
    Serial.println("Ready");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());


}

void callback(char* topic, byte* payload, unsigned int length) {

}

void reconnect() {

  while (WiFi.status() != WL_CONNECTED){
    Serial.println("Rec wifi");
    //ESP.restart();
    setup_wifi();
  }
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    //clientId += String(WiFi.macAddress(), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(mainTopic, "HI!");
      // ... and resubscribe
      client.subscribe("devicesub");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}







void setup() {

  Serial.begin(115200);
  Serial.print ( "INICIO: " );
  //pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output

   setup_wifi();


  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);


  delay(1000);
}

void loop() {

ArduinoOTA.handle();




  if (!client.connected()) {
    reconnect();
  }
  client.loop();





    while (Serial.available() > 0) {

      char incomingByte = Serial.read();
      if (incomingByte == '@'){
        String values = "\0";
          values += Serial.readString();

          values += String(WiFi.RSSI());
          values += '|';

      client.publish(mainTopic, values.c_str());



      }

    }


}

/*
 
*/
#include <WiFi.h>
#include <PubSubClient.h>

const String MQTT_ADDRESS = "";
const char* ssid = "";
const char* password = "";

void setup() {
  Serial.begin(115200);
  wifiConnection();
    
}

void wifiConnection() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // put your main code here, to run repeatedly:

}

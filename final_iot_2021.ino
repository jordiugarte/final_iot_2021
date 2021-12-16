/*
 
*/
#include <WiFi.h>
#include <PubSubClient.h>

const String MQTT_ADDRESS = "";
const char* ssid = "";
const char* password = "";

WiFiClient client;
PubSubClient pubsubClient;

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

void mqttConnection() {
  if (pubsubClient.connect("arduinoClient", "testuser", "testpass")) {
    pubsubClient.publish("outTopic","hello world");
    pubsubClient.subscribe("inTopic");
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
}

void setup() {
  Serial.begin(115200);
  wifiConnection();
    
}

void loop() {
  pubsubClient.loop();
}

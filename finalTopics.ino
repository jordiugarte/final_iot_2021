/*
  Mauricio Ayllon
  Mauricio Bejarano
  Jordi Ugarte
*/

#include <WiFi.h>
#include <PubSubClient.h>
#include <string>

// Replace the next variables with your SSID/Password combination
const char* ssid = "AYLLON 2.4G";
const char* password = "#Theo@AYLLON2021";

const String mqtt_server = "18.158.198.79";//broker.hivemq.com obtener la ip con nslookup
const int mqtt_port = 1883;
const char* mainTopic = "bjmm/main/";
const char* clientId = "Mauri642132137364325476";

char room = '1';

int intensity = 255;
String result="";

bool ledStatus = false;
// Pin BOTON
const int pinButton = 27;
uint8_t buttonPrev;
// Pin LED
const int pinLed = 25;

WiFiClient espClient;
PubSubClient client(espClient);

//TODO FIX
char* sendCommand() {
  String result = room + "|" + String(intensity);
  Serial.println(result);
  int resultSize = result.length();
  char buf[resultSize];
  result.toCharArray(buf, resultSize);
  return buf;
}

void onButtonPressed() {
  uint8_t button = digitalRead(pinButton);
  if (button == LOW && buttonPrev == HIGH) {
    Serial.print("Button pressed: ");
    ledStatus = !ledStatus;
    intensity = ledStatus ? 255 : 0;
    ledcWrite(0, intensity);
    if(intensity==255){
     client.publish(mainTopic, "1|255"); 
    }else{
     client.publish(mainTopic, "1|0");
    }   
    //result=room + "|" + String(intensity);
    //Serial.print(result);
    //Serial.println("||");
    //client.publish(mainTopic, room + "|" + String(intensity));
  }
  buttonPrev = digitalRead(pinButton);
}

void setup() {
  Serial.begin(115200);
  pinMode(pinButton, INPUT_PULLUP);

  ledcSetup(0, 5000, 8);
  ledcAttachPin(pinLed, 0);

  //attachInterrupt(digitalPinToInterrupt(pinButton), ISRbotonPresionado, RISING);
  setup_wifi();
  client.setServer(mqtt_server.c_str(), mqtt_port);
  client.setCallback(callback);

}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
  readCommand(messageTemp);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    //****************************************
    if (client.connect(clientId)) {
      Serial.println("connected");
      // Subscribe
      client.subscribe(mainTopic);
      client.publish(mainTopic, "1|60");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void readCommand(String command) {
  intensity = command.substring(2, command.length()).toInt();
  if (command.charAt(0) == room) {
    ledcWrite(0, intensity);
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  ledStatus = intensity > 0;
  onButtonPressed();
}

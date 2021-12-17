#include <WiFi.h>
#include <PubSubClient.h>

// Replace the next variables with your SSID/Password combination
const char* ssid = "UGARTE YAFFAR";
const char* password = "kenan123";

// Add your MQTT Broker IP address, example:
const String mqtt_server = "18.158.198.79";//broker.hivemq.com obtener la ip con nslookup
const int mqtt_port = 1883;

const char room = '3';
int intensity = 255;
const char* mainTopic = "bjmm/main/";
const char* clientId = "ESP32Client23642132137364325476";

bool botonPresionado = false;
// Pin BOTON
const int pinBoton = 27;
// Pin LED
const int pinLed = 25;
int ledState = LOW;

WiFiClient espClient;
PubSubClient client(espClient);


void IRAM_ATTR ISRbotonPresionado() {
  Serial.println("Boton Presionado");
  botonPresionado = true;
}

void setup() {
  Serial.begin(115200);
  pinMode(pinBoton, INPUT_PULLUP);

  ledcSetup(0, 5000, 8);
  ledcAttachPin(pinLed, 0);

  attachInterrupt(digitalPinToInterrupt(pinBoton), ISRbotonPresionado, RISING);
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

//TODO FIX
const char* initialSetup() {
  String result = room + "|" + intensity;
  return (char*)result.c_str();
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
      client.publish(mainTopic, "3|60");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

String sendCommand() {
  return room + "|" + intensity;
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
}

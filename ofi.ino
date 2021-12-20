#include <WiFi.h>
#include <PubSubClient.h>

// Replace the next variables with your SSID/Password combination
const char* ssid = "AndroidAP1C8F";
const char* password = "gyzi1866";

// Add your MQTT Broker IP address, example:
const String mqtt_server = "18.158.198.79";//broker.hivemq.com obtener la ip con nslookup
const int mqtt_port = 1883;

const char room = '2';
int intensity = 255;
const char* mainTopic = "bjmm/main/";
const char* clientId = "Maveto23642132137364123456";

// Pin BOTON
const int pinBoton = 27;
bool buttonPressed = false;
// Pin LED
const int pinLed = 25;

WiFiClient espClient;
PubSubClient client(espClient);


void IRAM_ATTR ISRbotonPresionado() {
  Serial.println("Boton Presionado");
  if (intensity>0){
    intensity = 0;
  }else{
    intensity = 255;
  }
  buttonPressed = true;
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

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(clientId)) {
      Serial.println("connected");
      // Subscribe
      client.subscribe(mainTopic);
      char payload[6];
      sendCommand(payload);
      client.publish(mainTopic, payload);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void sendCommand(char* buf) {
  String result = String(room) + "|" + String(intensity);
  int resultSize = result.length()+1;
  result.toCharArray(buf, resultSize);
}

void readCommand(String command) {
  if (command.charAt(0) == room) {
    intensity = command.substring(2, command.length()).toInt();
    ledcWrite(0, intensity);
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  if(buttonPressed){
    char payload[6];
    sendCommand(payload);
    client.publish(mainTopic,payload);
    buttonPressed = false;
  }
  
  client.loop();
}

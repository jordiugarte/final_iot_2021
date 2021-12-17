
#include <WiFi.h>
#include <PubSubClient.h>

// Replace the next variables with your SSID/Password combination
const char* ssid = "UGARTE YAFFAR";
const char* password = "kenan123";

// Add your MQTT Broker IP address, example:
const char* mqtt_server = "18.158.198.79";//broker.hivemq.com obtener la ip con nslookup
const int mqtt_port=1883;
const char* publishTopic="casa/portico/luz/estado";
const char* suscribeTopic="casa/portico/luz/cmd";
const char* suscribeTopic2="casa/portico/luz/brillo";
const char* clientId="ESP32Client236421321373643276";

bool botonPresionado = false;
// Pin BOTON
const int pinBoton=27;
// Pin LED
const int pinLed=2;
int ledState=LOW;

WiFiClient espClient;
PubSubClient client(espClient);


void IRAM_ATTR ISRbotonPresionado(){
  Serial.println("Boton Presionado");
  botonPresionado=true;
  }

void setup() {
  Serial.begin(115200); 
  pinMode(pinBoton, INPUT_PULLUP);
  pinMode(pinLed, OUTPUT);
  digitalWrite(pinLed,ledState);
  attachInterrupt(digitalPinToInterrupt(pinBoton), ISRbotonPresionado, RISING);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
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
  int intensity;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32P/output, you check if the message is either "1" or "0". 
  // Changes the output state according to the message
//****************************************
if (String(topic) == suscribeTopic2) {
    Serial.print("Intensity Changed: ");
    Serial.print(messageTemp);
    //intensity=map((int)messageTemp, 0,100, 0, 255);
    //ledcWrite(pinLed,intensity)
  }
//****************************************
if (String(topic) == suscribeTopic) {
    Serial.print("Changing output to ");
    if(messageTemp == "0"){
      Serial.println("Off");
      digitalWrite(pinLed, LOW);
    }
    if(messageTemp == "1"){
      Serial.println("on");
      digitalWrite(pinLed, HIGH);
    }
  if(messageTemp == "2"){
      Serial.print("Sending led status:");
    Serial.println(ledState);
    if(ledState==1){
    client.publish(publishTopic, "1");
      }else{
      client.publish(publishTopic, "0");
    }
    }
  }
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
      client.subscribe(suscribeTopic);
      client.subscribe(suscribeTopic2);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if(botonPresionado==true){
    if(ledState==LOW){
      client.publish(publishTopic, "1");
      ledState=HIGH;
      digitalWrite(pinLed, ledState);
      botonPresionado=false;
      delay(100);
      }
    }
    if(botonPresionado==true){
    if(ledState==HIGH){
      client.publish(publishTopic, "0");
      ledState=LOW;
      digitalWrite(pinLed, ledState);
      botonPresionado=false;
      delay(100);
      }
    }

}

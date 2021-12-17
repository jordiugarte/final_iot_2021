#include <WiFi.h>
#include <PubSubClient.h>

// Replace the next variables with your SSID/Password combination
const char* ssid = "UGARTE YAFFAR";
const char* password = "kenan123";

// Add your MQTT Broker IP address, example:
const String mqtt_server = "18.158.198.79";//broker.hivemq.com obtener la ip con nslookup
const int mqtt_port = 1883;

const char room = '1';
int intensity = 255;
const char* mainTopic = "bjmm/main/";
const char* clientId = "ESP32Client236421321373643276";

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
  pinMode(pinLed, OUTPUT);
  digitalWrite(pinLed, ledState);
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

  //****************************************
  /*sif (String(topic) == mainTopic) {
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
      client.publish(publishTopic.c_str(), "1");
        }else{
        client.publish(publishTopic.c_str(), "0");
      }
      }
    }*/
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
      client.publish(mainTopic, sendCommand().c_str());

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
  if (command.charAt(0) == room) {
    intensity = command.substring(2, command.length()).toInt();
    if (botonPresionado) {
      ledcWrite(pinLed, intensity);
    } else {
      ledcWrite(pinLed, 0);
    }

    /*
        if(botonPresionado==true){
          if(ledState==LOW){
            client.publish(publishTopic.c_str(), "1");
            ledState=HIGH;
            digitalWrite(pinLed, ledState);
            botonPresionado=false;
            delay(100);
            }
          }
        if(botonPresionado==true){
          if(ledState==HIGH){
            client.publish(publishTopic.c_str(), "0");
            ledState=LOW;
            digitalWrite(pinLed, ledState);
            botonPresionado=false;
            delay(100);
            }
          }
      }*/
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  /*
    if(botonPresionado==true){
      if(ledState==LOW){
        client.publish(publishTopic.c_str(), "1");
        ledState=HIGH;
        digitalWrite(pinLed, ledState);
        botonPresionado=false;
        delay(100);
        }
      }
      if(botonPresionado==true){
      if(ledState==HIGH){
        client.publish(publishTopic.c_str(), "0");
        ledState=LOW;
        digitalWrite(pinLed, ledState);
        botonPresionado=false;
        delay(100);
        }
      }*/
}

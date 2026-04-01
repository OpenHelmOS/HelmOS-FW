#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Elisa 15 2,4GHz";
const char* password = "aqws1234";
const char* mqtt_server = "192.168.0.9";

WiFiClient espClient;
PubSubClient client(espClient);

const int LED_PIN = 2; // sisäänrakennettu LED

void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  
  if (String(topic) == "helmos/led/set") {
    if (message == "on") {
      digitalWrite(LED_PIN, HIGH);
      client.publish("helmos/led", "on");
    } else {
      digitalWrite(LED_PIN, LOW);
      client.publish("helmos/led", "off");
    }
  }
}

void setup_wifi() {
  Serial.begin(115200);
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected! IP: " + WiFi.localIP().toString());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("connected!");
      client.subscribe("helmos/led/set");
    } else {
      Serial.println("failed, rc=" + String(client.state()));
      delay(5000);
    }
  }
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

#include <WiFi.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>

#define SENSOR_PIN 18  // ESP32 pin GPIO18 connected to OUT pin of IR obstacle avoidance sensor

const char WIFI_SSID[] = "";
const char WIFI_PASSWORD[] = "";

const char MQTT_BROKER_ADRRESS[] = "";
const int MQTT_PORT = 1883;
const char MQTT_CLIENT_ID[] = "";
const char MQTT_USERNAME[] = "";
const char MQTT_PASSWORD[] = "";

// The MQTT topics that ESP32 should publish/subscribe
const char PUBLISH_TOPIC[] = "";
const char SUBSCRIBE_TOPIC[] = "";

const int PUBLISH_INTERVAL = 5000;  // 5 seconds

WiFiClient network;
MQTTClient mqtt = MQTTClient(256);

unsigned long lastPublishTime = 0;


void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);

  // initialize the Arduino's pin as aninput
  pinMode(SENSOR_PIN, INPUT);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println("ESP32 - Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  connectToMQTT();
}

void loop() {
  mqtt.loop();

  if (millis() - lastPublishTime > PUBLISH_INTERVAL) {
    sendToMQTT();
    lastPublishTime = millis();
  }

  delay(100);
}


void connectToMQTT() {
  // Connect to the MQTT broker
  mqtt.begin(MQTT_BROKER_ADRRESS, MQTT_PORT, network);

  Serial.print("ESP32 - Connecting to MQTT broker");

  while (!mqtt.connect(MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD)) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();

  if (!mqtt.connected()) {
    Serial.println("ESP32 - MQTT broker Timeout!");
    return;
  }

  Serial.println("ESP32 - MQTT broker Connected!");
}

void sendToMQTT() {

  // read the state of the the input pin:
  int state = digitalRead(SENSOR_PIN);

  if (state == LOW)
    Serial.println("The obstacle is present");
  else
    Serial.println("The obstacle is NOT present");

  StaticJsonDocument<200> message;
  message["timestamp"] = millis();
  message["binary_sensor"] = state;
  char messageBuffer[512];
  serializeJson(message, messageBuffer);

  mqtt.publish(PUBLISH_TOPIC, messageBuffer);

  Serial.println("ESP32 - sent to MQTT:");
  Serial.print("- topic: ");
  Serial.println(PUBLISH_TOPIC);
  Serial.print("- payload:");
  Serial.println(messageBuffer);
}


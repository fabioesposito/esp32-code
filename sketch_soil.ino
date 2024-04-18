#include <WiFi.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>

#define AOUT_PIN 36 // ESP32 pin GPIO36 (ADC0) that connects to AOUT pin of moisture sensor (VP pin)
#define THRESHOLD 2800 // CHANGE YOUR THRESHOLD HERE

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
  pinMode(AOUT_PIN, INPUT);

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

  int value = analogRead(AOUT_PIN); // read the analog value from sensor

  if (value > THRESHOLD)
    Serial.print("The soil is DRY (");
  else
    Serial.print("The soil is WET (");

  Serial.print(value);
  Serial.println(")");


  delay(500);

  StaticJsonDocument<200> message;
  message["timestamp"] = millis();
  message["sensor"] = value;
  char messageBuffer[512];
  serializeJson(message, messageBuffer);

  mqtt.publish(PUBLISH_TOPIC, messageBuffer);

  Serial.println("ESP32 - sent to MQTT:");
  Serial.print("- topic: ");
  Serial.println(PUBLISH_TOPIC);
  Serial.print("- payload:");
  Serial.println(messageBuffer);
}


#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char *SENSOR_NAME = "<REPLACE-ME>";
const int SENSOR_PIN = 16;

const char *WIFI_SSID = "<REPLACE-ME>";
const char *WIFI_PASSWORD = "<REPLACE-ME>";

const char *MQTT_BROKER = "<REPLACE-ME>";
const char *MQTT_CLIENT_ID = SENSOR_NAME;
const char *MQTT_TOPIC = "esp32/sensors/";
const char *MQTT_USER = "<REPLACE-ME>";
const char *MQTT_PASS = "<REPLACE-ME>";
const int MQTT_PORT = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Set PIN mode
  pinMode(SENSOR_PIN, INPUT);

  Serial.println("STARTING...");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  Serial.println(WiFi.localIP());
  Serial.println("WIFI OK");

  client.setServer(MQTT_BROKER, MQTT_PORT);
  while (!client.connected()) {
    if (!client.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASS)) {      
      Serial.print("Can't connect to MQTT:");
      Serial.print(client.state());
      delay(2000);
    }
  }
  Serial.println("MQTT OK");
}

void loop() {
  client.loop();
  readAndSendData();
  delay(2000);
}

void readAndSendData() {
  // example read 
  // int value = analogRead(AOUT_PIN); 
  int state = digitalRead(SENSOR_PIN);

  JsonDocument doc;
  doc["sensor"] = SENSOR_NAME;
  doc["timestamp"] = millis();
  doc["data"] = state;

  char docBuffer[512];
  serializeJson(doc, docBuffer);
  
  char topic[30] = {0};
  strcat(topic, MQTT_TOPIC);
  strcat(topic, SENSOR_NAME);

  Serial.println(topic);
  client.publish(topic, docBuffer);
  Serial.println("SENT");

}

#define BLYNK_TEMPLATE_ID "TMPL6Sal62wCJ"
#define BLYNK_TEMPLATE_NAME "Soil"
#define BLYNK_AUTH_TOKEN "axxQvp8nDq5am20rEsvoldFJWvl4mGza"
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "DHT.h"
BlynkTimer timer;
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
//#include <SimpleTimer.h>
#define DHTTYPE DHT11
#define dht_dpin D4
DHT dht(dht_dpin, DHTTYPE); 
float t;
float h;
char auth[] = "axxQvp8nDq5am20rEsvoldFJWvl4mGza";
const char* ssid = "T303";
const char* password = "daothienkhong";
const char* mqtt_server = "test.mosquitto.org";
const int sensor_pin = A0;	/* Soil moisture sensor O/P pin */
int cm;
void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

}
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  
  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload);
  cm = doc["Water Level"];
}
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe("ESP/SensorData");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);	/* Define baud rate for serial communication */
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  pinMode(D2, OUTPUT);
  pinMode(D1, OUTPUT);
  dht.begin();
  Blynk.begin(auth, ssid, password);
  timer.setInterval(100L, sendUptime);
}
void sendUptime()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature(); 
  Serial.println("Humidity and temperature\n\n");
  Serial.print("Current humidity = ");
  Serial.print(h);
  Serial.print("%  ");
  Serial.print("temperature = ");
  Serial.print(t); 
  Blynk.virtualWrite(V2, t);
  Blynk.virtualWrite(V3, h);
  float moisture_percentage;
  int sensor_analog;
  sensor_analog = analogRead(sensor_pin);
  moisture_percentage = ( 100 - ( (sensor_analog/1023.00) * 100 ) );
  Blynk.virtualWrite(V1, moisture_percentage);
}
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  delay(5000);
  float h = dht.readHumidity();
  float t = dht.readTemperature(); 
  Serial.println("Humidity and temperature\n\n");
  Serial.print("Current humidity = ");
  Serial.print(h);
  Serial.println("%  ");
  Serial.print("temperature = ");
  Serial.println(t); 
  float moisture_percentage;
  int sensor_analog;
  sensor_analog = analogRead(sensor_pin);
  moisture_percentage = ( 100 - ( (sensor_analog/1023.00) * 100 ) );
  Serial.print("Moisture Percentage = ");
  Serial.print(moisture_percentage);
  Serial.print("%\n\n");
  unsigned long now = millis();
  Serial.print("Water Level: ");
  Serial.println(cm);
  if (moisture_percentage <=15 && cm >= 100)
  {
  
    digitalWrite(D2, HIGH);
    digitalWrite(D1, HIGH);
    
  } 
   else {
    digitalWrite(D2, LOW);
    digitalWrite(D1, LOW);
  }
  delay(1000);
  Blynk.run();
  timer.run();
}
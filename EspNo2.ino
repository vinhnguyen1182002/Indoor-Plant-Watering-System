#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h> 
const char* ssid = "T303";
const char* password = "daothienkhong";
const char* mqtt_server = "test.mosquitto.org";
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int val = 0 ;

 void setup()  
 {  
   Serial.begin(9600); // sensor buart rate  
   setup_wifi();
   client.setServer(mqtt_server, 1883);
   client.setCallback(callback);
 }  
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

 void loop()   
 {  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  int s1=analogRead(A0); // Water Level Sensor output pin connected A0  
  int cm = map(s1, 0, 500 , 0, 400);
  Serial.println(cm);  // See the Value In Serial Monitor  
  delay(1000);
  unsigned long now = millis();
    if (now - lastMsg > 5000) 
    {
      lastMsg = now;
      
      Serial.print("Water Level (cm): ");
      Serial.print(cm);
      Serial.print("\n\n");
      
      StaticJsonDocument<200> doc;
      doc["Water Level"] = cm;
      serializeJson(doc, msg);
      
      Serial.print("Publish message: ");
      Serial.println(msg);
      Serial.println("");
      client.publish("ESP/SensorData", msg);
    }   
  
  }

  void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  }

  void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
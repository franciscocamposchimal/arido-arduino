#include <WiFi.h>
#include <PubSubClient.h>
// Conexion al wifi
const char* ssid = "Pineaple-mansion";
const char* password = "holdthedoor";
// ip del broker
const char* mqtt_server = "104.248.59.64";
// se crea un cliente de tipo WiFi y se le asigna a la instancia de la lib PubSub
WiFiClient espClient;
PubSubClient client(espClient);
// Variable que lleva el conteo para el envio de mensajes al broker por tiempo
long lastMsg = 0;

void setup() {
  // inicializamos el serial
  Serial.begin(115200);
  // conecta al wifi
  setup_wifi();
  // conecta al server
  client.setServer(mqtt_server, 1883);
  // inicia el ;istener para mensajes entrantes
  client.setCallback(callback);
}
// rutina para conectar al wifi
void setup_wifi() {
  delay(10);
  // connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected...");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
// rutina para obtener mensajes entrante e imprimirlos en el serial
void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    // Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }

  Serial.print(messageTemp);
  Serial.println();

}
// rutina para conectarse y verificar su estado
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/incoming");
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
  // verifica si estamos conectados al broker
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  // rutina de envio de mensajes cada 5 sec
  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
    client.publish("esp32/messages", "Hi from esp32!!");
  }
  
}

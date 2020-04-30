#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "SSID WIFI KAMU";
const char* password = "PASSWORD WIFI KAMU";

const char* mqtt_server = "IP PUBLIK INSTANCE KAMU WAKTU INSTALL MQTT BROKER";

int soilSensorPin = A0;
int mesinPompaPin = 15;
int lampuMerahPin = 25;
int lampuKuningPin = 26;
int lampuHijauPin = 27; 
int nilai;
int i = 0;

#define COMMAND_TOPIC    "actuator/machine/"
#define SOIL_TOPIC    "sensor/soil/"

/* create an instance of PubSubClient client */
WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[100];
char msg2[100];


void receivedCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received: ");
  Serial.println(topic);
  String messageTemp;


  Serial.print("payload: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    messageTemp += (char)payload[i];
  }
  Serial.println();
  if (String(topic) == "actuator/machine/") {
    Serial.print("Changing output to ");
    if(messageTemp == "0"){
      Serial.println("on");
      digitalWrite(mesinPompaPin, 0);
    }
    else if(messageTemp == "1"){
      Serial.println("off");
      digitalWrite(mesinPompaPin, 1);
    }
  }

}

void mqttconnect() {
  /* Loop until reconnected */
  while (!client.connected()) {
    Serial.print("MQTT connecting ...");
    /* connect now */
    if (client.connect("CLIENT ID BEBAS","USERNAME MQTT BROKER","PASSWORD MQTT BROKER")) {
      Serial.println("connected");
      /* subscribe topic with default QoS 0*/
      client.subscribe(SOIL_TOPIC);
      client.subscribe(COMMAND_TOPIC);
    } else {
      Serial.print("failed, status code =");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      /* Wait 5 seconds before retrying */
      delay(5000);
    }
  }
}

//Dalam relay jika HIGH maka mati jika LOW maka hidup

void setup() {
  Serial.begin(115200);
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
  /* configure the MQTT server with IPaddress and port */
  client.setServer(mqtt_server, 1883);

  client.setCallback(receivedCallback);
  
  pinMode(lampuHijauPin, OUTPUT);
  pinMode(lampuKuningPin, OUTPUT);
  pinMode(lampuMerahPin, OUTPUT);
  pinMode(mesinPompaPin, OUTPUT);
}

void loop() {
   if (!client.connected()) {
    mqttconnect();
  }
  client.loop();
  long now = millis();
  int nilai = analogRead(soilSensorPin);
  int relay_status = digitalRead(mesinPompaPin);
  
  if (now - lastMsg > 2000) {
      lastMsg = now;
      client.publish(SOIL_TOPIC, msg);
        if (!isnan(nilai)) {
           if(nilai <= 350){
            sprintf (msg, "Tanah Basah, Nilai : %d", nilai, nilai);
              digitalWrite(lampuHijauPin, HIGH);
              digitalWrite(lampuKuningPin, LOW);
              digitalWrite(lampuMerahPin, LOW);
           }else if(nilai <= 700){
            sprintf (msg, "Kelembapan Tanah Normal, Nilai : %d", nilai, nilai);
              digitalWrite(lampuHijauPin, LOW);
              digitalWrite(lampuKuningPin, HIGH);
              digitalWrite(lampuMerahPin, LOW);    
           }else if(nilai > 700){
            sprintf (msg, "Tanah kering, Nilai : %d", nilai, nilai);
              digitalWrite(lampuHijauPin, LOW);
              digitalWrite(lampuKuningPin, LOW);
              digitalWrite(lampuMerahPin, HIGH);
           }
        }
            if(i<1){
             client.publish(COMMAND_TOPIC, msg2);
              sprintf (msg2, "%d", relay_status, relay_status); 
            }
            i++;
    }
      
}

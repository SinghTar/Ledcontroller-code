#include <WiFi.h>
#include <PubSubClient.h>
#include <FastLED.h>

const char* ssid = "xxxxxxxxxxx";                 // wifi naam
const char* password = "xxxxxxxxxxxx";            // wifi wachtwoord
const char* mqtt_server  = "xxxxxxxxxxxxx";       // broker ip
const int mqtt_port  = 1883;
const char* mqttUser = "xxxxxxxxxxxxxxxx";        // broker username
const char* mqttPassword = "xxxxxxxxxxxxx";       // broker wachtwoord

#define NUM_LEDS 3
#define NUM_STRIPS 3
#define DATA_PIN1 25
#define DATA_PIN2 26
#define DATA_PIN3 27
CRGB leds[NUM_STRIPS][NUM_LEDS];

WiFiClient espClient;
PubSubClient client(espClient);

String TOPIC;
String bericht;

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
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32Client";
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqttUser, mqttPassword)) {
      Serial.println("connected");
      client.subscribe("Farmlab/1/licht/aanUitKnop/level/1");
      client.subscribe("Farmlab/1/licht/aanUitKnop/level/2");
      client.subscribe("Farmlab/1/licht/aanUitKnop/level/3");
      client.subscribe("Farmlab/1/plantKeuze/level/1");
      client.subscribe("Farmlab/1/plantKeuze/level/2");
      client.subscribe("Farmlab/1/plantKeuze/level/3");
      client.subscribe("Farmlab/1/kleurKiezer/level/1");
      client.subscribe("Farmlab/1/kleurKiezer/level/2");
      client.subscribe("Farmlab/1/kleurKiezer/level/3");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}


void callback(String topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  bericht = "";
  TOPIC = topic;
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    bericht += (char)message[i];
  }
  Serial.println();
}


void setup() {
  Serial.begin(115200);
  FastLED.addLeds<WS2812, DATA_PIN1, RGB>(leds[0], NUM_LEDS);
  FastLED.addLeds<WS2812, DATA_PIN2, RGB>(leds[1], NUM_LEDS);
  FastLED.addLeds<WS2812, DATA_PIN3, RGB>(leds[2], NUM_LEDS);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  reconnect();
}


void loop() {
  client.loop();
  if (TOPIC == "Farmlab/1/licht/aanUitKnop/level/1" || TOPIC == "Farmlab/1/licht/aanUitKnop/level/2" || TOPIC == "Farmlab/1/licht/aanUitKnop/level/3") {
    byte level = ((TOPIC[TOPIC.length() - 1]) - '0');
    switch (bericht.toInt()) {
      case 1:
        ledBesturing(255, 255, 255, level);
        break;
      case 0:
        ledBesturing(0, 0, 0, level);
        break;
    }
    //Serial.println(level);
  }
  if (TOPIC == "Farmlab/1/plantKeuze/level/1" || TOPIC == "Farmlab/1/plantKeuze/level/2" || TOPIC == "Farmlab/1/plantKeuze/level/3") {
    byte level = ((TOPIC[TOPIC.length() - 1]) - '0');
    switch (bericht.toInt()) {
      case 0:
        ledBesturing(0, 148, 211, level); // darkviolet
        break;
      case 1:
        ledBesturing(0, 75, 130, level);  //indigo
        break;
      case 2:
        ledBesturing(0, 139, 139, level); // dark magenta
        break;
      case 3:
        ledBesturing(0, 255, 0, level); // groen
        break;
      case 4:
        ledBesturing(0, 0, 255, level); // rood
        break;
    }
  }
  if (TOPIC == "Farmlab/1/kleurKiezer/level/1" || TOPIC == "Farmlab/1/kleurKiezer/level/2" || TOPIC == "Farmlab/1/kleurKiezer/level/3") {
    byte level = ((TOPIC[TOPIC.length() - 1]) - '0');
    bericht = bericht.substring(bericht.indexOf('(') + 1);
    bericht = bericht.substring(0, bericht.indexOf(')'));
    byte commaIndex = bericht.indexOf(',');
    byte tweedeCommaIndex = bericht.indexOf(',', commaIndex + 1);
    String groen = bericht.substring(0, commaIndex);
    String rood = bericht.substring(commaIndex + 1, tweedeCommaIndex);
    String blauw = bericht.substring(tweedeCommaIndex + 1);
    ledBesturing(rood.toInt(), groen.toInt(), blauw.toInt(), level);
  }
}

void ledBesturing(byte groen, byte rood, byte blauw, byte level) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[level - 1][i] = CRGB(groen, rood, blauw);
  }
  FastLED.show();
}

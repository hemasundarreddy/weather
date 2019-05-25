#include <ESP8266WiFi.h>
#include <PubSubClient.h>

////////////
#include "DHT.h"  
#define DHTPIN D2   
#define DHTTYPE DHT11   
DHT dht (DHTPIN, DHTTYPE);

float temperature;
int humidity;
String data3;
String data="";


void callback(char* topic, byte* payload, unsigned int payloadLength);

// CHANGE TO YOUR WIFI CREDENTIALS
const char* ssid = "YOU BROADBAND1S2";
const char* password = "SBJNTU1S2";


// CHANGE TO YOUR DEVICE CREDENTIALS AS PER IN IBM BLUMIX

#define ORG "dwgpht"
#define DEVICE_TYPE "nodemcu-iot"
#define DEVICE_ID "22222"
#define TOKEN "z(f-dBuljQvmAmsAv2"  //  Authentication Token OF THE DEVICE

//  PIN DECLARATIONS 

//-------- Customise the above values --------
const char publishTopic[] = "iot-2/evt/sensorsdata/fmt/json";
char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char topic[] = "iot-2/cmd/data/fmt/String";// cmd  REPRESENT command type AND COMMAND IS TEST OF FORMAT STRING
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;


WiFiClient wifiClient;
PubSubClient client(server, 1883, callback, wifiClient);

int publishInterval = 5000; // 30 seconds
long lastPublishMillis;
void publishData();

void setup() {
  
  Serial.begin(9600);
  Serial.println();
  dht.begin(); 
  wifiConnect();
  mqttConnect();
}

void loop() {


 if (millis() - lastPublishMillis > publishInterval)
  {
    publishData();
    lastPublishMillis = millis();
  }
  
  if (!client.loop()) {
    mqttConnect();
  }
}

void wifiConnect() {
  Serial.print("Connecting to "); Serial.print(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("WiFi connected, IP address: "); 
  Serial.println(WiFi.localIP());
}

void mqttConnect() {
  if (!client.connected()) 
  {
    Serial.print("Reconnecting MQTT client to "); Serial.println(server);
    while (!client.connect(clientId, authMethod, token)) {
      Serial.print(".");
      delay(500);
    }
    
    initManagedDevice();
    Serial.println();
  }
}

void initManagedDevice() {
  if (client.subscribe(topic)) {
   
    Serial.println("subscribe to cmd OK");
  } else {
    Serial.println("subscribe to cmd FAILED");
  }
}

void callback(char* topic, byte* payload, unsigned int payloadLength) {
  
  Serial.print("callback invoked for topic: ");
  Serial.println(topic);

  for (int i = 0; i < payloadLength; i++) {
    
    data3 += (char)payload[i];
  }
  
  Serial.print("data: "+ data3);
  control_func();
  data3 = "";
}

void control_func()
{
  
   
  if(data3 == "on")
 {
    Serial.println("......on command have been subscribed..........");
    
  }
  else if(data3 == "off")
  {
    Serial.println("......off command have been subscribed..........");
  }

  
  else
  {
    Serial.println("......no commands have been subscribed..........");
   }
}

void publishData() 
{
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

   if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  int moisture = analogRead(A0);
  
  String payload = "{\"d\":{\"temperature\":";
  payload += temperature;
  payload += ",""\"humidity\":";
  payload +=  humidity;
  payload += ",""\"moisturevalue\":";
  payload +=  moisture;
  
  payload += "}}";


  Serial.print("\n");
  Serial.print("Sending payload: "); Serial.println(payload);

  if (client.publish(publishTopic, (char*) payload.c_str())) {
    Serial.println("Publish OK");
  } else {
    Serial.println("Publish FAILED");
  }
}

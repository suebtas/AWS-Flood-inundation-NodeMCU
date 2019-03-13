#include <ESP8266WiFi.h>
#include <AmazonIOTClient.h>
#include <ESP8266AWSImplementations.h>
#include <AWSFoundationalTypes.h>
#include <DHT.h>

Esp8266HttpClient httpClient;
Esp8266DateTimeProvider dateTimeProvider;

AmazonIOTClient iotClient;
ActionError actionError;

// DHT Sensor setup
#define DHTPIN D1
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

char *ssid="home";
char *password="0813060904";

void setup() {
  Serial.begin(115200);
  delay(10);

  // LED on nodemcu
  pinMode(BUILTIN_LED, OUTPUT);

  // DHT Sensor
  dht.begin();

  // Connect to WAP
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

  iotClient.setAWSRegion("ap-southeast-1");
  iotClient.setAWSEndpoint("amazonaws.com");
  iotClient.setAWSDomain(""); //YOUR REST THING API ENDPOINT
  iotClient.setAWSPath("");                                //YOUR THING SHADOW PATH
  iotClient.setAWSKeyID("");                                 //YOUR ACCESS KEY ID
  iotClient.setAWSSecretKey("");        //YOUR SECRET ACCESS KEY
  iotClient.setHttpClient(&httpClient);
  iotClient.setDateTimeProvider(&dateTimeProvider);
}

void loop(){
  float t = dht.readTemperature(true);
  char temp[10];
  dtostrf(t,1,2,temp);
 
  float h = dht.readHumidity();
  char hum[10];
  dtostrf(h,1,2,hum);

  MinimalString shadow = ("{\"state\":{\"reported\":{\"temperature\":" + String(temp) + ",\"humidity\":" + String(hum) + "}}}").c_str();

  char* result = iotClient.update_shadow(shadow, actionError);
  Serial.print(result);

  delay(60000);
}

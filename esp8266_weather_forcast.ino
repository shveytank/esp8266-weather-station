#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Arduino_JSON.h>

const char* ssid = "your wifi ssid";
const char* password = "your wifi password";
String openWeatherMapApiKey = "your API Key";
String city = "your city";
String countryCode = "country code";

unsigned long lastTime = 0;
unsigned long timerDelay = 10000;

String jsonBuffer;

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 
  Serial.println("Timer set to 10 seconds (timerDelay variable), it will take 10 seconds before publishing the first reading.");
}
int temp;
int pres;
int hum;
String wind;
void loop() {
  if ((millis() - lastTime) > timerDelay) {
    if(WiFi.status()== WL_CONNECTED){
      String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey;
      
      jsonBuffer = httpGETRequest(serverPath.c_str());
      Serial.println(jsonBuffer);
      JSONVar myObject = JSON.parse(jsonBuffer);
      if (JSON.typeof(myObject) == "undefined") {
        Serial.println("Parsing input failed!");
        return;
      }
      String jsonString = JSON.stringify(myObject["main"]["temp"]);
      temp=jsonString.toInt();
      temp=(temp-273);
      jsonString = JSON.stringify(myObject["main"]["pressure"]);
      pres=jsonString.toInt();
      jsonString = JSON.stringify(myObject["main"]["humidity"]);
      hum=jsonString.toInt();
      jsonString = JSON.stringify(myObject["wind"]["speed"]);
      wind=jsonString;
      Serial.println();
      Serial.print("JSON object = ");
      Serial.println(myObject);
      Serial.print("Temperature: ");
      Serial.println(temp);
      Serial.print("Pressure: ");
      Serial.println(pres);
      Serial.print("Humidity: ");
      Serial.println(hum);
      Serial.print("Wind Speed: ");
      Serial.println(wind);
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}

String httpGETRequest(const char* serverName) {
  HTTPClient http; 
  http.begin(serverName);
  
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  http.end();

  return payload;
}

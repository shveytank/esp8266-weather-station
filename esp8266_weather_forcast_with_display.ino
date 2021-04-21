#include <Adafruit_GFX.h>      // include Adafruit graphics library
#include <Adafruit_ST7735.h>   // include Adafruit ST7735 TFT library
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Arduino_JSON.h>
// ST7735 TFT module connections
#define TFT_RST   D4     // TFT RST pin is connected to NodeMCU pin D4 (GPIO2)
#define TFT_CS    D3     // TFT CS  pin is connected to NodeMCU pin D4 (GPIO0)
#define TFT_DC    D2     // TFT DC  pin is connected to NodeMCU pin D4 (GPIO4)

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

const char* ssid = "your wifi ssid";
const char* password = "your wifi password";
String openWeatherMapApiKey = "your API Key";
String city = "your city";
String countryCode = "country code";

unsigned long lastTime = 0;
unsigned long timerDelay = 10000;
String jsonBuffer;

void setup(void) {
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
  
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
 tft.setRotation(1);
  uint16_t time = millis();
  tft.fillScreen(ST7735_BLACK);
  time = millis() - time;
  
  delay(500);
 
  tft.fillScreen(ST7735_BLACK);
    //layout
    tft.drawFastHLine(0, 0, tft.width(), ST7735_WHITE);
    tft.drawFastHLine(1, 127, tft.width(), ST7735_WHITE);
    tft.drawFastVLine(0, 0, tft.height(), ST7735_WHITE);
    tft.drawFastVLine(159, 0, tft.height(), ST7735_WHITE);
    tft.drawFastVLine(80, 50, 40, ST7735_WHITE);
    tft.drawFastHLine(1, 50, tft.width(), ST7735_WHITE);
    tft.drawFastHLine(1, 90, tft.width(), ST7735_WHITE);
    tft.setTextColor(ST7735_WHITE);
    tft.setCursor(10,10);
    tft.setTextSize(2);
    tft.print(city);
    tft.setTextSize(1);
    tft.setCursor(10,35);
    tft.print(countryCode);
    tft.setTextSize(1);
    
    tft.setTextColor(ST7735_RED);
    tft.setCursor(15,60);
    tft.print("Pressure");
    tft.setCursor(90,60);
    tft.print("Humidity");
    tft.setCursor(10,105);
    tft.print("Wind Speed");
    tft.setTextColor(ST7735_GREEN);
    tft.setCursor(140,10);
    tft.print(" C");
    tft.setTextColor(ST7735_YELLOW);
    tft.setCursor(42,75);
    tft.print(" hpa");
    tft.setCursor(115,75);
    tft.print("%");
    tft.setCursor(115,105);
    tft.print("m/s");
  delay(1000);
 
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
      
      tft.fillRect(105, 8 , 38, 30, ST7735_BLACK);
      tft.fillRect(12, 70 , 32, 15, ST7735_BLACK);
      tft.fillRect(97, 70 , 16, 15, ST7735_BLACK);
      tft.fillRect(87, 103 , 25, 15, ST7735_BLACK);
      tft.setTextColor(ST7735_GREEN);
      tft.setTextSize(3);
      tft.setCursor(108,10);
      tft.print(temp);
      tft.setTextColor(ST7735_YELLOW);
      tft.setTextSize(1);
      tft.setCursor(15,75);
      tft.print(pres);
      tft.setCursor(100,75);
      tft.print(hum);
      tft.setCursor(90,105);
      tft.print(wind);
    
    
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

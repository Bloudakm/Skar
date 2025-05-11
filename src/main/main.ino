#include <WiFi.h>
#include <HTTPClient.h>
#include <TFT_eSPI.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <XPT2046_Touchscreen.h> // Only if using touchscreen

#include "user_secrets.h"

// --- Display Setup ---
TFT_eSPI tft = TFT_eSPI();
#define TOUCH_CS  21
#define TOUCH_IRQ 39
XPT2046_Touchscreen ts(TOUCH_CS, TOUCH_IRQ);

// --- State ---
bool showDetails = false;
unsigned long lastTouchTime = 0;
int temperature = 0;
int cloudiness = 0;
int humidity = 0;
String description = "";
String icon = "";

// --- Colors ---
uint16_t tempToColor(int tempC) {
  if (tempC <= 0) return tft.color565(0, 100, 255);  // blue
  if (tempC <= 10) return tft.color565(0, 200, 255);
  if (tempC <= 20) return tft.color565(0, 255, 150);
  if (tempC <= 30) return tft.color565(255, 200, 0);
  return tft.color565(255, 0, 0);  // red
}

void connectWiFi() {
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void fetchWeather() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "http://api.openweathermap.org/data/2.5/weather?q=";
           url += String(CITY) + "," + String(COUNTRY);
           url += "&units=metric&appid=" + String(API_KEY);
    http.begin(url);
    int httpCode = http.GET();
    if (httpCode > 0) {
      String payload = http.getString();
      StaticJsonDocument<1024> doc;
      DeserializationError err = deserializeJson(doc, payload);
      if (!err) {
        temperature = int(doc["main"]["temp"]);
        cloudiness = int(doc["clouds"]["all"]);
        humidity = int(doc["main"]["humidity"]);
        description = String(doc["weather"][0]["description"].as<const char*>());
      }
    }
    http.end();
  }
}

void drawFaceScreen() {
  tft.fillScreen(TFT_BLACK);
  uint16_t eyeColor = tempToColor(temperature);

  // Eyes
  int eyeRadius = 20;
  tft.fillCircle(80, 120, eyeRadius, eyeColor);
  tft.fillCircle(240, 120, eyeRadius, eyeColor);

  // Eyebrows = rectangles that grow based on cloudiness
  int browWidth = map(cloudiness, 0, 100, 0, 80);
  tft.fillRect(80 - browWidth / 2, 80, browWidth, 8, TFT_WHITE);
  tft.fillRect(240 - browWidth / 2, 80, browWidth, 8, TFT_WHITE);
}

void drawDetailsScreen() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);

  tft.setCursor(10, 30);
  tft.printf("Temp: %d C\n", temperature);
  tft.setCursor(10, 70);
  tft.printf("Clouds: %d %%\n", cloudiness);
  tft.setCursor(10, 110);
  tft.printf("Humidity: %d %%\n", humidity);
  tft.setCursor(10, 150);
  tft.printf("Sky: %s", description.c_str());
}

void setup() {
  Serial.begin(115200);
  tft.init();
  tft.setRotation(1); // adjust if needed
  ts.begin();
  ts.setRotation(1);
  connectWiFi();
  fetchWeather();
  drawFaceScreen();
}

void loop() {
  if (ts.touched()) {
    TS_Point p = ts.getPoint();
    // Simple debounce
    if (millis() - lastTouchTime > 800) {
      showDetails = !showDetails;
      if (showDetails) {
        drawDetailsScreen();
      } else {
        drawFaceScreen();
      }
      lastTouchTime = millis();
    }
  }
}
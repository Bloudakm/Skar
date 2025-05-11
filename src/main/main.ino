#include <WiFi.h>
#include <HTTPClient.h>
#include <TFT_eSPI.h>
#include "SPIFFS.h"
#include "FS.h"
#include <ArduinoJson.h>
#include <SPI.h>

#include "secrets.h"

#define CALIBRATION_FILE "/TouchCalData"
#define REPEAT_CAL false

// --- Display Setup ---
TFT_eSPI tft = TFT_eSPI();

// --- State ---
bool showDetails = false;
unsigned long lastTouchTime = 0;
int temperature = 0;
int cloudiness = 0;
int humidity = 0;
String description = "";
String icon = "";

void touch_calibrate() {
  uint16_t calData[5];
  uint8_t calDataOK = 0;

  // Starting SPIFFS file system
  if (!SPIFFS.begin()) {
    //Serial.println("Formatting file system");
    SPIFFS.format();
    SPIFFS.begin();
  }

  // Check if calibration already exists 
  if (SPIFFS.exists(CALIBRATION_FILE)) {
    if (REPEAT_CAL) {
      // Repeat calibration in case user wants to 
      SPIFFS.remove(CALIBRATION_FILE);
    } else {
      // Load last calibration file
      fs::File f = SPIFFS.open(CALIBRATION_FILE, "r");
      if (f) {
        if (f.readBytes((char *)calData, 14) == 14)
          calDataOK = 1;
        f.close();
      }
    }
  }

  if (calDataOK && !REPEAT_CAL) {
    // Set calibration for display
    tft.setTouch(calData);
  } else {
    // In case of faulty calibration file repeat it
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(20, 0);
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.println("Touch corners as indicated");

    tft.setTextFont(1);
    tft.println();

    if (REPEAT_CAL) {
      tft.setTextColor(TFT_RED, TFT_BLACK);
      tft.println("Set REPEAT_CAL to false to stop this running again!");
    }
    // During the calibration the user has to click on an arrow in each corner
    tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.println("Calibration complete!");

    // Saving calibration file into SPIFFS
    fs::File f = SPIFFS.open(CALIBRATION_FILE, "w");
    if (f) {
      f.write((const unsigned char *)calData, 14);
      f.close();
    }
  }
}

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

  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  } 

  tft.init();
  tft.setRotation(1); // adjust if needed
  tft.fillScreen(TFT_BLACK);
  tft.setTextFont(2);

  touch_calibrate();
  tft.begin();

  connectWiFi();
  fetchWeather();
  drawFaceScreen();
}

void loop() {
  uint16_t x, y;
  if (tft.getTouch(&x, &y)) {
    if (millis() - lastTouchTime > 800) {
      showDetails = !showDetails;
      showDetails ? drawDetailsScreen() : drawFaceScreen();
      lastTouchTime = millis();
    }
  }
}
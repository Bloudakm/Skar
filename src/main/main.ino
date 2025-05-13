#include <ArduinoJson.h>
#include "FS.h"
#include <HTTPClient.h>
#include "SPIFFS.h"
#include <SPI.h>
#include <TFT_eSPI.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#include "secrets.h"
#include "icons.h"

const char *rootCACertificate = R"string_literal(
-----BEGIN CERTIFICATE-----
MIIF3jCCA8agAwIBAgIQAf1tMPyjylGoG7xkDjUDLTANBgkqhkiG9w0BAQwFADCB
iDELMAkGA1UEBhMCVVMxEzARBgNVBAgTCk5ldyBKZXJzZXkxFDASBgNVBAcTC0pl
cnNleSBDaXR5MR4wHAYDVQQKExVUaGUgVVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNV
BAMTJVVTRVJUcnVzdCBSU0EgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwHhcNMTAw
MjAxMDAwMDAwWhcNMzgwMTE4MjM1OTU5WjCBiDELMAkGA1UEBhMCVVMxEzARBgNV
BAgTCk5ldyBKZXJzZXkxFDASBgNVBAcTC0plcnNleSBDaXR5MR4wHAYDVQQKExVU
aGUgVVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNVBAMTJVVTRVJUcnVzdCBSU0EgQ2Vy
dGlmaWNhdGlvbiBBdXRob3JpdHkwggIiMA0GCSqGSIb3DQEBAQUAA4ICDwAwggIK
AoICAQCAEmUXNg7D2wiz0KxXDXbtzSfTTK1Qg2HiqiBNCS1kCdzOiZ/MPans9s/B
3PHTsdZ7NygRK0faOca8Ohm0X6a9fZ2jY0K2dvKpOyuR+OJv0OwWIJAJPuLodMkY
tJHUYmTbf6MG8YgYapAiPLz+E/CHFHv25B+O1ORRxhFnRghRy4YUVD+8M/5+bJz/
Fp0YvVGONaanZshyZ9shZrHUm3gDwFA66Mzw3LyeTP6vBZY1H1dat//O+T23LLb2
VN3I5xI6Ta5MirdcmrS3ID3KfyI0rn47aGYBROcBTkZTmzNg95S+UzeQc0PzMsNT
79uq/nROacdrjGCT3sTHDN/hMq7MkztReJVni+49Vv4M0GkPGw/zJSZrM233bkf6
c0Plfg6lZrEpfDKEY1WJxA3Bk1QwGROs0303p+tdOmw1XNtB1xLaqUkL39iAigmT
Yo61Zs8liM2EuLE/pDkP2QKe6xJMlXzzawWpXhaDzLhn4ugTncxbgtNMs+1b/97l
c6wjOy0AvzVVdAlJ2ElYGn+SNuZRkg7zJn0cTRe8yexDJtC/QV9AqURE9JnnV4ee
UB9XVKg+/XRjL7FQZQnmWEIuQxpMtPAlR1n6BB6T1CZGSlCBst6+eLf8ZxXhyVeE
Hg9j1uliutZfVS7qXMYoCAQlObgOK6nyTJccBz8NUvXt7y+CDwIDAQABo0IwQDAd
BgNVHQ4EFgQUU3m/WqorSs9UgOHYm8Cd8rIDZsswDgYDVR0PAQH/BAQDAgEGMA8G
A1UdEwEB/wQFMAMBAf8wDQYJKoZIhvcNAQEMBQADggIBAFzUfA3P9wF9QZllDHPF
Up/L+M+ZBn8b2kMVn54CVVeWFPFSPCeHlCjtHzoBN6J2/FNQwISbxmtOuowhT6KO
VWKR82kV2LyI48SqC/3vqOlLVSoGIG1VeCkZ7l8wXEskEVX/JJpuXior7gtNn3/3
ATiUFJVDBwn7YKnuHKsSjKCaXqeYalltiz8I+8jRRa8YFWSQEg9zKC7F4iRO/Fjs
8PRF/iKz6y+O0tlFYQXBl2+odnKPi4w2r78NBc5xjeambx9spnFixdjQg3IM8WcR
iQycE0xyNN+81XHfqnHd4blsjDwSXWXavVcStkNr/+XeTWYRUc+ZruwXtuhxkYze
Sf7dNXGiFSeUHM9h4ya7b6NnJSFd5t0dCy5oGzuCr+yDZ4XUmFF0sbmZgIn/f3gZ
XHlKYC6SQK5MNyosycdiyA5d9zZbyuAlJQG03RoHnHcAP9Dc1ew91Pq7P8yF1m9/
qS3fuQL39ZeatTXaw2ewh0qpKJ4jjv9cJ2vhsE/zB+4ALtRZh8tSQZXq9EfX7mRB
VXyNWQKV3WKdwrnuWih0hKWbt5DHDAff9Yk2dDLWKMGwsAvgnEzDHNb842m1R0aB
L6KCq9NjRHDEjf8tM7qtj3u1cIiuPhnPQCjY/MiQu12ZIvVS5ljFH4gxQ+6IHdfG
jjxDah2nGN59PRbxYvnKkKj9
-----END CERTIFICATE-----)string_literal";

#define CALIBRATION_FILE "/TouchCalData"
#define REPEAT_CAL false

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

#define EYE_X 60
#define EYE_Y 90
#define EYE_WIDTH 90
#define EYE_HEIGHT 60
#define EYE_SPACING 20

// --- Display Setup ---
TFT_eSPI tft = TFT_eSPI();

// --- State ---
bool showDetails = false;
unsigned long lastTouchTime = 0;
unsigned long lastAPICall = 0;
unsigned long lastFrame = 0;
unsigned long currentMillis = 0;

int blinkingPeriod = 4000;

int temperature = 0;
int cloudiness = 0;
int humidity = 0;
String description = "";
String icon = "";

enum EyeState {BLINK_GROW, BLINK_SHRINK, MOVE, IDLE};
EyeState eyeState = IDLE;

struct Eye {
  int x, y, targetX, targetY, w, h;
  int tearY = 0;
  void reset(int i) {
    x = EYE_X;
    y = EYE_Y;
    targetX = EYE_X + i*(EYE_WIDTH+EYE_SPACING);
    targetY = EYE_Y;
    w = EYE_WIDTH;
    h = EYE_HEIGHT;
  }
};

int eyeColor;

unsigned long lastMove = 1000;
unsigned long lastBlink = 2000;
unsigned int speed = 20;
int speedX = 0;
int speedY = 0;

// SETTING the eyes parameters
Eye eyes[2];

// Store forecast data
#define MAX_FORECAST 4
struct Forecast {
  String time;
  String condition;
  int temp;
};
Forecast forecast[MAX_FORECAST];

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
int tempToColor(int tempC) {
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
    WiFiClientSecure *client = new WiFiClientSecure;
    if(client) {
      client->setCACert(rootCACertificate);
      // Beware these parantheses ensure client is destroyed only after https
      {
        HTTPClient https;

        String url = "https://api.openweathermap.org/data/2.5/forecast?q=";
              url += String(CITY) + "," + String(COUNTRY);
              url += "&units=metric&appid=" + String(API_KEY);
        
        if(https.begin(*client, url)) {
          int httpCode = https.GET();
          if (httpCode > 0) {
            String payload = https.getString();

            Serial.println("Request payload: ");
            Serial.println(payload);

            DynamicJsonDocument doc(8192);
            deserializeJson(doc, payload);

            for (int i = 0; i < MAX_FORECAST; i++) {
              String timestamp = doc["list"][i]["dt_txt"].as<String>();
              forecast[i].time = timestamp.substring(11, 16);
              forecast[i].temp = int(doc["list"][i]["main"]["temp"].as<float>());
              forecast[i].condition = doc["list"][i]["weather"][0]["main"].as<String>();
            }
          }

          https.end();
        } else {
          Serial.println("HTTPS Unable to connect\n");
        }
      }

      delete client;
    } else {
      Serial.println("Unable to create client");
    }
  }
}

void eyesBlink() {
  Serial.println("blinkin");
  tft.fillScreen(TFT_BLACK);

  for(int i = 0; i < 2; i++) {
    if(eyeState == BLINK_SHRINK) {
      if(eyes[i].h > 10) {
        eyes[i].h -= 5;
      } else {
        eyeState = BLINK_GROW;
        eyes[i].h += 5;
      }
    } else if(eyeState == BLINK_GROW) {
      if(eyes[i].h < EYE_HEIGHT) {
        eyes[i].h += 5;
      } else {
        eyeState = IDLE;
        eyes[i].reset(i);
        lastBlink = currentMillis;
      }
    }

    tft.fillRoundRect(eyes[i].x, eyes[i].y-3*(60-eyes[i].h), eyes[i].w, eyes[i].h, 10, eyeColor);
  }
}

void eyesMove() {
  bool stillMoving = false;
  tft.fillScreen(TFT_BLACK);

  for (int i = 0; i < 2; i++) {
    int dx = eyes[i].targetX - eyes[i].x;
    int dy = eyes[i].targetY - eyes[i].y;

    // Smooth easing
    eyes[i].x += dx * 0.2;
    eyes[i].y += dy * 0.2;

    // Check if more movement is needed
    if (abs(dx) > 1 || abs(dy) > 1) stillMoving = true;

    tft.fillRoundRect(eyes[i].x, eyes[i].y, eyes[i].w, eyes[i].h, 10, eyeColor);
  }

  if (!stillMoving) {
    for (int i = 0; i < 2; i++) eyes[i].reset(i);
    eyeState = IDLE;
    lastMove = currentMillis;
  }
}


void drawMainScreen() {
  Serial.println("Main screen");
  tft.fillScreen(TFT_BLACK);

  // Color of eye
  eyeColor = tempToColor(forecast[0].temp);

  // Eyes
  for (int i = 0; i < 2; i++) {
    eyes[i].reset(i);
  }
  
  
  tft.fillRoundRect(176, 78, 108, 98, 10, 0x77E0);
  tft.fillEllipse(230, 126, 9, 41, 0x0);
  tft.fillRoundRect(46, 78, 108, 98, 10, 0x77E0);
  tft.fillEllipse(107, 126, 9, 41, 0x0);
}

void drawDetails() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);

  for (int i = 0; i < MAX_FORECAST; i++) {
    int x = 10;
    int y = 10 + i * 55;
    tft.setCursor(x, y);
    tft.print(forecast[i].time);
    tft.setCursor(x + 100, y);
    tft.print(forecast[i].temp);
    tft.print(" C");
    drawWeatherIcon(forecast[i].condition, x + 200, y);
  }
}

void drawWeatherIcon(String condition, int x, int y) {
  if (condition == "Clear") {
    tft.fillCircle(x, y, 10, TFT_YELLOW);
  } else if (condition == "Clouds") {
    tft.fillRoundRect(x - 10, y - 5, 20, 10, 3, TFT_LIGHTGREY);
  } else if (condition == "Rain") {
    tft.fillRoundRect(x - 10, y - 5, 20, 10, 3, TFT_LIGHTGREY);
    tft.fillCircle(x - 5, y + 8, 2, TFT_BLUE);
    tft.fillCircle(x + 5, y + 8, 2, TFT_BLUE);
  } else {
    tft.drawXBitmap(x, y, rain_bits, 16, 16, TFT_WHITE); // fallback
  }
}

int createRandom(int min, int max) {
  int rand = random(min, max);

  if(rand < (max/2) && rand > (min/2)) {
    return createRandom(min, max);
  } else {
    Serial.print("Random number: ");
    Serial.println(rand);
    return rand;
  }
}

int clamp(int value, int minVal, int maxVal) {
  return max(minVal, min(value, maxVal));
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting app");

  pinMode(16, OUTPUT);

  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  } 

  Serial.println("SPIFFS mounted");

  tft.init();
  tft.setRotation(3); // adjust if needed
  tft.fillScreen(TFT_BLACK);
  tft.setTextFont(2);
  digitalWrite(16, HIGH);

  Serial.println("TFT initialized");

  touch_calibrate();

  Serial.println("Touch calibrated");

  tft.begin();

  

  randomSeed(analogRead(0)); // Seed randomness

  connectWiFi();
  fetchWeather();
  drawMainScreen();
}

void loop() {
  currentMillis = millis();
  uint16_t x, y;
  if (tft.getTouch(&x, &y)) {
    if (currentMillis - lastTouchTime > 800) {
      showDetails = !showDetails;
      if(showDetails) {
        eyeState = IDLE;

        drawDetails();
      } else {
        drawMainScreen();
      }
      lastTouchTime = currentMillis;
    }
  }

  if (!showDetails && currentMillis - lastFrame < 0) {
    if(currentMillis-lastBlink > blinkingPeriod && eyeState == IDLE) {
      eyeState = BLINK_SHRINK;
    } else if (currentMillis-lastMove > random(2000, 3000) && eyeState == IDLE) {
      int randomX = createRandom(-40, 40);
      int randomY = createRandom(-20, 20);

      // Set new target, but clamp it to screen bounds
      for (int i = 0; i < 2; i++) {
        eyes[i].targetX = clamp(eyes[i].x + randomX, 0, SCREEN_WIDTH - EYE_WIDTH);
        eyes[i].targetY = clamp(eyes[i].y + randomY, 0, SCREEN_HEIGHT - EYE_HEIGHT);
      }

      eyeState = MOVE;
    }

    switch (eyeState){
      case BLINK_SHRINK:
      case BLINK_GROW:
        eyesBlink();
        break;
      case MOVE:
        eyesMove();
        break;
      default:
        break;
    }

    lastFrame = currentMillis;
    //Serial.println(String(eyes));
  }

  // update information every 10 minutes
  if(currentMillis - lastAPICall > (10*60*1000) && eyeState == IDLE) {
    fetchWeather();
    showDetails ? drawDetails() : drawMainScreen();
    lastAPICall = currentMillis;
  }
}
# SKAR: A Robotic Desk Companion

**Skar** is a minimalist desk assistant built on the ESP32 and a 3.2" ILI9341 TFT LCD. It visually communicates live weather data using robotic "eyes" and expressive features — designed to feel more like a companion than a display.

---

## 💡 Features

- 👁️ Expressive robot eyes that change color with temperature
- ☁️ Eyebrows indicate cloudiness (subtly animated)
- 🌡️ Tap-to-toggle screen for full weather breakdown (temp, clouds, humidity, condition)
- 🔁 Eyes subtly pulse using animation for a more "alive" effect
- 📱 Weather data pulled from OpenWeatherMap API

---

## 🔧 Hardware Requirements

- ESP32 Dev Board  
- 3.2" ILI9341 TFT LCD display (240x320)  
- Touch support (built-in to the TFT and enabled via TFT_eSPI)  
- Wi-Fi access  

---

## 📁 Folder Structure

---

## 📦 `secrets.h` Example

Make sure to create a `secrets.h` file in the src/main/ folder:

```cpp
#define SSID = "YOUR_WIFI_SSID"
#define PASSWORD = "YOUR_WIFI_PASSWORD"
#define API_KEY = "YOUR_OPENWEATHER_API_KEY"
#define CITY = "YOUR_CITY"
#define COUNTRY = "YOUR_COUNTRY";// e.g., "US"

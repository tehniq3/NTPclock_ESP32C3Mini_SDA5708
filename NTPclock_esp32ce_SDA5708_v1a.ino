// program conceput de Nicu FLORICA (niq_ro) folosind AI
// ver.1 - added base info from open-meteo site
//----------------------------------------------
// ver.1a - added degree symbol, 15s for clock, 3s for other info
//----------------------------------------------

#include <WiFi.h>
#include <time.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// --- Configurare Wi-Fi ---
const char* ssid = "bbk2";
const char* password = "internet2";

// --- Configurare Meteo (Coordonate) --- Craiova
const float LATITUDE = 44.3174;
const float LONGITUDE = 23.7974;

// --- Configurare Fus Orar Romania ---
const char* ntpServer = "pool.ntp.org";
const char* timezoneInfo = "EET-2EEST,M3.5.0/3,M10.5.0/3";

// --- Configurare Intensitate ---
#define USE_POTENTIOMETER 0 
const int POT_PIN = 2; 
int currentBrightness = 0; 

// --- Pini SDA5708 ---
const int LOAD_PIN = 6;
const int DATA_PIN = 4;
const int CLK_PIN  = 5;

// --- Variabile Meteo ---
float temperature = 0.0;
int humidity = 0;
int pressure_mmHg = 0; // Presiunea in mmHg
int weather_code = 0;
String weather_desc = " senin  ";
unsigned long lastWeatherUpdate = 0;
const long weatherInterval = 600000; // 10 minute

// --- Variabile Ciclare Ecran ---
int displayMode = 0; 
unsigned long lastDisplayChange = 0;

// --- Fontul complet 5x7 (95 caractere: Space -> ~, include litere mici) ---
// Am modificat caracterul ^ (circumflex) pentru a fi folosit ca simbolul de grad (°)
const unsigned char font[752] PROGMEM = {
   0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000, // Space
   0b00100000,0b00100000,0b00100000,0b00100000,0b00100000,0b00000000,0b00100000, // !
   0b01010000,0b01010000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000, // "
   0b01010000,0b01010000,0b11111000,0b01010000,0b11111000,0b01010000,0b01010000, // #
   0b00100000,0b01111000,0b10100000,0b01110000,0b00101000,0b00110000,0b00100000, // $    
   0b11000000,0b11001000,0b00010000,0b00100000,0b01000000,0b10011000,0b00011000, // %
   0b01000000,0b10100000,0b01000000,0b10100000,0b10010000,0b10001000,0b01110000, // &
   0b00010000,0b00010000,0b00100000,0b00000000,0b00000000,0b00000000,0b00000000, // '
   0b00100000,0b01000000,0b01000000,0b01000000,0b01000000,0b01000000,0b00100000, // (
   0b00010000,0b00001000,0b00001000,0b00001000,0b00001000,0b00001000,0b00010000, // )
   0b00000000,0b10001000,0b01010000,0b11111000,0b01010000,0b10001000,0b00000000, // *
   0b00000000,0b00100000,0b00100000,0b11111000,0b00100000,0b00100000,0b00000000, // +
   0b00000000,0b00000000,0b00000000,0b00000000,0b01000000,0b01000000,0b10000000, // ,
   0b00000000,0b00000000,0b00000000,0b11111000,0b00000000,0b00000000,0b00000000, // -
   0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b11000000,0b11000000, // .
   0b00000000,0b00001000,0b00010000,0b00100000,0b01000000,0b10000000,0b00000000, // /
   0b01110000,0b10001000,0b10011000,0b10101000,0b11001000,0b10001000,0b01110000, // 0
   0b00100000,0b01100000,0b00100000,0b00100000,0b00100000,0b00100000,0b01110000, // 1
   0b01110000,0b00001000,0b00001000,0b01110000,0b10000000,0b10000000,0b11111000, // 2
   0b11110000,0b00001000,0b00001000,0b01110000,0b00001000,0b00001000,0b11110000, // 3
   0b00001000,0b00011000,0b00101000,0b01001000,0b11111000,0b00001000,0b00001000, // 4
   0b11111000,0b10000000,0b10000000,0b11110000,0b00001000,0b10001000,0b01110000, // 5
   0b01110000,0b10000000,0b10000000,0b11110000,0b10001000,0b10001000,0b01110000, // 6
   0b11111000,0b00001000,0b00001000,0b00010000,0b00100000,0b01000000,0b10000000, // 7
   0b01110000,0b10001000,0b10001000,0b01110000,0b10001000,0b10001000,0b01110000, // 8
   0b01110000,0b10001000,0b10001000,0b01111000,0b00010000,0b00100000,0b01000000, // 9
   0b00000000,0b00000000,0b01100000,0b01100000,0b00000000,0b01100000,0b01100000, // :
   0b00000000,0b00000000,0b01100000,0b01100000,0b00000000,0b00100000,0b01000000, // ;
   0b00010000,0b00100000,0b01000000,0b10000000,0b01000000,0b00100000,0b00010000, // <
   0b00000000,0b00000000,0b11111000,0b00000000,0b11111000,0b00000000,0b00000000, // =
   0b01000000,0b00100000,0b00010000,0b00001000,0b00010000,0b00100000,0b01000000, // >
   0b01110000,0b10001000,0b00001000,0b00110000,0b01000000,0b00000000,0b01000000, // ?
   0b01110000,0b10001000,0b10111000,0b10101000,0b10111000,0b10000000,0b01111000, // @
   0b00100000,0b01010000,0b10001000,0b11111000,0b10001000,0b10001000,0b10001000, // A
   0b11110000,0b10001000,0b10001000,0b11110000,0b10001000,0b10001000,0b11110000, // B
   0b01110000,0b10001000,0b10000000,0b10000000,0b10000000,0b10001000,0b01110000, // C
   0b11110000,0b10001000,0b10001000,0b10001000,0b10001000,0b10001000,0b11110000, // D
   0b11111000,0b10000000,0b10000000,0b11110000,0b10000000,0b10000000,0b11111000, // E
   0b11111000,0b10000000,0b10000000,0b11110000,0b10000000,0b10000000,0b10000000, // F
   0b01110000,0b10001000,0b10000000,0b10000000,0b10011000,0b10001000,0b01111000, // G
   0b10001000,0b10001000,0b10001000,0b11111000,0b10001000,0b10001000,0b10001000, // H
   0b01110000,0b00100000,0b00100000,0b00100000,0b00100000,0b00100000,0b01110000, // I
   0b11111000,0b00010000,0b00010000,0b00010000,0b10010000,0b10010000,0b01100000, // J
   0b10001000,0b10010000,0b10100000,0b11000000,0b10100000,0b10010000,0b10001000, // K
   0b10000000,0b10000000,0b10000000,0b10000000,0b10000000,0b10000000,0b11111000, // L
   0b10001000,0b11011000,0b10101000,0b10001000,0b10001000,0b10001000,0b10001000, // M
   0b10001000,0b11001000,0b10101000,0b10011000,0b10001000,0b10001000,0b10001000, // N
   0b01110000,0b10001000,0b10001000,0b10001000,0b10001000,0b10001000,0b01110000, // O
   0b11110000,0b10001000,0b10001000,0b11110000,0b10000000,0b10000000,0b10000000, // P
   0b01110000,0b10001000,0b10001000,0b10001000,0b10101000,0b10011000,0b01111000, // Q
   0b11110000,0b10001000,0b10001000,0b11110000,0b10100000,0b10010000,0b10001000, // R
   0b01111000,0b10000000,0b10000000,0b01110000,0b00001000,0b00001000,0b11110000, // S
   0b11111000,0b00100000,0b00100000,0b00100000,0b00100000,0b00100000,0b00100000, // T
   0b10001000,0b10001000,0b10001000,0b10001000,0b10001000,0b10001000,0b01110000, // U
   0b10001000,0b10001000,0b10001000,0b10001000,0b10001000,0b01010000,0b00100000, // V
   0b10001000,0b10001000,0b10001000,0b10001000,0b10101000,0b11011000,0b10001000, // W
   0b10001000,0b10001000,0b01010000,0b00100000,0b01010000,0b10001000,0b10001000, // X
   0b10001000,0b10001000,0b10001000,0b01010000,0b00100000,0b00100000,0b00100000, // Y
   0b11111000,0b00001000,0b00010000,0b00100000,0b01000000,0b10000000,0b11111000, // Z
   0b11100000,0b10000000,0b10000000,0b10000000,0b10000000,0b10000000,0b11100000, // [
   0b00000000,0b10000000,0b01000000,0b00100000,0b00010000,0b00001000,0b00000000, // backslash
   0b00111000,0b00001000,0b00001000,0b00001000,0b00001000,0b00001000,0b00111000, // ]
   0b01100000,0b10010000,0b01100000,0b00000000,0b00000000,0b00000000,0b00000000, // ^ (MODIFICAT: Simbol Grad °)
   0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b11111000, // _
   0b00010000,0b00010000,0b00001000,0b00000000,0b00000000,0b00000000,0b00000000, // '
   0b00000000,0b00000000,0b01110000,0b10001000,0b10001000,0b10011000,0b01101000, // a
   0b10000000,0b10000000,0b11110000,0b10001000,0b10001000,0b10001000,0b11110000, // b
   0b00000000,0b00000000,0b01111000,0b10000000,0b10000000,0b10000000,0b01111000, // c
   0b00001000,0b00001000,0b01111000,0b10001000,0b10001000,0b10001000,0b01111000, // d
   0b00000000,0b00000000,0b01110000,0b10001000,0b11111000,0b10000000,0b01111000, // e
   0b00010000,0b00101000,0b01110000,0b00100000,0b00100000,0b00100000,0b00100000, // f
   0b00000000,0b00000000,0b01110000,0b10001000,0b01111000,0b00001000,0b01110000, // g
   0b10000000,0b10000000,0b11110000,0b10001000,0b10001000,0b10001000,0b10001000, // h
   0b00100000,0b00000000,0b00100000,0b00100000,0b00100000,0b00100000,0b00100000, // i
   0b00100000,0b00000000,0b00100000,0b00100000,0b00100000,0b10100000,0b01000000, // j
   0b10000000,0b10000000,0b10001000,0b10010000,0b10100000,0b11010000,0b10001000, // k
   0b00100000,0b00100000,0b00100000,0b00100000,0b00100000,0b00100000,0b00100000, // l
   0b00000000,0b00000000,0b11010000,0b10101000,0b10101000,0b10101000,0b10101000, // m
   0b00000000,0b00000000,0b10110000,0b11001000,0b10001000,0b10001000,0b10001000, // n
   0b00000000,0b00000000,0b01110000,0b10001000,0b10001000,0b10001000,0b01110000, // o
   0b00000000,0b00000000,0b11110000,0b10001000,0b11110000,0b10000000,0b10000000, // p
   0b00000000,0b00000000,0b01111000,0b10001000,0b01111000,0b00001000,0b00001000, // q
   0b00000000,0b00000000,0b10110000,0b11001000,0b10000000,0b10000000,0b10000000, // r
   0b00000000,0b00000000,0b01111000,0b10000000,0b01110000,0b00001000,0b11110000, // s
   0b00100000,0b00100000,0b01110000,0b00100000,0b00100000,0b00100000,0b00110000, // t
   0b00000000,0b00000000,0b10001000,0b10001000,0b10001000,0b10011000,0b01101000, // u
   0b00000000,0b00000000,0b10001000,0b10001000,0b10001000,0b01010000,0b00100000, // v
   0b00000000,0b00000000,0b10001000,0b10001000,0b10101000,0b10101000,0b01010000, // w
   0b00000000,0b00000000,0b10001000,0b01010000,0b00100000,0b01010000,0b10001000, // x
   0b00000000,0b00000000,0b10001000,0b10001000,0b11111000,0b00001000,0b01110000, // y
   0b00000000,0b00000000,0b11111000,0b00010000,0b00100000,0b01000000,0b11111000, // z
   0b00100000,0b01000000,0b01000000,0b10000000,0b01000000,0b01000000,0b00100000, // {
   0b00100000,0b00100000,0b00100000,0b00000000,0b00100000,0b00100000,0b00100000, // |
   0b00100000,0b00010000,0b00010000,0b00001000,0b00010000,0b00010000,0b00100000, // }
   0b01010000,0b10100000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000  // ~
};

// ==========================================
// DRIVER SDA5708
// ==========================================
void init_SDA5708() { digitalWrite(LOAD_PIN, HIGH); }

void send_byte_to_SDA5708(uint8_t byte_val) {
  digitalWrite(LOAD_PIN, LOW);
  for (uint8_t x = 0; x <= 7; x++) {
    digitalWrite(DATA_PIN, (byte_val >> x) & 1);
    digitalWrite(CLK_PIN, HIGH); delayMicroseconds(1);
    digitalWrite(CLK_PIN, LOW); delayMicroseconds(1);
  }
  digitalWrite(LOAD_PIN, HIGH);
}

void brightness_SDA5708(uint8_t val) {
  if (val != currentBrightness) {
    send_byte_to_SDA5708(0b11100000 | (val & 0b00000111));
    currentBrightness = val;
  }
}

void digit_to_SDA5708(uint8_t sign, uint8_t digit) {
  if ((sign < 0x20) || (sign > 0x7E)) sign = 0x20; 
  if (digit > 7) digit = 0;
  send_byte_to_SDA5708(0b10100000 | digit);
  for(uint8_t i = 0; i < 7; i++) {
    send_byte_to_SDA5708(font[(sign - 0x20) * 7 + i] / 8);
  }
}

void print2display(String text) {
  uint8_t cursor = 0;
  for (unsigned int p = 0; p < text.length(); p++) {
    if (cursor > 7) break; 
    digit_to_SDA5708(text.charAt(p), cursor);
    cursor++;
  }
}

// ==========================================
// FUNCTII WI-FI & NTP
// ==========================================
void connectToWiFi() {
  Serial.printf("Conectare la %s ", ssid);
  WiFi.begin(ssid, password);
  WiFi.setTxPower(WIFI_POWER_8_5dBm);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 40) { delay(500); Serial.print("."); attempts++; }
  if (WiFi.status() == WL_CONNECTED) { Serial.println("\nCONECTAT!"); configTzTime(timezoneInfo, ntpServer); }
  else { Serial.println("\nEroare conectare."); }
}

void checkWiFiConnection() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nWi-Fi pierdut! Reconectare...");
    print2display(" lose   "); delay(1000);
    WiFi.disconnect(); WiFi.reconnect(); WiFi.setTxPower(WIFI_POWER_8_5dBm);
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) { delay(500); Serial.print("."); attempts++; }
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nReconectat!"); print2display(" rCon   "); delay(1000);
      configTzTime(timezoneInfo, ntpServer);
    } else { Serial.println("\nEroare reconectare."); print2display(" err    "); }
  }
}

// ==========================================
// FUNCTIE INTENSITATE
// ==========================================
void updateBrightness(struct tm timeinfo) {
  int targetBrightness = 0;
  #if USE_POTENTIOMETER
    int potValue = analogRead(POT_PIN);
    targetBrightness = map(potValue, 0, 4095, 0, 7);
  #else
    int hour = timeinfo.tm_hour;
    if (hour >= 23 || hour < 6) targetBrightness = 5;
    else if (hour >= 21 || hour < 8) targetBrightness = 3;
    else targetBrightness = 0;
  #endif
  brightness_SDA5708(targetBrightness);
}

// ==========================================
// FUNCTIE METEO (Open-Meteo) - Presiune in mmHg la nivelul marii
// ==========================================
void translateWeatherCode() {
  if (weather_code == 0) weather_desc = " senin  ";
  else if (weather_code >= 1 && weather_code <= 3) weather_desc = " noros  ";
  else if (weather_code == 45 || weather_code == 48) weather_desc = " ceata  ";
  else if (weather_code >= 51 && weather_code <= 57) weather_desc = " burnita";
  else if (weather_code >= 61 && weather_code <= 67) weather_desc = " ploaie ";
  else if (weather_code >= 71 && weather_code <= 77) weather_desc = " zapada ";
  else if (weather_code >= 80 && weather_code <= 82) weather_desc = " ploaie ";
  else if (weather_code >= 85 && weather_code <= 86) weather_desc = " zapada ";
  else if (weather_code >= 95) weather_desc = " furtuna";
  else weather_desc = " ????   ";
}

void getWeatherData() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "http://api.open-meteo.com/v1/forecast?latitude=" + String(LATITUDE, 2) + 
                 "&longitude=" + String(LONGITUDE, 2) + 
                 "&current=temperature_2m,relative_humidity_2m,pressure_msl,weather_code&timezone=auto";
    
    http.begin(url);
    int httpCode = http.GET();

    if (httpCode == 200) {
      String payload = http.getString();
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, payload);
      
      JsonObject current = doc["current"];
      temperature = current["temperature_2m"].as<float>();
      humidity = current["relative_humidity_2m"].as<int>();
      
      float pressure_hPa = current["pressure_msl"].as<float>();
      pressure_mmHg = (int)(pressure_hPa * 0.750062 + 0.5); 
      
      weather_code = current["weather_code"].as<int>();
      
      translateWeatherCode();
      
      Serial.printf("Meteo: Temp=%.1f, Umid=%d%%, Pres=%dmmHg, Cod=%d\n", temperature, humidity, pressure_mmHg, weather_code);
    } else {
      Serial.printf("Eroare HTTP Meteo: %d\n", httpCode);
    }
    http.end();
  }
}

// ==========================================
// SETUP & LOOP
// ==========================================
void setup() {
  Serial.begin(115200);
  pinMode(DATA_PIN, OUTPUT); pinMode(LOAD_PIN, OUTPUT); pinMode(CLK_PIN, OUTPUT);
  #if USE_POTENTIOMETER
    pinMode(POT_PIN, INPUT);
  #endif

  init_SDA5708();
  brightness_SDA5708(0);
  print2display(" wait   ");
  connectToWiFi();
  getWeatherData();
  lastWeatherUpdate = millis();
  lastDisplayChange = millis();
}

void loop() {
  checkWiFiConnection();

  if (millis() - lastWeatherUpdate >= weatherInterval) {
    getWeatherData();
    lastWeatherUpdate = millis();
  }

  if(WiFi.status() == WL_CONNECTED) {
    struct tm timeinfo;
    if(getLocalTime(&timeinfo)) {
      
      updateBrightness(timeinfo);

      // Logica schimbare ecran: 15 secunde pentru Ceas, 3 secunde pentru restul
      unsigned long currentDisplayInterval = (displayMode == 0) ? 15000 : 3000;
      
      if (millis() - lastDisplayChange >= currentDisplayInterval) {
        displayMode++;
        if (displayMode > 4) displayMode = 0;
        lastDisplayChange = millis();
      }

      char screenStr[9];
      
      switch (displayMode) {
        case 0: // Ora (tine 15 secunde)
          snprintf(screenStr, sizeof(screenStr), "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
          break;
        case 1: // Temperatura (tine 3 secunde)
          {
            //temperature = -27.3;
            int tempInt = (int)temperature;
            int tempDec = abs((int)(temperature * 10) % 10);
            char tempStr[9];
            // Folosim caracterul ^ care va fi afisat pe display ca simbolul de grad (°)
            if (temperature >= 0) {
              if (temperature >=10)
              snprintf(tempStr, sizeof(tempStr), "+%2d.%d^C", tempInt, tempDec);
              else
              snprintf(tempStr, sizeof(tempStr), " +%1d.%d^C", tempInt, tempDec);
            } else {
              if (temperature <=-10)
              snprintf(tempStr, sizeof(tempStr), "%2d.%d^C", tempInt, tempDec);
              else
              snprintf(tempStr, sizeof(tempStr), "  %1d.%d^C", tempInt, tempDec);
            }
            snprintf(screenStr, sizeof(screenStr), "%-8s", tempStr); // Aliniere la stanga + spatii goale
          }
          break;
        case 2: // Umiditate (tine 3 secunde)
          snprintf(screenStr, sizeof(screenStr), " %2d%% RH ", humidity);
          break;
        case 3: // Presiune in mmHg (tine 3 secunde)
          snprintf(screenStr, sizeof(screenStr), "%4dmmHg ", pressure_mmHg);
          break;
        case 4: // Stare Vreme (tine 3 secunde)
          snprintf(screenStr, sizeof(screenStr), "%-8s", weather_desc.c_str());
          break;
      }
      
      print2display(screenStr);

    } else {
      print2display(" --:--  ");
    }
  }
  
  delay(100); 
}

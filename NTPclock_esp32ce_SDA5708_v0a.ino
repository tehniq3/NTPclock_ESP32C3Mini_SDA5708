#include <WiFi.h>
#include <time.h>

// --- Configurare Wi-Fi ---
const char* ssid = "bbk2";
const char* password = "internet2";

// --- Configurare Fus Orar Romania ---
const char* ntpServer = "pool.ntp.org";
const char* timezoneInfo = "EET-2EEST,M3.5.0/3,M10.5.0/3";

// --- Pini SDA5708 (Conform ESP32-C3 SuperMini) ---
const int LOAD_PIN = 6;
const int DATA_PIN = 4;
const int CLK_PIN  = 5;

// --- Fontul matrice 5x7 (De la Space la Z) ---
const unsigned char font[] = {
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
   0b11111000,0b00001000,0b00010000,0b00100000,0b01000000,0b10000000,0b11111000  // Z
};

// ==========================================
// DRIVER SDA5708 (Adaptat pentru ESP32)
// ==========================================

void init_SDA5708() {
  digitalWrite(LOAD_PIN, HIGH);
}

void send_byte_to_SDA5708(uint8_t byte_val) {
  digitalWrite(LOAD_PIN, LOW);
  for (uint8_t x = 0; x <= 7; x++) {
    digitalWrite(DATA_PIN, (byte_val >> x) & 1);
    digitalWrite(CLK_PIN, HIGH);
    delayMicroseconds(1);
    digitalWrite(CLK_PIN, LOW);
    delayMicroseconds(1);
  }
  digitalWrite(LOAD_PIN, HIGH);
}

void brightness_SDA5708(uint8_t val) {
  send_byte_to_SDA5708(0b11100000 | (val & 0b00000111));
}

void digit_to_SDA5708(uint8_t sign, uint8_t digit) {
  if ((sign < 0x20) || (sign > 0x5A)) sign = 0x20; 
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
// FUNCTII WI-FI
// ==========================================

void connectToWiFi() {
  Serial.printf("Conectare la %s ", ssid);
  WiFi.begin(ssid, password);
  WiFi.setTxPower(WIFI_POWER_8_5dBm); // Putere redusa pentru stabilitate
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 40) { // Timeout 20 secunde
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nCONECTAT!");
    // Configurare NTP la prima conectare
    configTzTime(timezoneInfo, ntpServer);
  } else {
    Serial.println("\nEroare conectare initiala.");
  }
}

void checkWiFiConnection() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nWi-Fi pierdut! Reconectare...");
    print2display(" LOSE   "); // Afisam mesaj ca am pierdut legatura
    delay(1000);
    
    WiFi.disconnect();
    WiFi.reconnect(); // Incercam reconectarea automata cu credentialele salvate
    WiFi.setTxPower(WIFI_POWER_8_5dBm);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) { // Timeout 10 secunde
      delay(500);
      Serial.print(".");
      attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nReconectat cu succes!");
      print2display(" rCon   "); // Afisam mesaj de reconectare reusita
      delay(1000);
      configTzTime(timezoneInfo, ntpServer); // Re-sincronizam NTP
    } else {
      Serial.println("\nEroare reconectare.");
      print2display(" Err    ");
    }
  }
}

// ==========================================
// SETUP & LOOP
// ==========================================

void setup() {
  Serial.begin(115200);
  
  pinMode(DATA_PIN, OUTPUT);      
  pinMode(LOAD_PIN, OUTPUT);      
  pinMode(CLK_PIN, OUTPUT);      

  init_SDA5708();
  brightness_SDA5708(0); // Luminozitate maximă
  print2display(" WAIT   ");

  connectToWiFi();
}

void loop() {
  // 1. Verificam daca mai avem Wi-Fi. Daca nu, reconectam.
  checkWiFiConnection();

  // 2. Daca avem Wi-Fi, incercam sa actualizam ora
  if(WiFi.status() == WL_CONNECTED) {
    struct tm timeinfo;
    if(getLocalTime(&timeinfo)) {
      char timeStr[9];
      snprintf(timeStr, sizeof(timeStr), "%02d:%02d:%02d", 
               timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
      print2display(timeStr);
    } else {
      // Daca NTP-ul nu raspunde temporar, afisam "--"
      print2display(" --:--  ");
    }
  }
  
  delay(500); 
}

#include <WiFi.h>
#include <time.h>

// --- Configurare Wi-Fi ---
const char* ssid = "NUME_RETEA_WIFI";
const char* password = "PAROLA_WIFI";

// --- Configurare Fus Orar Romania (EET/EEST) ---
// EET-2EEST,M3.5.0/3,M10.5.0/3 => UTC+2 iarna, UTC+3 vara
const char* ntpServer = "pool.ntp.org";
const char* timezoneInfo = "EET-2EEST,M3.5.0/3,M10.5.0/3";

// --- Pini SDA5708 ---
#define DATA_PIN  4
#define CLK_PIN   5
#define LOAD_PIN  6
// #define RESET_PIN 7 // Decomentează dacă ai conectat Reset-ul la un pin

void setup() {
  Serial.begin(115200);
  
  // Inițializare pini SDA5708
  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLK_PIN, OUTPUT);
  pinMode(LOAD_PIN, OUTPUT);
  
  // Reset hardware (opțional, decomentează dacă folosești pinul de reset)
  /*
  pinMode(RESET_PIN, OUTPUT);
  digitalWrite(RESET_PIN, LOW);
  delay(10);
  digitalWrite(RESET_PIN, HIGH);
  */

  // Conectare Wi-Fi
  Serial.printf("Conectare la %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println(" CONECTAT!");

  // Configurare NTP și Fus Orar
  configTzTime(timezoneInfo, ntpServer);
  
  // Așteptăm sincronizarea NTP la prima pornire
  struct tm timeinfo;
  while(!getLocalTime(&timeinfo)){
    Serial.println("Sincronizare NTP esuata, reincercare...");
    configTzTime(timezoneInfo, ntpServer);
    delay(1000);
  }
  Serial.println("Ora sincronizata cu succes!");
}

void loop() {
  struct tm timeinfo;
  
  if(getLocalTime(&timeinfo)) {
    // Formatam ora ca un text de 8 caractere: HH:MM:SS
    char timeStr[9];
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d:%02d", 
             timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    
    // Afisam pe SDA5708
    displayString(timeStr);
  } else {
    // Daca se pierde conexiunea, afisam "Err"
    displayString("  Err   ");
  }
  
  delay(500); // Reimprospatare la fiecare 0.5 secunde
}

// ==========================================
// DRIVER SDA5708
// ==========================================

// Funcție pentru a trimite un caracter la o poziție specifică
// SDA5708 folosește 11 biți: 8 biți date (caracter) + 3 biți adresă (poziție)
void sendChar(uint8_t digit, uint8_t character) {
  // Asiguram ca adresa digitului este intre 0 si 7
  digit &= 0x07;

  digitalWrite(LOAD_PIN, LOW);
  
  // Trimitem cei 8 biți de date (caracterul ASCII) - MSB First
  for (int i = 7; i >= 0; i--) {
    digitalWrite(CLK_PIN, LOW);
    digitalWrite(DATA_PIN, (character >> i) & 0x01);
    digitalWrite(CLK_PIN, HIGH);
  }
  
  // Trimitem cei 3 biți de adresă (poziția digitului) - MSB First
  for (int i = 2; i >= 0; i--) {
    digitalWrite(CLK_PIN, LOW);
    digitalWrite(DATA_PIN, (digit >> i) & 0x01);
    digitalWrite(CLK_PIN, HIGH);
  }
  
  digitalWrite(LOAD_PIN, HIGH);
}

// Funcție pentru a afisa un string complet (max 8 caractere)
void displayString(String text) {
  // Daca textul e mai scurt de 8, umplem cu spatii
  while(text.length() < 8) {
    text += " ";
  }
  
  for (int i = 0; i < 8; i++) {
    // SDA5708 are adresele inversate uneori (digit 0 e cel mai din dreapta)
    // Daca afisajul e inversat, schimba (7 - i) cu i
    sendChar(7 - i, text[i]); 
  }
}

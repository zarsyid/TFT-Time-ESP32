#include <WiFi.h>
#include "time.h"
#include "esp_sntp.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <DHT.h>
#include <SPI.h>

// Pins for the TFT display
#define TFT_CS     5
#define TFT_RST    16
#define TFT_DC     4

// DHT Sensor
#define DHTPIN 14      // Pin connected to DHT11 data pin
#define DHTTYPE DHT11  // DHT 11

const char *ssid = "IRA-GUEST"; // your ssid /your wifi name
const char *password = "berakhl@k"; // your wifi password

const char *ntpServer1 = "pool.ntp.org";
const char *ntpServer2 = "time.nist.gov";
const long gmtOffset_sec = 25200;  // GMT+7 for WIB
const int daylightOffset_sec = 0;  // No daylight saving time in Indonesia

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
DHT dht(DHTPIN, DHTTYPE);

#define ST77XX_PURPLE 0x780F
#define ST77XX_BLUE 0x001F
#define ST77XX_GREEN 0x07E0
#define ST77XX_ORANGE 0xFD20
#define ST77XX_GRAY 0x8410  // Custom definition for gray

const char* monthStr(int month) {
  switch (month) {
    case 0: return "Jan";
    case 1: return "Feb";
    case 2: return "Mar";
    case 3: return "Apr";
    case 4: return "May";
    case 5: return "Jun";
    case 6: return "Jul";
    case 7: return "Aug";
    case 8: return "Sep";
    case 9: return "Oct";
    case 10: return "Nov";
    case 11: return "Dec";
    default: return "Unknown";
  }
}

const char* dayStr(int wday) {
  switch (wday) {
    case 0: return "Sunday";
    case 1: return "Monday";
    case 2: return "Tuesday";
    case 3: return "Wednesday";
    case 4: return "Thursday";
    case 5: return "Friday";
    case 6: return "Saturday";
    default: return "Unknown";
  }
}

void drawInitialScreen() {
  // Draw date section
  tft.fillRect(0, 0, 160, 20, ST77XX_PURPLE);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);

  // Draw time section in the center
  tft.fillRect(0, 20, 160, 40, ST77XX_BLUE);

  // Draw temperature section
  tft.fillRect(0, 60, 80, 20, ST77XX_GREEN);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(5, 65);
  tft.print("Temp: --'C");

  // Draw humidity section
  tft.fillRect(80, 60, 80, 20, ST77XX_ORANGE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(85, 65);
  tft.print("Hum: --%");

  // Draw bottom text section
  tft.fillRect(0, 80, 160, 40, ST77XX_GRAY);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(2);
  tft.setCursor(5, 90);
  tft.print("Hello World!");
}

void updateDateSection(struct tm timeinfo) {
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.setCursor(5, 5);
  tft.printf("%s, %02d %s %04d", dayStr(timeinfo.tm_wday), timeinfo.tm_mday, monthStr(timeinfo.tm_mon), timeinfo.tm_year + 1900);
}

void updateTimeSection(struct tm timeinfo) {
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(3);
  int centerX = (160 - (6 * 18)) / 2; // Calculate center position for time (each character is approx 18 pixels wide)
  tft.fillRect(centerX, 25, 160, 40, ST77XX_BLUE); // Clear previous time
  tft.setCursor(centerX, 25);
  tft.printf("%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
}

void updateTemperature(float temperature) {
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(5, 65);
  tft.fillRect(0, 60, 80, 20, ST77XX_GREEN); // Clear previous temperature
  tft.printf("Temp: %02d'C", (int)temperature);
}

void updateHumidity(float humidity) {
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(85, 65);
  tft.fillRect(80, 60, 80, 20, ST77XX_ORANGE); // Clear previous humidity
  tft.printf("Hum: %02d%%", (int)humidity);
}

void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("No time available (yet)");
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(0, 0);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(1);
    tft.println("No time available");
    return;
  }
  
  // Update sections without clearing the screen
  updateDateSection(timeinfo);
  updateTimeSection(timeinfo);
  updateTemperature(dht.readTemperature());
  updateHumidity(dht.readHumidity());
}

// Callback function (gets called when time adjusts via NTP)
void timeavailable(struct timeval *t) {
  Serial.println("Got time adjustment from NTP!");
  printLocalTime();
}

void setup() {
  Serial.begin(115200);

  // Initialize TFT
  tft.initR(INITR_BLACKTAB);  // Initialize a ST7735S chip, black tab
  tft.setRotation(1); // Set to landscape mode
  tft.fillScreen(ST77XX_BLACK);

  // Initialize DHT sensor
  dht.begin();

  // Connect to Wi-Fi
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);

  // Set NTP server mode to DHCP
  esp_sntp_servermode_dhcp(1);  // (optional)

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    tft.fillScreen(ST77XX_BLACK);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(1);
    tft.setCursor(0, 0);
    tft.println("Connecting to WiFi ...");
  }
  Serial.println(" CONNECTED");

  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.setCursor(0, 0);
  tft.println("Connected");

  // Set notification call-back function
  sntp_set_time_sync_notification_cb(timeavailable);

  // Configure time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);

  // Draw the initial screen layout
  drawInitialScreen();

  delay(2000);
}

void loop() {
  // Update the displayed time and sensor readings every 5 seconds
  delay(5000);
  printLocalTime();  // It will take some time to sync time :)
}

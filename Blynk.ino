#define BLYNK_TEMPLATE_ID "id"
#define BLYNK_TEMPLATE_NAME "Esp32"
#define BLYNK_AUTH_TOKEN "key"

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <BlynkSimpleEsp32.h>

char ssid[] = "ssid";
char password[] = "pass";

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define WATER_LEVEL_PIN 34

BlynkTimer timer;
int displayMode = 1;

BLYNK_WRITE(V4) {
  displayMode = param.asInt();
  updateDisplay();
}

void updateDisplay() {
  float tempC = dht.readTemperature();
  float tempF = dht.readTemperature(true);
  float humidity = dht.readHumidity();
  int rawWaterLevel = analogRead(WATER_LEVEL_PIN);
  float waterLevel = map(rawWaterLevel, 0, 4095, 0, 100);

  if (isnan(tempC) || isnan(tempF) || isnan(humidity)) {
    tempC = tempF = humidity = 0;
  }

  Blynk.virtualWrite(V0, tempC);
  Blynk.virtualWrite(V1, tempF);
  Blynk.virtualWrite(V2, humidity);
  Blynk.virtualWrite(V3, waterLevel);

  lcd.clear();
  lcd.setCursor(0, 0);
  switch (displayMode) {
    case 1: lcd.print("Temp: " + String(tempF) + "F"); break;
    case 2: lcd.print("Temp: " + String(tempC) + "C"); break;
    case 3: lcd.print("Humidity: " + String(humidity) + "%"); break;
    case 4: lcd.print("Water: " + String(waterLevel)  + "%"); break;
    default: lcd.print("Invalid Mode");
  }
}

void setup() {
  Serial.begin(115200);

  lcd.init();
  lcd.backlight();
  lcd.print("Connecting...");
  
  lcd.clear();
  lcd.print("Starting Inputs!");

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);
  dht.begin();
  delay(3000);
  pinMode(WATER_LEVEL_PIN, INPUT);

  lcd.clear();
  lcd.print("Done!");

  timer.setInterval(2000L, updateDisplay);
}

void loop() {
  Blynk.run();
  timer.run();
  delay(1000);
}
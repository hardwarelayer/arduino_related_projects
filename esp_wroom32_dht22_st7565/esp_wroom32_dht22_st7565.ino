#include <bmps.h>
#include <ST7565_homephone_esp8266.h>
#include "DHT.h"

#define DHTPIN 17     // Digital pin connected to the DHT sensor(D17)
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

ST7565 lcd(23,22,21,19);
DHT dht(DHTPIN, DHTTYPE);
 
void setup()   {  
  lcd.ON();//c
  lcd.SET(22,0,0,0,4);   //d

  dht.begin();

  pinMode(18, OUTPUT);
}

bool isInited = false;
char sHumidity[40];
char sTemperature[40];
char sHic[40];

void loop(){

  if (!isInited) {
    digitalWrite(18, HIGH);
    lcd.Asc_String(0, 0, "Welcome to CloudSky", BLACK);
    lcd.Asc_String(0, 10, "Version 1.0", BLACK);
    lcd.Asc_String(0, 20, "Initializing sensors ...", BLACK);
    lcd.Display();
  
    delay(2000);
    lcd.Clear();
    lcd.Asc_String(0, 0, "Welcome to CloudSky", BLACK);
    lcd.Asc_String(0, 10, "Version 1.0", BLACK);
    lcd.Asc_String(0, 20, "Init completed!", BLACK);
    lcd.Display();
    delay(1000);

    isInited = true;
  }

  // Wait a few seconds between measurements.
  delay(2000);

  lcd.Clear();
  lcd.Asc_String(0, 0, "Welcome to CloudSky", BLACK);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  sprintf(sHumidity, "Humid: %2.2f", h);
  sprintf(sTemperature, "Temp C/F:%2.2f/%2.2f", t, f);
  sprintf(sHic, "HeatIdx C/F:%2.2f/%2.2f", hic, hif);
  
  lcd.Asc_String(0, 20, sHumidity, BLACK);
  lcd.Asc_String(0, 30, sTemperature, BLACK);
  lcd.Asc_String(0, 40, sHic, BLACK);
  lcd.Display();

  delay(3000);
}  

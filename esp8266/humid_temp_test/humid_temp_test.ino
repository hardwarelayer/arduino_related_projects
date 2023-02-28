#include <bmps.h>
#include <ST7565_homephone_esp8266.h>
#include "DHT.h"

#define PIN_WIRE_SDA (4)
#define PIN_WIRE_SCL (5)

#define DHTPIN 12     // Digital pin connected to the DHT sensor(D6)
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

//static const uint8_t SDA = PIN_WIRE_SDA;
//static const uint8_t SCL = PIN_WIRE_SCL;

//static const uint8_t LED_BUILTIN = 16;
//static const uint8_t BUILTIN_LED = 16;

static const uint8_t D0   = 16;
static const uint8_t D1   = 5;
static const uint8_t D2   = 4;
static const uint8_t D3   = 0;
static const uint8_t D4   = 2;
static const uint8_t D5   = 14;
static const uint8_t D6   = 12;
static const uint8_t D7   = 13;
static const uint8_t D8   = 15;
static const uint8_t D9   = 3;
static const uint8_t D10  = 1;

ST7565 lcd(D0, D1, D2, D3);//b
DHT dht(DHTPIN, DHTTYPE);
 
void setup()   {  
  lcd.ON();//c
  lcd.SET(22,0,0,0,4);   //d

  dht.begin();

  pinMode(D7, OUTPUT);
  pinMode(D8, OUTPUT);
}

bool isInited = false;
char sHumidity[40];
char sTemperature[40];
char sHic[40];

void loop(){

  if (!isInited) {
    digitalWrite(D7, HIGH);
    digitalWrite(D8, HIGH);
  
    lcd.Asc_String(0, 0, "Welcome to CloudSky", BLACK);
    lcd.Asc_String(0, 10, "Version 1.0", BLACK);
    lcd.Asc_String(0, 20, "Initializing sensors ...", BLACK);
    lcd.Display();
  
    delay(2000);
    lcd.Clear();
    digitalWrite(D7, LOW);
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

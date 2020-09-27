/*
 * Note: this library is compatible 
 * with Arduino-LiquidCrystal-I2C-library-master.zip, but you
 * have to change .init() -> .begin()
 * https://www.instructables.com/id/LCD-With-I2C/
But the wiring is wrong for this topic:  Then connect the SCL pin to A4 pin on the Arduino and the SDA pin to the A5 pin on the Arduino
--> in fact, SDA->A4, SCL->A5 and the lcd object init is different
lol, crap topic
 *
 * Scanning result:
  I2C device found at address 0x27  !
 * library:
 * https://github.com/johnrickman/LiquidCrystal_I2C
 * 
 * Note: Neopixel libs cannot work with CPU speed > 16MHz 
 * so you can't compile if set to 32MHz or above (in LGT8F328P)
 * It can be fixed in the .cpp of that library, but I am too lazy to do that :)
 * 
 * About DFPlayer, use https://powerbroker2.github.io/DFPlayerMini_Fast/html/index.html
 * This code also use FireTimer from https://badge.fury.io/gh/PowerBroker2%2FFireTimer
 */
#include  <Wire.h>
#include  <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <DFPlayerMini_Fast.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN    6

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 16

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

//https://stackoverflow.com/questions/54537198/arduino-nano-mini-dfplayer-mp3-is-disconnecting
SoftwareSerial mySerial(10, 11); // RX, TX
DFPlayerMini_Fast myDFPlayer;

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

#define DT A0
#define SCK A1

//calibrate button
#define sw 2

long sample=0;
float val=0;
long count=0;

int iCurrentOnLoadRainbowMode = 0;
unsigned long iIdleTimeStart = 0;
unsigned long iCurrentTime = 0;
#define IDLE_TO_FIX_TIME 30000

unsigned long iResultSpeakPendingStart = 0;
#define RESULT_SPEAK_PENDING_TIME 15000

unsigned long iPromptingVoiceStart = 0;
#define PROMPTING_VOICE_PENDING_TIME 30000

unsigned long readCount(void)
{
  unsigned long Count;
  unsigned char i;
  pinMode(DT, OUTPUT);
  digitalWrite(DT,HIGH);
  digitalWrite(SCK,LOW);
  Count=0;
  pinMode(DT, INPUT);
  while(digitalRead(DT));
  for (i=0;i<24;i++)
  {
    digitalWrite(SCK,HIGH);
    Count=Count<<1;
    digitalWrite(SCK,LOW);
    if(digitalRead(DT)) 
    Count++;
  }
  digitalWrite(SCK,HIGH);
  Count=Count^0x800000;
  digitalWrite(SCK,LOW);
  return(Count);
}

void setup() {
  mySerial.begin(9600);      
  myDFPlayer.begin(mySerial);

  delay(1000);

  //myDFPlayer.play(1); //play mp3 file with leading identifier "0001"
  //myDFPlayer.volume(20); //set volume low    
  //delay(1000); //wait 100ms

  //myDFPlayer.volume(30); //crank that stuff!    
  //delay(100); //wait another 100ms

  lcd.begin();                      // initialize the lcd 
  lcd.backlight();

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(25); // Set BRIGHTNESS to about 1/5 (max = 255)

  rainbow(2);             // Flowing rainbow cycle along the whole strip
  //delay(1000);
  lcd.setCursor(0,0);
  lcd.print("Xin chao!");
  lcd.setCursor(0,1);
  lcd.print("Weight Sensor v1");

  Serial.begin(9600);
  pinMode(SCK, OUTPUT);
  pinMode(sw, INPUT_PULLUP);
  //lcd.begin(16, 2);
  //lcd.print("    Weight ");
  //lcd.setCursor(0,1);
  //lcd.print(" Measurement ");
  delay(1000);
  //lcd.clear();

  playHelloAudioScript();

  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(0); // Set BRIGHTNESS to about 1/5 (max = 255)
  playCalibAudioScript(0);
  calibrate();
  playCalibAudioScript(2);
  theaterChase(strip.Color(127, 127, 127), 50); // White, half brightness
}

boolean bIsJustHaveNoWeight = 1;

void loop()
{
  //Serial.println("Start reading ...");

  count = readCount();
  int w=(((count-sample)/val)-2*((count-sample)/val));
  //Serial.print("weight:");
  //Serial.println((int)w);
  //Serial.println("g");

  if (w < 2) {
      strip.show();            // Turn OFF all pixels ASAP
      strip.setBrightness(0); // Set BRIGHTNESS to about 1/5 (max = 255)
      lcd.noBacklight();
      lcd.clear();

      if (iPromptingVoiceStart == 0) {
        iPromptingVoiceStart = millis();        
      }
      iCurrentTime = millis();
      if (iCurrentTime - iPromptingVoiceStart > PROMPTING_VOICE_PENDING_TIME) {
        playPlsUseAudioScript();
        iPromptingVoiceStart = millis();
      }

      //auto fix if under zero for 30s
      if (iIdleTimeStart > 0) {
        iIdleTimeStart = millis();
      }
      else {
        iCurrentTime = millis();
        if (iCurrentTime-iIdleTimeStart > IDLE_TO_FIX_TIME) {
          sample += w;
        }
      }

      bIsJustHaveNoWeight = 1;
  }
  else {
    //reset idle time
    iIdleTimeStart = 0;
    //reset prompting voice
    iPromptingVoiceStart = 0;

    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Weight sensor v1");  
    lcd.setCursor(0,1);
    String sMsg = "KetQua: " + String((int)w) + "g";
    lcd.print(sMsg);

    if (bIsJustHaveNoWeight) {
      //vua moi co do dat len can
      //doi 1.5s cho sensor on dinh
      //sau do lay lai ket qua
      delay(1500);
      count = readCount();
      w=(((count-sample)/val)-2*((count-sample)/val));
      //update lai
      lcd.setCursor(0,1);
      String sMsg = "KetQua: " + String((int)w) + "g";
      lcd.print(sMsg);
    }
    //neu dang khong co gi ma xuat hien vat the thi 
    //phat am ket qua luon, sau do cho
    if (w > 2) { //<-- ket qua do co the moi hon nen phai check lai
      if (iResultSpeakPendingStart > 0 || bIsJustHaveNoWeight == 1) {
        iCurrentTime = millis();
        if ((bIsJustHaveNoWeight == 1) || 
        (iCurrentTime - iResultSpeakPendingStart > RESULT_SPEAK_PENDING_TIME)) {
          iResultSpeakPendingStart = 0;
          bIsJustHaveNoWeight = 0;
          playWeightResultAudioScript((int)w);
        }
      }
      else {
        iResultSpeakPendingStart = millis();
      }   
    }

    //reset
    if (bIsJustHaveNoWeight == 1) {
      bIsJustHaveNoWeight = 1;
    }

    strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
    if (iCurrentOnLoadRainbowMode == 0) {
      rainbow(1);             // Flowing rainbow cycle along the whole strip
    }
    else if (iCurrentOnLoadRainbowMode == 1) {
      theaterChase(strip.Color(127,   0,   0), 10); // Red, half brightness      
    }
    else if (iCurrentOnLoadRainbowMode == 2) {
      theaterChase(strip.Color(  0,   0, 127), 10); // Blue, half brightness      
    }
    else if (iCurrentOnLoadRainbowMode == 3) {
      theaterChaseRainbow(10); // Rainbow-enhanced theaterChase variant
    }
    else if (iCurrentOnLoadRainbowMode == 4) {
      theaterChase(strip.Color(127, 127, 127), 10); // White, half brightness
    }
    if (iCurrentOnLoadRainbowMode < 4) {
      iCurrentOnLoadRainbowMode += 1;
    }
    else {
      iCurrentOnLoadRainbowMode = 0;
    }
  }

  //Serial.println("Stop reading ...");
  //delay(1000);
  /*
  lcd.setCursor(0,0);
  lcd.print("Weight            ");
  lcd.setCursor(0,1);
  lcd.print(w);
  lcd.print("g             ");
  */

  if(digitalRead(sw)==0)
  {
    val=0;
    sample=0;
    w=0;
    count=0;

    strip.show();            // Turn OFF all pixels ASAP
    strip.setBrightness(0); // Set BRIGHTNESS to about 1/5 (max = 255)
    playCalibAudioScript(0);
    calibrate();
    playCalibAudioScript(2);
    theaterChase(strip.Color(127, 127, 127), 50); // White, half brightness
  }
}

void calibrate()
{
  /*
  lcd.clear();
  lcd.print("Calibrating...");
  lcd.setCursor(0,1);
  lcd.print("Please Wait...");
  */
  int iTotalSample = 100;
  Serial.println("Calibrating ...");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Dang canh chinh");
  lcd.blink();
  delay(500);

  for(int i=0;i<iTotalSample;i++)
  {
    count=readCount();
    sample+=count;
    Serial.println(count);
  }
  sample/=iTotalSample;
  Serial.print("Avg:");
  Serial.println(sample);
  //lcd.clear();
  //lcd.print("Put 100g & wait");

  playCalibAudioScript(1);
  float fSampleWeight = 20.0;
  int iSampleWeight = int(fSampleWeight)*10;
  Serial.println("Put 20g and wait");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Dat 20g len may");
  lcd.setCursor(0,1);
  lcd.print("  va doi ...");
  lcd.blink();
  delay(2000);

  count=0;
  while(count<iSampleWeight)
  {
    count=readCount();
    count=sample-count;
    Serial.println(count);
  }
  //lcd.clear();
  //lcd.print("Please Wait....");
  Serial.println("Please Wait ...");
  delay(2000);
  for(int i=0;i<100;i++)
  {
    count=readCount();
    val+=sample-count;
    Serial.println(sample-count);
  }
  val=val/100.0;
  val=val/fSampleWeight; //100.0;        // put here your calibrating weight

  lcd.noBlink();
  lcd.setCursor(0,1);
  lcd.print("Done!          ");
  delay(1000);

}


// Some functions of our own for creating animated effects -----------------

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}

// Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
// between frames.
void theaterChase(uint32_t color, int wait) {
  for(int a=0; a<10; a++) {  // Repeat 10 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show(); // Update strip with new contents
      delay(wait);  // Pause for a moment
    }
  }
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait) {
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this outer loop:
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.numPixels() steps):
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through strip.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

// Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
void theaterChaseRainbow(int wait) {
  int firstPixelHue = 0;     // First pixel starts at red (hue 0)
  for(int a=0; a<30; a++) {  // Repeat 30 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in increments of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        // hue of pixel 'c' is offset by an amount to make one full
        // revolution of the color wheel (range 65536) along the length
        // of the strip (strip.numPixels() steps):
        int      hue   = firstPixelHue + c * 65536L / strip.numPixels();
        uint32_t color = strip.gamma32(strip.ColorHSV(hue)); // hue -> RGB
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show();                // Update strip with new contents
      delay(wait);                 // Pause for a moment
      firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
    }
  }
}

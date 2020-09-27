/*
 * https://www.instructables.com/id/LCD-With-I2C/
But the wiring is wrong for this topic:  Then connect the SCL pin to A4 pin on the Arduino and the SDA pin to the A5 pin on the Arduino
--> in fact, SDA->A4, SCL->A5
lol, crap topic
 *
 * Scanning result:
  I2C device found at address 0x27  !
 * library:
 * https://github.com/johnrickman/LiquidCrystal_I2C
 */
#include  <Wire.h>
#include  <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup()
{
  lcd.init();                      // initialize the lcd 
  lcd.backlight();
  Serial.begin(9600);
}

void loop()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Hello!");
  lcd.setCursor(0,1);
  lcd.print("Weight Sensor v1");

  delay(1000);
  return;

  // when characters arrive over the serial port...
  if (Serial.available()) {
    // wait a bit for the entire message to arrive
    delay(100);
    // clear the screen
    lcd.clear();
    // read all the available characters
    while (Serial.available() > 0) {
      // display each character to the LCD
      lcd.write(Serial.read());
    }
  }
}

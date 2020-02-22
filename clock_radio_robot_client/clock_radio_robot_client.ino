/*By TienTN - Nano NewBootLoader
 * 2020 Jan
//wiring: https://howtomechatronics.com/tutorials/arduino/arduino-wireless-communication-nrf24l01-tutorial/
// VCC     GND     * *
// CSN-8   CE-7    * *
// MOSI-11 SCK-13  * *
// IRQ     MISO-12 * *
 */
//Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>

#include <SPI.h>

RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001";
boolean flgLEDOn = false;
int iHeartRelay = 4; //5V +
int iDebugLED = 5;
unsigned long TOTAL_DISPLAY_INTERVAL = 140000;
unsigned long iStartDisplayTime = 0;

void initReceiver() {
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  delay(1000);
  radio.startListening();
  delay(1000);
}

void setup() {
  // put your setup code here, to run once:
  //Serial.begin(9600);

  pinMode(iDebugLED, OUTPUT);
  pinMode(iHeartRelay, OUTPUT);
  
  //Serial.println("initing receiver");
  initReceiver();
  //Serial.println("inited");

  digitalWrite(iDebugLED, HIGH);
  flgLEDOn = true;

  digitalWrite(iHeartRelay, LOW);
  delay(1000);
  digitalWrite(iHeartRelay, HIGH);
  delay(5000);
  digitalWrite(iHeartRelay, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (radio.available()) {
    char text[14] = "";
    radio.read(&text, sizeof(text));
    //Serial.println(text);

    if (!flgLEDOn) {
      iStartDisplayTime = millis();
      flgLEDOn = true;
    }
    
  }

  if (flgLEDOn) {
    digitalWrite(iHeartRelay, HIGH);

    if (millis() - iStartDisplayTime >= TOTAL_DISPLAY_INTERVAL) {
      flgLEDOn = false;
      digitalWrite(iHeartRelay, LOW);
      //to make sure there's no more current spike
      delay(2000);
      digitalWrite(iHeartRelay, LOW);
    }
  }

}

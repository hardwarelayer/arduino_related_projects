//by TienTN - LGT8FX Nano form
//LGT8328, Internal RC, 8MHz, 328P-LQFP32
//wiring: https://howtomechatronics.com/tutorials/arduino/arduino-wireless-communication-nrf24l01-tutorial/
//between Nano and nRF24L01
//NOTE: Only 3.3V!!!
// VCC     GND     * *
// CSN-8   CE-7    * *
// MOSI-11 SCK-13  * *
// IRQ     MISO-12 * *
//Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>
#include <SPI.h>

unsigned long TOTAL_DISPLAY_INTERVAL = 280000;
unsigned long iStartDisplayTime = 0;
RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001";
int iExternalRelay = 5; //5V +
boolean flgLEDOn = false;

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

  pinMode(iExternalRelay, OUTPUT);

  //Serial.println("initing receiver");
  initReceiver();
  //Serial.println("inited");

  digitalWrite(iExternalRelay, LOW);
  delay(1000);
  digitalWrite(iExternalRelay, HIGH); //1
  delay(5000);
  digitalWrite(iExternalRelay, LOW);

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
    digitalWrite(iExternalRelay, HIGH);

    if (millis() - iStartDisplayTime >= TOTAL_DISPLAY_INTERVAL) {
      flgLEDOn = false;
      digitalWrite(iExternalRelay, LOW);
      //to make sure there's no more current spike
      delay(2000);
      digitalWrite(iExternalRelay, LOW);
    }
    
  }
  
}

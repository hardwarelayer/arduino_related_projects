#include <SPI.h>

//by TienTN
//Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>

RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001";

//http://henrysbench.capnfatz.com/henrys-bench/arduino-sensors-and-input/arduino-hc-sr501-motion-sensor-tutorial/
int iNanoLEDPin=13;
int iMCUReplay = 2; //5V -
int iRadioRelay = 3; //3.3V +
int iClockRelay = 4; //9V +
int iMotionSensor = 5; //HC SR501

void initRadio() {
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
}

void transmitRadio() {
  const char text[] = "RadioBot_Event";
  Serial.println(text);
  radio.write(&text, sizeof(text));
  delay(1000);
}

//connect relay on NC (normally close)
void turnAllOn() {
  digitalWrite(iNanoLEDPin, HIGH);
  digitalWrite(iMCUReplay, HIGH); //turn off
  digitalWrite(iRadioRelay, HIGH);
  digitalWrite(iClockRelay, HIGH); 
}

void turnAllOff() {
  digitalWrite(iNanoLEDPin, LOW);
  digitalWrite(iMCUReplay, LOW); //turn on
  digitalWrite(iRadioRelay, LOW);
  digitalWrite(iClockRelay, LOW);
}

//WAIT TO CLEAR ANY INTERFERENCE
unsigned long t_delay_between_activate = 20000;
unsigned long t_delay_after_system_on = 90000;
unsigned long t_last_switch_status = 0;
boolean flg_system_on = false;

void setup() {
  Serial.begin(9600);

  pinMode(iNanoLEDPin,OUTPUT);
  
  pinMode(iMCUReplay, OUTPUT);
  pinMode(iRadioRelay, OUTPUT);
  pinMode(iClockRelay, OUTPUT);

  pinMode(iMotionSensor, INPUT);

  initRadio();

  turnAllOff();
  t_last_switch_status = millis();

  delay(3000); //warm up

}

void loop() {

  int val;
  val = digitalRead(iMotionSensor);
  //Serial.println(val);

  if (val < 1 && flg_system_on) {
    if (
       (millis() - t_last_switch_status < t_delay_after_system_on) 
       )
    {
      //Serial.println("force to on");
      val = 1; //force to always on during this time
    }  
  }

  if (val > 0)
  {
    if (!flg_system_on) {
      unsigned long timeDiff = millis() - t_last_switch_status;
      if (timeDiff > t_delay_between_activate)
      {
        Serial.println("turn on");
        flg_system_on = true;
        transmitRadio();
        turnAllOn();
        t_last_switch_status = millis();
      }
      else {
        //Serial.print(timeDiff);
        //Serial.println("  skipping");
      }
    }

  }
  else {
    if (flg_system_on) {
      turnAllOff();
      t_last_switch_status = millis();
      flg_system_on = false;
    }
  }
  delay(500);
}

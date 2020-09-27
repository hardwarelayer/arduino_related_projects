/*
 * https://www.instructables.com/id/LCD-With-I2C/
But the wiring is wrong for this topic:  Then connect the SCL pin to A4 pin on the Arduino and the SDA pin to the A5 pin on the Arduino
--> in fact, SDA->A4, SCL->A5 and the lcd object init is different
lol, crap topic
 *
 * Scanning result:
  I2C device found at address 0x27  !
 * library:
 * https://github.com/johnrickman/LiquidCrystal_I2C
 */
#include  <Wire.h>
#include  <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

#define DT A0
#define SCK A1

//calibrate button
//#define sw 2

long sample=0;
float val=0;
long count=0;

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
  lcd.init();                      // initialize the lcd 
  lcd.backlight();
  delay(1000);
  lcd.setCursor(0,0);
  lcd.print("Xin chao!");
  lcd.setCursor(0,1);
  lcd.print("Weight Sensor v1");

  Serial.begin(9600);
  pinMode(SCK, OUTPUT);
  //pinMode(sw, INPUT_PULLUP);
  //lcd.begin(16, 2);
  //lcd.print("    Weight ");
  //lcd.setCursor(0,1);
  //lcd.print(" Measurement ");
  delay(1000);
  //lcd.clear();
  calibrate();
}

void loop()
{
  Serial.println("Start reading ...");

  int baseCount = 839500;
  
  count= readCount();
  Serial.print(count-baseCount);
  int w=(((count-sample)/val)-2*((count-sample)/val));
  Serial.print("weight:");
  Serial.print((int)w);
  Serial.println("g");

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Weight sensor v1");  
  lcd.setCursor(0,1);
  String sMsg = "KQ: " + String((int)w) + "g";
  lcd.print(sMsg);

  //Serial.println("Stop reading ...");
  delay(1000);
  /*
  lcd.setCursor(0,0);
  lcd.print("Weight            ");
  lcd.setCursor(0,1);
  lcd.print(w);
  lcd.print("g             ");
  */

  /*
  if(digitalRead(sw)==0)
  {
    val=0;
    sample=0;
    w=0;
    count=0;
    calibrate();
  }*/
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
  lcd.print("Dang canh chinh ...");
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

  float fSampleWeight = 20.0;
  int iSampleWeight = int(fSampleWeight)*10;
  Serial.println("Put 20g and wait");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("De 20g len may ...");
  lcd.setCursor(0,1);
  lcd.print("  ... va doi ...");
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

  lcd.setCursor(0,1);
  lcd.print("Done!");
  delay(1000);

}

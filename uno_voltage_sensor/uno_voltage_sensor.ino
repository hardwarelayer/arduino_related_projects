// Define analog input
#define VOLTAGE_SENSOR_PIN A0
#define CURRENT_SENSOR_PIN A1
 
// Floats for ADC voltage & Input voltage
float adcVoltage = 0.0;
float inVoltage = 0.0;
 
// Floats for resistor values in divider (in ohms)
float R1 = 30000.0;
float R2 = 7500.0; 
 
// Float for Reference Voltage
float refVoltage = 5.0;
 
// Integer for ADC value
int adcValue = 0;

double ACSoffset = 2541.2; // offset for TienTN's ACS712
double mVAmp = 10.0;// mv per amp for your sensor you can find this on its data sheet

void setup()
{
   // Setup Serial Monitor
   Serial.begin(9600);
   Serial.println("DC Voltage Test");
}
 
void loop(){
   // Read the Analog Input
   adcValue = analogRead(VOLTAGE_SENSOR_PIN);
   
   // Determine voltage at ADC input
   adcVoltage  = (adcValue * refVoltage) / 1024.0; 
   
   // Calculate voltage at divider input
   inVoltage = adcVoltage / (R2/(R1+R2)) ; 

   float adcValueIn=0.0,rawSamples=0.0,adcAverage=0.0,inCurrent=0.0;

   for (int x = 0; x < 150; x++){ //Get 150 rawSamples
      adcValueIn = analogRead(CURRENT_SENSOR_PIN);     //Read current sensor values
      rawSamples = rawSamples + adcValueIn;  //Add rawSamples together
      delay (3); // let ADC settle before next sample 3ms
   }
   adcAverage=rawSamples/150.0;//Taking Average of rawSamples

   float v = (adcAverage/1024.0) * 5000.0; // converts the signal in to mA
   inCurrent = ((v - ACSoffset) / mVAmp);

   // Print results to Serial Monitor to 2 decimal places
   Serial.print("Input Voltage = ");
   Serial.print(inVoltage, 2);
   Serial.print(" Current = ");
   Serial.println(inCurrent, 2);
  
   // Short delay
   delay(500);
}

#include <SoftwareSerial.h>
#include <AltSoftSerial.h>

#define OperationPin 3

AltSoftSerial s;

const int analogInPin = A0;  // Analog input pin that the sensor is attached to
const int resValue = 10000;  // Value of 10kOhm resistor !change this to match your resistor
const float Vref = 1.1;  //This is the voltage of the internal reference

const float Sf = 0.6; // sensitivity in uA/ppm,

const int extraBit = 256; //Number of samples averaged, like adding 8 bits to ADC

long int sensorValue = 0;        // value read from the sensor

float sensedVal;
float Calculation;

String Instruction;
int operation;
int PreOn;

void setup() {
  Serial.begin(9600);      
  analogReference(INTERNAL);                                    
  pinMode(OperationPin,OUTPUT);
  s.begin(9600);
}

void loop() {
  OperationUpdate();
  delay(5000);
}

void OperationUpdate(){
  if (s.available()>0)
  {
     Instruction = s.readString();
     PreOn = getValue(Instruction, ',', 0).toInt();
     operation = getValue(Instruction, ',', 1).toInt();
     if (PreOn == 1)
     {
       analogWrite(OperationPin,168);
       Serial.println("PreOn");
     }
     if (operation == 1)
     {
       analogWrite(OperationPin,168);
       Serial.println("Operating");
       UploadValue();
     }
     else
     {
       analogWrite(OperationPin,0);
       Serial.println("Not Operating");
     }
  }
  else
  {
    analogWrite(OperationPin,0);
    UploadValue();
    Serial.println("s not available");
  }
}

void UploadValue(){
  Serial.print("Cl2 Concentration:"); 
  sensedVal = senseValue();
  Serial.print(sensedVal);
  Serial.print( "ppm" );
  Serial.print("\n");
  s.println(sensedVal);
}

float senseValue()
{
  sensorValue = 0;
  for (int i = 0; i < extraBit; i++) {
    sensorValue = analogRead(analogInPin) + sensorValue - 30;
    delay(3);   
  }

  Calculation = ((float) sensorValue / extraBit / 1024 * Vref / resValue * 1000000) / Sf;

  delay(218); 
  Serial.println(Calculation);
  return (Calculation);
}

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

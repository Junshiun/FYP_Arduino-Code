const int analogInPin = A0;  // Analog input pin that the sensor is attached to
const int resValue = 10000;  // Value of 10kOhm resistor !change this to match your resistor
const float Vref = 1.1;  //This is the voltage of the internal reference

const float Sf = 0.6; // sensitivity in uA/ppm,

const int extraBit = 256; //Number of samples averaged, like adding 8 bits to ADC

long int sensorValue = 0;        // value read from the sensor
float currentValue = 0;        // current read from the sensor

void setup() {
  Serial.begin(9600);
  analogReference(INTERNAL);
  Serial.println("CLEARDATA");
  Serial.println("LABEL, Time, Data");
  Serial.println("RESETTIMER"); 
  pinMode(3, OUTPUT);
}

void loop() {
  sensorValue = 0;
  analogWrite(3, 168);
  for (int i = 0; i < extraBit; i++) {
    sensorValue = analogRead(analogInPin) + sensorValue -30;
    delay(3);   // needs 2 ms for the analog-to-digital converter to settle after the last reading
  }

  //Serial.print("DATA, TIME, "); 
  Serial.print((float) sensorValue / extraBit);
  Serial.print("\tPPM = ");
  Serial.println( ((float) sensorValue / extraBit / 1024 * Vref / resValue * 1000000) / Sf);
  //Serial.print("\tuA = ");
  //Serial.print( (float) (sensorValue) / extraBit / 1024 * Vref / resValue * 1000000);
  //Serial.print("\tCounts = " );
  //Serial.println(sensorValue);
  
  //Trying to make each loop 1 second
  delay(218);  //1 second – 3ms*256ms (each adc read)-14ms (for printing)= 218ms
}

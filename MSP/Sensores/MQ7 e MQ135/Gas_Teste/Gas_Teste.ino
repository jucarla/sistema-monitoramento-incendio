#include <msp430.h>

void setup() {
  Serial.begin(9600);
}

void loop() {
  analogReadResolution(10);
  int CO_Sensor = analogRead(A0);
  Serial.print(CO_Sensor);
  int CO2_Sensor = analogRead(A1);
  Serial.print(", \t\t");
  Serial.print(CO2_Sensor);
  delay(1000);
}

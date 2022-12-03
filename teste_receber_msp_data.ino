#include <SoftwareSerial.h>
SoftwareSerial mspData(4,5);
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  mspData.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(mspData.available()){
    Serial.println("string:");
    Serial.print(mspData.readString());
  }else{
    Serial.println(".");
  }
}

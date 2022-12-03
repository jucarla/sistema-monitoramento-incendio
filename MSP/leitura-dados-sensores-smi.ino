//INCLUSÃO DAS BIBLIOTECAS
#include <msp430.h>
#include <dht.h>
#include <math.h>

//DEFINIÇÃO DE PINOS
int dhtPin = 7;
int wlSensorPin = A3;    //Sensor de nível de água
int sensorValue = 0;  // variable to store the value coming from the sensor
int relayPin = 6;
int co2Pin = A0;

//INTERVALO DE LEITURA
#define intervalo 2000
#define co2Zero   55 

//CRIANDO VARIAVEIS E INSTANCIANDO OBJETOS
unsigned long delayIntervalo;
dht sensorDHT;
int T, TGraus, R, HI;

void setup()
{
  // INICIANDO MONITOR SERIAL  
  Serial.begin(9600);
  pinMode(wlSensorPin, INPUT);
  pinMode(dhtPin, INPUT); 
  pinMode(co2, INPUT);   
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);
  delay(1000);
  digitalWrite(relayPin, HIGH);
  delay(1000);
  digitalWrite(relayPin, LOW);

}

void loop()
{

  if ( (millis() - delayIntervalo) > intervalo ) {
    //LEITURA DOS DADOS
    
    unsigned long start = micros();
    int chk = sensorDHT.read22(dhtPin);
    unsigned long stop = micros();

    waterLevel = analogRead(wlSensorPin);    
    
    analogReadResolution(10);         // Conversão Analógica em 10
    int CO2_Sensor = analogRead(co2Pin);  // Define pino P1.0 como Analógico
    int co2raw = 0;                   //int for raw value of co2
    int co2ppm = 0;                   //int for calculated ppm
    int zzz = 0;                      //int for averaging
  
    //Calibrando Sensor de CO2          
    zzz=zzz + CO2_Sensor;  
    co2raw = zzz/10;                  //divide samples by 10
    //co2ppm = co2raw - co2Zero;        //get calculated ppm
    co2ppm = CO2_Sensor;
    
    //Cálculo de Índice de Calor (HI)
    R = sensorDHT.humidity;
    TGraus = sensorDHT.temperature;
    T = (9/5)*(TGraus) + 32;
    HI = - 42.379 + 2.04901523*(T) + 10.14333127*(R) - 0.22475541*(T)*(R) - 0.00683783*(T)*(T) - 0.05481717*(R)*(R) + 0.00122874*(T)*(T)*(R) + 0.00085282*(T)*(R)*(R) - 0.00000199*(T)*(T)*(R)*(R);
    
    //Enviar os dados via serial para o esp receber:
    String Rstring = String(R);
    String Tstring = String(TGraus);
    String HIstring = String(HI);

  
    
    String msgBuffer = Rstring + "/" + Tstring + "/" + HIstring + "/" + co2ppm + "/" + waterLevel;
    Serial.println(msgBuffer);

    if (TGraus > 25){
      digitalWrite(relayPin, HIGH);
    }  else{
      digitalWrite(relayPin, LOW);
    }
    delayIntervalo = millis();
  }
}

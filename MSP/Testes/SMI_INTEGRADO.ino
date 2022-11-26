//INCLUSÃO DAS BIBLIOTECAS
#include <msp430.h>
#include <dht.h>
#include <math.h>
//DEFINIÇÃO DE PINOS
#define pinSensor 7

//INTERVALO DE LEITURA
#define intervalo 2000
#define co2Zero   55 

//CRIANDO VARIAVEIS E INSTANCIANDO OBJETOS
unsigned long delayIntervalo;
dht sensorDHT;
float T, TGraus, R, HI;

void setup()
{
  // INICIANDO MONITOR SERIAL  
  Serial.begin(9600);

  //IMPRIMINDO INFORMAÇÕES SOBRE A BIBLIOTECA
  Serial.print("VERSAO DA BIBLIOTECA: ");
  Serial.println(DHT_LIB_VERSION);
  Serial.println();
  Serial.println("Status,\tTempo(uS),\tUmidade(%),\tTemperatura(C)");
}

void loop()
{

  if ( (millis() - delayIntervalo) > intervalo ) {
    //LEITURA DOS DADOS
    unsigned long start = micros();
    int chk = sensorDHT.read22(pinSensor);
    unsigned long stop = micros();
    
    analogReadResolution(10);         // Conversão Analógica em 10
    int CO2_Sensor = analogRead(A0);  // Define pino P1.0 como Analógico
    int co2raw = 0;                   //int for raw value of co2
    int co2ppm = 0;                   //int for calculated ppm
    int zzz = 0;                      //int for averaging
  
    // VERIFICA SE HOUVE ERRO
    switch (chk)
    {
    case DHTLIB_OK:
        Serial.print("OK,\t");
        break;
    case DHTLIB_ERROR_CHECKSUM:
        Serial.print("Checksum error,\t");
        break;
    case DHTLIB_ERROR_TIMEOUT:
        Serial.print("Time out error,\t");
        break;
    case DHTLIB_ERROR_CONNECT:
        Serial.print("Connect error,\t");
        break;
    case DHTLIB_ERROR_ACK_L:
        Serial.print("Ack Low error,\t");
        break;
    case DHTLIB_ERROR_ACK_H:
        Serial.print("Ack High error,\t");
        break;
    default:
        Serial.print("Unknown error,\t");
        break;
    }

    //Calibrando Sensor de CO2          
    zzz=zzz + CO2_Sensor;  
    co2raw = zzz/10;                  //divide samples by 10
    co2ppm = co2raw - co2Zero;        //get calculated ppm
    
    //Cálculo de Índice de Calor (HI)
    R = sensorDHT.humidity;
    TGraus = sensorDHT.temperature;
    T = (9/5)*(TGraus) + 32;
    HI = - 42.379 + 2.04901523*(T) + 10.14333127*(R) - 0.22475541*(T)*(R) - 0.00683783*(T)*(T) - 0.05481717*(R)*(R) + 0.00122874*(T)*(T)*(R) + 0.00085282*(T)*(R)*(R) - 0.00000199*(T)*(T)*(R)*(R);
 
    // EXIBINDO DADOS LIDOS
    Serial.print(stop - start);
    Serial.print(", \t\t");
    Serial.print(sensorDHT.humidity, 2 /*FORMATAÇÃO PARA CASA DECIMAL*/);
    Serial.print(",\t\t");
    Serial.print(sensorDHT.temperature, 2 /*FORMATAÇÃO PARA CASA DECIMAL*/);
    Serial.print(",\t\t");
    Serial.print(co2ppm); // Printa o valor de CO2
    Serial.println(" ppm"); 
    

    //Verificando Índice de Calor
    if(HI < 80){
      Serial.print("Ok!");
      Serial.print("\t Indice de Calor = ");
      Serial.println(HI, 2);
    }
    else if(80 <= HI <= 90){
      Serial.print("Cuidado!");
      Serial.print("\t Indice de Calor = ");
      Serial.println(HI, 2);
    }
    else if(90 < HI <= 103){
      Serial.print("Cuidado Extremo!");
      Serial.print("\t Indice de Calor = ");
      Serial.println(HI, 2);
    }
    else if(103 < HI <= 125){
      Serial.print("PERIGO!");
      Serial.print("\t Indice de Calor = ");
      Serial.println(HI, 2);
    }
    else if(125 < HI){
      Serial.print("PERIGO EXTREMO!");
      Serial.print("\t Indice de Calor = ");
      Serial.println(HI, 2);
    }

    //Verificando ppm
    Serial.print("Qualidade do Ar: "); 
    if(co2ppm < 700){
      Serial.println("Excelente!");
    }
    else if(700 <= co2ppm < 1100){
      Serial.println("Boa!");
    }
    else if(1100 <= co2ppm < 1600){
      Serial.println("Mediana, ar interno contaminado...");
    }
    else if(1600 <= co2ppm < 2100){
      Serial.println("Ruim, ar interno MUITO contaminado!!");
    }
       
    delayIntervalo = millis();
  }
}

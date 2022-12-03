#include<ESP8266WiFi.h> //Biblioteca para funcionamento do WiFi do ESP
#include<ESP8266WebServer.h> //Biblioteca para o ESP funcionar como servidor
#include <DHT.h> //Biblioteca para funcionamento do sensor de temperatura e umidade DHT11
#include <SoftwareSerial.h>

SoftwareSerial mspData(4,5);   //D2, D1
const char* ssid = "Tássio";  // Rede WiFi
const char* password = "aaaaaaaa";  //Senha da Rede WiFi

ESP8266WebServer server(80); //server na porta 80

#define DHTPIN 4 //Pino digital D2 (GPIO5) conectado ao DHT11
#define DHTTYPE DHT11 //Tipo do sensor DHT11

DHT dht(DHTPIN, DHTTYPE); //Inicializando o objeto dht do tipo DHT passando como parâmetro o pino (DHTPIN) e o tipo do sensor (DHTTYPE)

float temperatura; //variável para armazenar a temperatura
float umidade; //Variável para armazenar a umidade


void setup() {
  Serial.begin(9600); //Inicializa a comunicação serial
  delay(50); // ?Intervalo para aguardar a estabilização do sistema
  dht.begin(); //Inicializa o sensor DHT11
  mspData.begin(9600);//Comunicacao msp
  Serial.println("Conectando a Rede: "); //Imprime na serial a mensagem
  Serial.println(ssid); //Imprime na serial o nome da Rede Wi-Fi

  WiFi.begin(ssid, password); //Inicialização da comunicação Wi-Fi

  //Verificação da conexão
  while (WiFi.status() != WL_CONNECTED) { //Enquanto estiver aguardando status da conexão
    delay(1000);
    Serial.print("."); //Imprime pontos
  }
  Serial.println("");
  Serial.println("WiFi Conectado");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP()); //Função para exibir o IP da ESP

  server.on("/", handle_OnConnect); //Servidor recebe uma solicitação HTTP - chama a função handle_OnConnect
  server.onNotFound(handle_NotFound); //Servidor recebe uma solicitação HTTP não especificada - chama a função handle_NotFound

  server.begin(); //Inicializa o servidor
  Serial.println("Servidor HTTP inicializado");

}

void loop() {
  server.handleClient(); //Chama o método handleClient()
}

void handle_OnConnect() {
  int mspBuffer = readMspData();
  temperatura = dht.readTemperature();  //Realiza a leitura da temperatura
  umidade = dht.readHumidity(); //Realiza a leitura da umidade
  String state;
  float limite = 190.0;
  if (mspBuffer > limite){
    state = "Cheio";
  } else {
    state = "Vazio";
  }
  Serial.print("Temperatura: ");
  Serial.print(temperatura); //Imprime no monitor serial o valor da temperatura lida
  Serial.println(" ºC");
  Serial.print("Umidade: ");
  Serial.print(umidade); //Imprime no monitor serial o valor da umidade lida
  Serial.println(" %");
  Serial.print("Fumaça: ");
  Serial.print("[Leitura do sensor de fumaça]"); //Imprime no monitor serial o valor da umidade lida
  Serial.println("!");
  Serial.print("Estado do tanque: ");
  Serial.println(mspBuffer); //Imprime no monitor serial o valor da umidade lida
  
  server.send(200, "text/html", EnvioHTML(temperatura, umidade, mspBuffer)); //Envia as informações usando o código 200, especifica o conteúdo como "text/html" e chama a função EnvioHTML
  

}

int readMspData(){
  int data;
  if (mspData.available()){
    data = mspData.read();
  }
  return data;
}

void handle_NotFound() { //Função para lidar com o erro 404
  server.send(404, "text/plain", "Não encontrado"); //Envia o código 404, especifica o conteúdo como "text/pain" e envia a mensagem "Não encontrado"

}

String EnvioHTML(float temperaturaStat, float umidadeStat, float mspBuffer) { //Exibindo a página da web em HTML
  String state;
  float limite = 190.0;
  if (mspBuffer > limite){
    state = "Cheio";
  } else {
    state = "Vazio";
  }
  String ptr = "<!DOCTYPE html> <html>\n"; //Indica o envio do código HTML
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n"; //Torna a página da Web responsiva em qualquer navegador Web
  ptr += "<meta http-equiv='refresh' content='2'>";//Atualizar browser a cada 2 segundos
  ptr += "<link href=\"https://fonts.googleapis.com/css?family=Open+Sans:300,400,600\" rel=\"stylesheet\">\n";
  ptr += "<title>Sistema de monitoramento de incêndio</title>\n"; //Define o título da página

  //Configurações de fonte do título e do corpo do texto da página web
  ptr += "<style>html { font-family: 'Open Sans', sans-serif; display: block; margin: 0px auto; text-align: center;color: #000000;}\n";
  ptr += "body{margin-top: 50px;}\n";
  ptr += "h1 {margin: 50px auto 30px;}\n";
  ptr += "h2 {margin: 40px auto 20px;}\n";
  ptr += "p {font-size: 24px;color: #000000;margin-bottom: 10px;}\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<div id=\"webpage\">\n";
  ptr += "<h1>Sistema de monitoramento de incêndio</h1>\n";
  ptr += "<h2>Sitema Microprocessados I</h2>\n";

  //Exibe as informações de temperatura e umidade na página web
  ptr += "<p><b>Temperatura: </b>";
  ptr += (float)temperaturaStat;
  ptr += " Graus Celsius</p>";
  ptr += "<p><b>Umidade: </b>";
  ptr += (float)umidadeStat;
  ptr += " %</p>";
  ptr += "<p><b>Fumaça: </b>";
  ptr += "[Leitura do sensor de fumaça]";
  ptr += " %</p>";
  ptr += "<p><b>Estado do tanque: </b>";
  ptr += (String)state;
  //ptr += " %</p>";

  ptr += "</div>\n";
  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;

}

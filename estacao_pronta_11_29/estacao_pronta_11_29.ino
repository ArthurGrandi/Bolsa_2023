#include <DHT.h>
#include <HTTP_Method.h>
#include <Uri.h>
#include <WiFi.h>
#include <DHT_U.h>
#include <Adafruit_Sensor.h>
#include <HTTPClient.h>
#include <ThingsBoard.h>

#define ssid "VISITANTES"
#define password ""

#define DHTPIN 32
#define DHTTYPE DHT11

const int PIN1 = 2;  // Reed Switch Pin
const int PIN2 = 4;  // Reed Switch Pin
const int PIN3 = 5;  // Reed Switch Pin
const int PIN4 = 18;  // Reed Switch Pin
const int PIN5 = 19;  // Reed Switch Pin
const int PIN6 = 21;  // Reed Switch Pin
const int PIN7 = 22;  // Reed Switch Pin
const int PIN8 = 34;  // Reed Switch Pin
const int PLUVIPIN = 23;  // Reed Switch Pin
const int ANEMPIN = 35;  // Reed Switch Pin

const char* thingsboardServer = "https://tb.geati.camboriu.ifc.edu.br/";
const char* accessToken = "f5a3iEEZ2iPhdIoVewTk";
const char* deviceId = "f0b973e0-5642-11ee-bdd4-b1244f07481c"; // Substitua pelo seu ID do dispositivo

DHT dht(DHTPIN, DHTTYPE);
float h;
float t;
float hic = dht.computeHeatIndex(t, h, false);

String direcao = "erro"; // Variável para armazenar a direção

volatile int ciclo = 0;
volatile float mm = 0;
volatile unsigned long lastPluviTime = 0;
const unsigned long debouncePluviDelay = 100;

const float pi = 3.14159265;     //Número de pi
int period = 5000;               //Tempo de medida(miliseconds)
int delaytime = 2000;            //Invervalo entre as amostras (miliseconds)
int radius = 65;                //Raio do anemometro(mm)

unsigned int counter = 0;        //Contador para o sensor
unsigned int RPM = 0;            //Rotações por minuto
float speedwind = 0;             //Velocidade do vento (m/s)
float windspeed = 0;             //Velocidade do vento (km/h)

void setup() {
  Serial.begin(9600);   // Init serial 9600
  pinMode(PIN1, INPUT);  // Set pin as INPUT
  pinMode(PIN2, INPUT);  // Set pin as INPUT
  pinMode(PIN3, INPUT);  // Set pin as INPUT
  pinMode(PIN4, INPUT);  // Set pin as INPUT
  pinMode(PIN5, INPUT);  // Set pin as INPUT
  pinMode(PIN6, INPUT);  // Set pin as INPUT
  pinMode(PIN7, INPUT);  // Set pin as INPUT
  pinMode(PIN8, INPUT);  // Set pin as INPUT
  pinMode(PLUVIPIN, INPUT);  // Set pin as INPUT
  pinMode(ANEMPIN, INPUT);  // Set pin as INPUT
  pinMode(DHTPIN, INPUT);  // Set pin as INPUT
  attachInterrupt(23, pluvi, CHANGE);  // Associa interrupção ao pino 2

  dht.begin();

  WiFi.mode(WIFI_STA);
  Serial.println("Conectando a Rede: "); //Imprime na serial a mensagem
  Serial.println(ssid); //Imprime na serial o nome da Rede Wi-Fi
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi Conectado");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  Serial.println("");
}

void loop() {
  Serial.println("Direc");
  direc();
  Serial.println("Anem");
  anem();
  Serial.println("DHT");
  sensorDHT();


  Serial.print("Direção: ");
  Serial.println(direcao);

  Serial.print("Velocidade do vento: ");
  Serial.print(speedwind);
  Serial.println("km/h");

  Serial.print("Milímetros de chuva: ");
  Serial.print(mm);
  Serial.println("mm");

  Serial.print("Umidade: ");
  Serial.print(h);
  Serial.println("%");
  Serial.print("Temperatura: ");
  Serial.print(t);
  Serial.println("°C ");
  Serial.print("Calor: ");
  Serial.print(hic);
  Serial.println("°C ");
  Serial.println("--------------------");

  delay(1000);  // Wait for 1 second

  // Crie uma mensagem JSON com os dados do sensor
  String payload = "{\"temperatura\":" + String(t) + ",\"humidade\":" + String(h) + ",\"calor\":" + String(hic) + ",\"direcao\":\"" + direcao + "\",\"velocidade do vento\":" + String(speedwind) + ",\"milimetros de chuva\":" + String(mm) + "}";

  String url = String(thingsboardServer) + "/api/v1/" + accessToken + "/telemetry";  // Crie a URL para enviar os dados para o ThingsBoar

  // Crie um cliente HTTP e envie os dados  HTTPClient http;
  HTTPClient http;  
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(payload);

  // Verifique a resposta do servidor
  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
  } else {
    Serial.print("Error sending data. HTTP Response code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

void direc() {
  int read1, read2, read3, read4, read5, read6, read7, read8;
  long startTime = millis();

  do {
    read1 = digitalRead(PIN1);
    read2 = digitalRead(PIN2);
    read3 = digitalRead(PIN3);
    read4 = digitalRead(PIN4);
    read5 = digitalRead(PIN5);
    read6 = digitalRead(PIN6);
    read7 = digitalRead(PIN7);
    read8 = digitalRead(PIN8);

    // Verifica cada reed switch usando switch...case
    if (read1 == 1) {
      direcao = "Norte";
    } else if (read2 == 1) {
      direcao = "Nordeste";
    } else if (read3 == 1) {
      direcao = "Leste";
    } else if (read4 == 1) {
      direcao = "Sudeste";
    } else if (read5 == 1) {
      direcao = "Sul";
    } else if (read6 == 1) {
      direcao = "Sudoeste";
    } else if (read7 == 1) {
      direcao = "Oeste";
    } else if (read8 == 1) {
      direcao = "Noroeste";
    } else {
      direcao = "erro";
    }

    if (direcao != "erro"){
      break;
    }

  } while (millis() < startTime + 5000);
}

void pluvi(){
  unsigned long currentPluviMillis = millis();

  if (currentPluviMillis - lastPluviTime >= debouncePluviDelay) {
    lastPluviTime = currentPluviMillis;
    ciclo++;
    mm += 1.7290;
  }
}

void anem(){
  speedwind = 0;
  windspeed = 0;
  counter = 0;

  long startTime = millis();
  while (millis() < startTime + period) {
    int readAnem = digitalRead(ANEMPIN);  // Reading pin value

    if (readAnem == 1){
      counter++;
      delay(20);
      while (readAnem == 1) {
        readAnem = digitalRead(ANEMPIN);
        if (readAnem != 1){
          break;
        } else if (millis() >= startTime + period){
          break;
        }
      }
    }
  }

  RPM = ((counter) * 60) / (period / 1000); // Calculate revolutions per minute (RPM)

  speedwind = (((2 * pi * radius * RPM) / 60) / 1000) * 3.6; //Calcula velocidade do vento em km/h
}

void sensorDHT(){
  h = dht.readHumidity();
  t = dht.readTemperature();
  hic = dht.computeHeatIndex(t, h, false);
}

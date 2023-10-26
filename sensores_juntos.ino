#include <Adafruit_BMP280.h>
#include <DHT.h>
#include <HTTP_Method.h>
#include <Uri.h>
#include <WebServer.h>
#include <WiFi.h>
#include <DHT_U.h>
#include <Adafruit_Sensor.h>
#include <ESPmDNS.h>
#include <HTTPClient.h>
#include <ThingsBoard.h>
#include <NTPClient.h>
#include <TimeLib.h>
#include <Timezone.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BME280.h>

#define TB_SERVER "http://tb.geati.camboriu.ifc.edu.br/"
#define TOKEN "f5a3iEEZ2iPhdIoVewTk"

#define ssid "VISITANTES"
#define password ""

#define SEALEVELPRESSURE_HPA (1013.25)

#define I2C_SCL 32
#define I2C_SDA 33

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "0.br.pool.ntp.org", -10800);

// Defina o fuso horário da sua região
const long gmtOffset_sec = -3 * 3600; // Por exemplo, GMT-3
const int daylightOffset_sec = 0;     // 0 para a maioria das regiões

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
const int SCL = 32;  // Reed Switch Pin
const int SDA = 

const char* thingsboardServer = "http://tb.geati.camboriu.ifc.edu.br/";
const char* accessToken = "f5a3iEEZ2iPhdIoVewTk";
const char* deviceId = "f0b973e0-5642-11ee-bdd4-b1244f07481c"; // Substitua pelo seu ID do dispositivo

DHT dht(DHTPIN, DHTTYPE);
float h;
float t;
float hic = dht.computeHeatIndex(t, h, false);
String msg;

WebServer server(80);

int direcao = 0; // Variável para armazenar a direção

volatile int ciclo = 0;
volatile float mm = 0;
volatile unsigned long lastPluviTime = 0;
const unsigned long debouncePluviDelay = 100;

const float pi = 3.14159265;     //Número de pi
int period = 5000;               //Tempo de medida(miliseconds)
int delaytime = 2000;            //Invervalo entre as amostras (miliseconds)
int radius = 65;                //Raio do anemometro(mm)

unsigned int Sample  = 0;        //Armazena o número de amostras
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
  attachInterrupt(digitalPinToInterrupt(PLUVIPIN), pluvi, CHANGE);  // Associa interrupção ao pino 2

if (!bme.begin(0x76)) {
    Serial.println("Não foi possível encontrar um sensor BME280, verifique a conexão!");
    while (1);
  }


  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

  WiFi.mode(WIFI_STA);
  Serial.println("Conectando a Rede: "); //Imprime na serial a mensagem
  Serial.println(ssid); //Imprime na serial o nome da Rede Wi-Fi
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

 // Inicialize o cliente NTP
  timeClient.begin();

  Serial.println("");
  Serial.println("WiFi Conectado");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  Serial.println("");

  if (MDNS.begin("esp32")){
    Serial.println("MDNS disponivel em esp32.local");
  }
  server.on("/", home);
  server.begin();
}
void loop() {
  direc();
  anem();
  sensorDHT();

  // Obter a hora da Internet
  timeClient.update();
  // Obter a hora atual
  Serial.println(timeClient.getFormattedTime());

  // Obter a data e hora atual
  time_t now = timeClient.getEpochTime();
  struct tm timeInfo;
  gmtime_r(&now, &timeInfo);

  // Exibir a data e hora no formato desejado
  char timeStringBuff[20];
  strftime(timeStringBuff, sizeof(timeStringBuff), "%Y-%m-%d %H:%M:%S", &timeInfo);
  Serial.print("Data e hora atual: ");
  Serial.println(timeStringBuff);
  delay(1000); // Esperar 1 segundo antes de obter a hora novamente

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
 
  server.handleClient();

  delay(1000);  // Wait for 1 second

  // Crie uma mensagem JSON com os dados do sensor
  String payload = "{\"temperatura\":" + String(t) + ",\"humidade\":" + String(h) + ",\"calor\":" + String(hic) + ",\"direcao\":\"" + direcao + "\",\"velocidade do vento\":" + String(speedwind) + ",\"milimetros de chuva\":" + String(mm) + "}";

  String url = String(thingsboardServer) + "/api/v1/" + accessToken + "/telemetry";  // Crie a URL para enviar os dados para o ThingsBoard

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
    if (read1 == HIGH) {
      direcao = 0;
    } else if (read2 == HIGH) {
      direcao = 45;
    } else if (read3 == HIGH) {
      direcao = 90;
    } else if (read4 == HIGH) {
      direcao = 135;
    } else if (read5 == HIGH) {
      direcao = 180;
    } else if (read6 == HIGH) {
      direcao = 225;
    } else if (read7 == HIGH) {
      direcao = 270;
    } else if (read8 == HIGH) {
      direcao = 315;
    } else {
      direcao = 1;
    }

    if (direcao != 1){
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

void home(){
  msg = "<!DOCTYPE html>";
  msg += "<html lang='pt-BR'>";
  msg += "<head><meta charset='UTF-8'></head>";
  msg += "<body>";
  msg += "<h2>Dados ESP-32:</h2>";
  msg += "<p><b>Temperatura: </b>";
  msg += t;
  msg += "&deg;C.</p>";
  msg += "<p><b>Umidade: </b>";
  msg += h;
  msg += "%.</p>";
  msg += "<p><b>Calor: </b>";
  msg += hic;
  msg += "&deg;C.</p>";
  msg += "<p><b>Direção do vento: </b>";
  msg += direcao;
  msg += ". </p>";
  msg += "<p><b>Velocidade do vento: </b>";
  msg += speedwind;
  msg += "km/h.</p>";
  msg += "<p><b>Mílimetros de chuva: </b>";
  msg += mm;
  msg += "mm.</p>";
  msg += "<script>";
  msg += "function autoRefresh() {";
  msg += "window.location = window.location.href;";
  msg += "}";
  msg += "setInterval('autoRefresh()', 10000);";
  msg += "</script>";
  msg += "</body>";
  msg += "</html>";

  server.send(200,"text/html",msg);

  delay(10000);
}
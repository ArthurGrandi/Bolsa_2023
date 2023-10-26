
#include <HTTP_Method.h>
#include <Uri.h>
#include <WebServer.h>

#include <WiFi.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_Sensor.h>
#include <ESPmDNS.h>


#define ssid "IFC"
#define password ""
//#define minutos 5
//#define segundos minutos*60*1000

#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

float h;
float t;
float hic = dht.computeHeatIndex(t, h, false);
String msg;

WebServer server(80);

void home() {
  msg = "<!DOCTYPE html><html><body><h2>Dados ESP-WROOM-32:</h2><p><b>Temperatura: </b>";
  msg += t;
  msg += "&deg;C.</p><p><b>Umidade: </b>";
  msg += h;
  msg += "%. </p><p><b>Calor: </b>";
  msg += hic;
  msg += "&deg;C. </p><script>function autoRefresh() {window.location = window.location.href;}setInterval('autoRefresh()', 10000);</script></body></html>";

  server.send(200,"text/html",msg);
}

void setup() {
  Serial.begin(9600);
  dht.begin();

  delay(3000);
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

  if (MDNS.begin("esp32")){
    Serial.println("MDNS disponivel em esp32.local");
  }
  server.on("/", home);
  server.begin();

}

void loop() {
  delay(500);
  h = dht.readHumidity();
  t = dht.readTemperature();
  hic = dht.computeHeatIndex(t, h, false);

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.println(F("%"));
  Serial.print(F("Temperature: "));
  Serial.print(t);
  Serial.println(F("°C "));
  Serial.print(F("Calor: "));
  Serial.print(hic);
  Serial.println(F("°C "));
  Serial.println(F(""));

  server.handleClient();
}
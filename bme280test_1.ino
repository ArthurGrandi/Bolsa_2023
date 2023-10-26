#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <DHT.h>  // Inclui a biblioteca do sensor DHT

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // Sensor BME280
#define DHTPIN 4       // Define a porta D4 para o DHT11
#define DHTTYPE DHT11  // Especifique o tipo do sensor DHT

DHT dht(DHTPIN, DHTTYPE);

unsigned long delayTime;

void setup() {
    Serial.begin(9600);
    while (!Serial); // Aguarda a inicialização da porta serial
    Serial.println(F("BME280 and DHT11 test"));

    if (!bme.begin(0x76)) {
        Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
        Serial.print("Sensor ID was: 0x");
        Serial.println(bme.sensorID(), 16);
        Serial.print("ID of 0x61 represents a BME 680.\n");
        while (1) delay(10);
    }

    dht.begin(); // Inicializa o sensor DHT

    Serial.println("-- Default Test --");
    delayTime = 1000;
}

void loop() {
    printBMEValues();
    printDHTValues();
    delay(delayTime);
}

void printBMEValues() {
    Serial.print("BME Temperature = ");
    float bme_temperature = bme.readTemperature();
    Serial.print(bme_temperature);
    Serial.println(" °C");

    Serial.print("Pressure = ");
    float bme_pressure = bme.readPressure() / 100.0F;
    Serial.print(bme_pressure);
    Serial.println(" hPa");

    Serial.print("Approx. Altitude = ");
    float bme_altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
    Serial.print(bme_altitude);
    Serial.println(" m");

    Serial.print("Humidity = ");
    float bme_humidity = bme.readHumidity();
    Serial.print(bme_humidity);
    Serial.println(" %");

    float bme_heatIndex = Sensacao(bme_temperature, bme_humidity);

    Serial.print("Heat Index (BME) = ");
    Serial.print(bme_heatIndex);
    Serial.println(" °C");

    Serial.println();
}

void printDHTValues() {
    Serial.print("DHT Temperature = ");
    float dht_temperature = dht.readTemperature();
    Serial.print(dht_temperature);
    Serial.println(" °C");

    Serial.print("DHT Humidity = ");
    float dht_humidity = dht.readHumidity();
    Serial.print(dht_humidity);
    Serial.println(" %");

    float dht_heatIndex = dht.computeHeatIndex(dht_temperature, dht_humidity, false);

    Serial.print("Heat Index (DHT) = ");
    Serial.print(dht_heatIndex);
    Serial.println(" °C");

    Serial.println();
}

// Função para calcular o índice de calor
float Sensacao(float temperature, float humidity) {
    // A mesma função de cálculo do índice de calor que você já tinha.
    // Não é necessário alterá-la.

    float T = temperature;
    float R = humidity;

    // Constants for the heat index calculation
    const float c1 = -8.784695;
    const float c2 = 1.61139411;
    const float c3 = 2.33854883889;
    const float c4 = -0.14611605;
    const float c5 = -0.012308094;
    const float c6 = -0.0164248277778;
    const float c7 = 0.002211732;
    const float c8 = 0.00072546;
    const float c9 = -0.000003582;

    float heatIndex = c1 + (c2 * T) + (c3 * R) + (c4 * T * R) + (c5 * T * T) + (c6 * R * R) + (c7 * T * T * R) + (c8 * T * R * R) + (c9 * T * T * R * R);

    return heatIndex;
}

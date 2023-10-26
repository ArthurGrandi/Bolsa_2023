const int pluvi = 2;  // Pino do Reed Switch do pluviômetro
volatile int ciclo = 0;
volatile float mm = 0;
volatile unsigned long lastPluviTime = 0;
const unsigned long debouncePluviDelay = 50;

void setup() {
  Serial.begin(9600);   // Inicializa a comunicação serial a 9600 bps
  pinMode(pluvi, INPUT);  // Define o pino do pluviômetro como INPUT
  attachInterrupt(digitalPinToInterrupt(pluvi), pluviometro, CHANGE);  // Associa interrupção ao pino 2
}
void loop() {
  
  Serial.println("");
  Serial.print("Pluviômetro ");
  Serial.println("");
  Serial.print("Quantas vezes tombou: ");
  Serial.println(ciclo);
  Serial.print("Milímetros: ");
  Serial.println(mm);
  delay(100);  // Wait for 1 second
}

void pluviometro(){
  unsigned long currentPluviMillis = millis();
  
  if (currentPluviMillis - lastPluviTime >= debouncePluviDelay) {
    lastPluviTime = currentPluviMillis;
    Serial.println(currentPluviMillis);

    
    ciclo++;
    mm += 1.7290; // 
  }
}
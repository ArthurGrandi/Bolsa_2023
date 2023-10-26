const int pluvi = 2;  // Pino do Reed Switch do pluviômetro
const int anem = 4;   // Pino do anemômetro
volatile int ciclo = 0;
volatile int volta = 0;
volatile int mm = 0;
volatile bool anemActive = false;
volatile unsigned long lastPluviTime = 0;
const unsigned long debouncePluviDelay = 50;
volatile unsigned long lastAnemTime = 0;
const unsigned long debounceAnemDelay = 50;

void setup() {
  Serial.begin(9600);   // Inicializa a comunicação serial a 9600 bps
  pinMode(pluvi, INPUT);  // Define o pino do pluviômetro como INPUT
  pinMode(anem, INPUT_PULLUP);   // Define o pino do anemômetro como INPUT com resistor de pull-up
  attachInterrupt(digitalPinToInterrupt(pluvi), pluviometro, CHANGE);  // Associa interrupção ao pino 2
  attachInterrupt(digitalPinToInterrupt(anem), anenometro, CHANGE);   // Associa interrupção ao pino 4
}

void loop() {
  Serial.println("Pluviômetro");
  Serial.print("Quantas vezes tombou: ");
  Serial.println(ciclo);
  Serial.println("");
  Serial.print("Milímetros: ");
  Serial.println(mm);
  Serial.println();

  Serial.println("Anemômetro");
  Serial.print("Voltas: ");
  Serial.println(volta);
  Serial.println();
  
  delay(1000);
}

void pluviometro() {
  unsigned long currentPluviMillis = millis();
  
  if (currentPluviMillis - lastPluviTime >= debouncePluviDelay) {
    lastPluviTime = currentPluviMillis;
    
    ciclo++;
    mm += 11;
  }
}

void anenometro() {
  unsigned long currentAnemMillis = millis();

  if (currentAnemMillis - lastAnemTime >= debounceAnemDelay) {
    lastAnemTime = currentAnemMillis;

    // Verifica se o pino do anemômetro está em LOW, indicando rotação
    if (digitalRead(anem) == LOW) {
      if (!anemActive) {
        volta++;
        anemActive = true;
      }
    } else {
      anemActive = false;
    }
  }
}
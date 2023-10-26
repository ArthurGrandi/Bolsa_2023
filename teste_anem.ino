  const int PIN1 = 2;  // Reed Switch Pin
volatile int voltas = 0;

void setup() {
  Serial.begin(9600);   // Init serial 9600
  pinMode(PIN1, INPUT);  // Set pin as INPUT
}
void loop() {
  int read1 = digitalRead(PIN1);  // Reading pin value

  if (read1 == 1){
    Serial.println("Ativado");
    voltas++;
    while (read1 == 1) {
      read1 = digitalRead(PIN1);
      if (read1 != 1){
        break;
      }
    }
  } else {
    Serial.println("Não encontrado.");
  }

  Serial.print("Voltas:");
  Serial.println(voltas);

  delay(10);  // Wait for 1 second

}
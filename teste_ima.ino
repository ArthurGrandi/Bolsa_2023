
const int PIN = 2; // Reed Switch Pin
int volta = 0;

void setup() {
  Serial.begin(9600); // Init serial 9600
  pinMode(PIN, INPUT); // Set pin as INPUT
}
void loop() {
  int read = digitalRead(PIN); // Reading pin value

  Serial.print("Voltas:");
  Serial.println(volta);

  if (read == HIGH) {
    Serial.println("I can feel the magnet!");
    while (read == HIGH) {
      read = digitalRead(PIN);
      if (read != HIGH){
        break;
      }
    }
    volta++;
  } else {
    Serial.println("Where is the magnet?");
  }
  delay(1); // Wait for 1 second
}
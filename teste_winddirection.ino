const int PIN1 = 2;  // Reed Switch Pin
const int PIN2 = 4;  // Reed Switch Pin
const int PIN3 = 5;  // Reed Switch Pin
const int PIN4 = 18;  // Reed Switch Pin
const int PIN5 = 19;  // Reed Switch Pin
const int PIN6 = 21;  // Reed Switch Pin
const int PIN7 = 22;  // Reed Switch Pin
const int PIN8 = 23;  // Reed Switch Pin

void setup() {
  Serial.begin(9600);   // Init serial 9600
  pinMode(PIN8, INPUT);  // Set pin as INPUT
  pinMode(PIN7, INPUT);  // Set pin as INPUT
  pinMode(PIN6, INPUT);  // Set pin as INPUT
  pinMode(PIN5, INPUT);  // Set pin as INPUT
  pinMode(PIN4, INPUT);  // Set pin as INPUT
  pinMode(PIN3, INPUT);  // Set pin as INPUT
  pinMode(PIN2, INPUT);  // Set pin as INPUT
  pinMode(PIN1, INPUT);  // Set pin as INPUT
}
void loop() {
  int read1 = digitalRead(PIN1);  // Reading pin value
  int read2 = digitalRead(PIN2);  // Reading pin value
  int read3 = digitalRead(PIN3);  // Reading pin value
  int read4 = digitalRead(PIN4);  // Reading pin value
  int read5 = digitalRead(PIN5);  // Reading pin value
  int read6 = digitalRead(PIN6);  // Reading pin value
  int read7 = digitalRead(PIN7);  // Reading pin value
  int read8 = digitalRead(PIN8);  // Reading pin value
  Serial.print("M");
  Serial.println(read1);
  Serial.print("V");
  Serial.println(read2);
  Serial.print("L");
  Serial.println(read3);
  Serial.print("A");
  Serial.println(read4);
  Serial.print("D");
  Serial.println(read5);
  Serial.print("Z");
  Serial.println(read6);
  Serial.print("R");
  Serial.println(read7);
  Serial.print("B");
  Serial.println(read8);

  if (read1 == 1){
    Serial.println("Norte");
  } else if (read2 == 1){
    Serial.println("Nordeste");
  } else if (read3 == 1){
    Serial.println("Leste");
  } else if (read4 == 1){
    Serial.println("Sudeste");
  } else if (read5 == 1){
    Serial.println("Sul");
  } else if (read6 == 1){
    Serial.println("Sudoeste");
  } else if (read7 == 1){
    Serial.println("Oeste");
  } else if (read8 == 1){
    Serial.println("Noroeste");
  } else {
    Serial.println("Não encontrado.");
  }

  delay(100);  // Wait for 1 second

}
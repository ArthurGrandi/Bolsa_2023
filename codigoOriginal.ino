
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <LiquidCrystal_PCF8574.h>


LiquidCrystal_PCF8574 lcd(0x3f);  // set the LCD address to 0x3f for a 20 chars and 4 line display
//LiquidCrystal_PCF8574 lcd(0x27);  // set the LCD address to 0x27 for a 16 chars and 2 line display


//Wind Direction
int show = 0;
int error = 0;
int ledPin = 13;
byte direct = 1;  //begin with North
char* compass[] = { "N  ", "NNE", "NE ", "ENE", "E  ", "ESE", "SE ", "SSE", "S  ", "SSW", "SW ", "WSW", "W  ", "WNW", "NW ", "NNW", "???" };
byte pointer = 0;


//Wind Speed
float radius = 0.065;  //metres from center pin to middle of cup
volatile int seconds = 0;
volatile int revolutions = 0;  //counted by interrupt
volatile int rps = -1;         // read revs per second (5 second interval)
volatile int mps = 0;          //wind speed metre/sec


//Rainfall
volatile double rainfall = 0;


//Barometer and Thermometer
double temperature = 0.0;
double barometer = 0.0;
Adafruit_BMP085 bmp;


void setup() {
  //The angle of wind direction is given here for each pin on its own
  //if two opins are activated, ie magnet halfway between, the direction
  //is interpolated so that 16 angles are reported, not just the 8 shown.
  //you may need to experiment with the magnet to get this accurate and
  //reliable.  The alignment of the reed switches should not be critical.
  pinMode(4, INPUT_PULLUP);   //angle ==   0 degrees
  pinMode(5, INPUT_PULLUP);   //angle ==  45 degrees
  pinMode(6, INPUT_PULLUP);   //angle ==  90 degrees
  pinMode(7, INPUT_PULLUP);   //angle == 135 degrees
  pinMode(8, INPUT_PULLUP);   //angle == 180 degrees
  pinMode(9, INPUT_PULLUP);   //angle == 225 degrees
  pinMode(10, INPUT_PULLUP);  //angle == 270 degrees
  pinMode(11, INPUT_PULLUP);  //angle == 315 degrees
  pinMode(ledPin, OUTPUT);


  Serial.begin(115200);
  Serial.println("LCD...");
  while (!Serial)
    ;
  Serial.println("Begin: check for LCD");


  // See http://playground.arduino.cc/Main/I2cScanner
  Wire.begin();
  Wire.beginTransmission(0x3f);
  error = Wire.endTransmission();
  if (error) {
    Serial.print("Error: ");
    Serial.print(error);
    Serial.println(": LCD not found.");
  }  // if
  lcd;
  lcd.begin(20, 4);  // initialize the lcd
  show = 0;
  pinMode(12, OUTPUT);  //controls LCD Back Light
  digitalWrite(12, HIGH);
  lcd.setBacklight(255);  //also controls LCD Back Light
  //Anemometer
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(0, rps_fan, FALLING);
  //Rainfall
  pinMode(3, INPUT_PULLUP);
  attachInterrupt(1, tipbuckets, FALLING);
  //Barometer
  bmp.begin();  //start the barometer and temp packages


  // Initialize Timer1 for a 1 second interrupt
  // Thanks to http://www.engblaze.com/ for this section, see their Interrupt Tutorial
  cli();       // disable global interrupts
  TCCR1A = 0;  // set entire TCCR1A register to 0
  TCCR1B = 0;  // same for TCCR1B
  // set compare match register to desired timer count:
  OCR1A = 15624;
  // turn on CTC mode:
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler:
  TCCR1B |= (1 << CS10);
  TCCR1B |= (1 << CS12);
  // enable timer compare interrupt:
  TIMSK1 |= (1 << OCIE1A);
  // enable global interrupts:
  sei();
}  // setup()


//Routine Driven by Interrupt, trap 1 second interrupts, and output every 5 seconds
ISR(TIMER1_COMPA_vect) {
  seconds++;
  if (seconds == 5) {  //make 5 for each output
    seconds = 0;
    rps = revolutions;
    revolutions = 0;
  }
}


// executed every time the interrupt 0 (pin2) gets low, ie one rev of cups.
void rps_fan() {
  revolutions++;
}  //end of interrupt routine


// executed every time the interrupt 1 (pin3) gets low, ie one tip of buckets.
void tipbuckets() {
  rainfall++;
  //this has not been calibrated as it will depend on your own printing dimensions
  //as a rough guide though, this system has 50 tips per 100ml, over a radius of 4.25cm
  //one tip is approx 2ml, or 2cmCubed. Area of catchment= pi*4.25*4.25=56.7 cm Squared
  //Rainfal per tip is 2/56.7=25th of a ml
}  //end of interrupt routine




void loop() {
  //lcd.setBacklight(255);
  lcd.home();
  lcd.clear();
  lcd.print("LTB Weather Rob Ward");
  while (true) {
    int pointer = -1;
    direct = (((PINB & B00001111) << 4) | ((PIND & B11110000) >> 4)) ^ B11111111;
    //chuck out any non readings where the magnet doesn't work
    switch (direct) {
      case 1:
        pointer = 0;
        break;
      case 3:
        pointer = 1;
        break;
      case 2:
        pointer = 2;
        break;
      case 6:
        pointer = 3;
        break;
      case 4:
        pointer = 4;
        break;
      case 12:
        pointer = 5;
        break;
      case 8:
        pointer = 6;
        break;
      case 24:
        pointer = 7;
        break;
      case 16:
        pointer = 8;
        break;
      case 48:
        pointer = 9;
        break;
      case 32:
        pointer = 10;
        break;
      case 96:
        pointer = 11;
        break;
      case 64:
        pointer = 12;
        break;
      case 192:
        pointer = 13;
        break;
      case 128:
        pointer = 14;
        break;
      case 129:
        pointer = 15;
        break;
      default:
        pointer = 16;
        // if nothing else matches, do the default
        // default 16, "???" mainly for debugging
        break;
    }
    //Serial.print(direct);
    //Serial.print("\t -> ");
    //Serial.print(pointer);
    //Serial.print("\t -> ");
    hexBinDump(direct);  //working beautifully

    lcd.setCursor(0, 1);
    lcd.print("Wind: ");
    if (rps != -1) {  //Update every 5 seconds, this will be equal to reading frequency (Hz)x5.
      float kmph = rps * 3.1414 * 2 * radius * 12 * 60 / 1000;
      if (direct != 0) {
        lcd.print(compass[pointer]);
      }
      lcd.setCursor(10, 1);
      lcd.print("@ ");
      if (kmph < 10) {
        lcd.print(" ");
      }
      lcd.print(kmph, 1);  //kilometres per hour to one decimal place
      lcd.print("m/s ");
    }
    //Barometer and Temperature
    temperature = (double)bmp.readTemperature();     //internal temperature
    barometer = (double)bmp.readPressure() / 100.0;  //Pa reduced to mBar
    lcd.setCursor(0, 2);
    lcd.print("P:");
    if (barometer < 1000) {
      lcd.print(" ");
    }
    lcd.print(barometer, 0);
    lcd.print("kPa ");
    lcd.setCursor(11, 2);
    lcd.print("T:");
    if (temperature < 10) {
      lcd.print(" ");
    }
    lcd.print(temperature, 1);
    lcd.print("C ");
    //Rainfall
    lcd.setCursor(0, 3);
    lcd.print("Rainfall: ");
    lcd.print(rainfall, 1);
    lcd.print("mm   ");
    delay(1000);
  }
}  // loop()


//Useful to invoke to debug the byte Array
void hexBinDump(int myNos) {
  //Serial.println("T A3 10100011 07 00000111 02 00000010 AA 10101010 F0 11110000 06 00000110 FF 11111111 07 00000111 33 00110011 60 01100000");
  byte mask = B10000000;
  if ((myNos & B11110000) == 0) {  //alter if you need to change how many bytes you are checking
    Serial.print("0");
  }
  Serial.print(myNos, HEX);
  Serial.print(" ");
  for (int k = 0; k < 8; k++) {
    if (myNos & mask) {
      Serial.print("1");
    } else {
      Serial.print("0");
    }
    mask = mask >> 1;
  }
  Serial.println();
}
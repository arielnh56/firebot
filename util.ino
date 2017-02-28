// grabbed off the playground - for the qti

long RCtime(int sensPin) {
  long result = 0;
  pinMode(sensPin, OUTPUT);       // make pin OUTPUT
  digitalWrite(sensPin, HIGH);    // make pin HIGH to discharge capacitor - study the schematic
  delay(1);                       // wait a  ms to make sure cap is discharged

  pinMode(sensPin, INPUT);        // turn pin into an input and time till pin goes low
  digitalWrite(sensPin, LOW);     // turn pullups off - or it won't work
  while (digitalRead(sensPin)) {  // wait for pin to go low
    result++;
  }

  return result;                   // report results
}

unsigned long int qtiset = 0;
void setQTI(int sensPin) {
  pinMode(sensPin, OUTPUT);       // make pin OUTPUT
  digitalWrite(sensPin, HIGH);    // make pin HIGH to discharge capacitor - study the schematic
  delay(1);                       // wait a  ms to make sure cap is discharged

  pinMode(sensPin, INPUT);        // turn pin into an input and time till pin goes low
  digitalWrite(sensPin, LOW);     // turn pullups off - or it won't work
  qtiset = micros();
  attachInterrupt(digitalPinToInterrupt(sensPin), readQTI, CHANGE);
}

void readQTI() {
  QTIvalue = micros() - qtiset;
}

void douse_fire() {
  co2.write(CO2_FIRE);
  delay(500);
  co2.write(CO2_PARK);
}


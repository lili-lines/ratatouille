void setup() {
  pinMode(LED_BUILTIN, OUTPUT);   // la LED intégrée du Teensy
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH); // allumée
  delay(200);
  digitalWrite(LED_BUILTIN, LOW);  // éteinte
  delay(200);
}

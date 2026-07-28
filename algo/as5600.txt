// Lecture des moustaches AS5600 en analogique — Teensy 4.0
const int WHISKER_L = A0;   // OUT moustache gauche
const int WHISKER_R = A1;   // OUT moustache droite (si 2ᵉ capteur)

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);   // valeurs sur 0..4095
}

void loop() {
  int rawL = analogRead(WHISKER_L);
  int rawR = analogRead(WHISKER_R);

  Serial.print(rawL);         // colonne 1
  Serial.print('\t');         // tabulation
  Serial.println(rawR);       // colonne 2

  delay(50);                  // 20 lectures/s
}

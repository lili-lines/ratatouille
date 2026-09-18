// Lecture des moustaches AS5600 en analogique — Teensy 4.0
const int WHISKER_R = 14;   // OUT moustache droite
const int WHISKER_F = 15;   // OUT moustache avant
const int WHISKER_L = 16;   // OUT moustache gauche

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);   // valeurs sur 0..4095
}

void loop() {
  int rawL = analogRead(WHISKER_L);
  int rawF = analogRead(WHISKER_F);
  int rawR = analogRead(WHISKER_R);

  Serial.print(rawL);         // colonne 1
  Serial.print('\t');         // tabulation
  Serial.print(rawF);         // colonne 2
  Serial.print('\t');
  Serial.println(rawR);       // colonne 3

  delay(50);                  // 20 lectures/s
}

// Lecture d'un encodeur seul, pour mesurer le CPR (comptes par tour de roue)
// protocole : noter la valeur, tourner la roue A LA MAIN de 10 tours complets,
// noter la nouvelle valeur, puis CPR = (finale - initiale) / 10

const int ENC_A = 9;
const int ENC_B = 10;
volatile long encoderCount = 0;

void encoderISR() {
  if (digitalRead(ENC_B) == HIGH) encoderCount++;
  else encoderCount--;
}

void setup() {
  Serial.begin(115200);

  pinMode(ENC_A, INPUT);
  pinMode(ENC_B, INPUT);
  attachInterrupt(digitalPinToInterrupt(ENC_A), encoderISR, RISING);

  Serial.println("Note la valeur, tourne la roue de 10 tours complets, note la nouvelle valeur.");
  Serial.println("CPR = (valeur finale - valeur initiale) / 10");
}

void loop() {
  Serial.println(encoderCount);
  delay(100);
}

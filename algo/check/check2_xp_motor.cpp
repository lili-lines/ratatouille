// test de mise en route des 2 moteurs, appuie sur GO pour lancer la sequence
// roues levees dans le vide pour ce test (comme le test de glissement)

const int PWMA = 2;
const int AIN1 = 3;
const int AIN2 = 4;

const int PWMB = 5;
const int BIN1 = 6;
const int BIN2 = 20;

const int STBY = 8;

const int ENC_A_R = 9;
const int ENC_B_R = 10;
const int ENC_A_L = 11;
const int ENC_B_L = 12;
volatile long encCountR = 0;
volatile long encCountL = 0;

const int BTN_GO = 17;
const int BTN_RESET = 22;

const int LED_STATE = 21;

const int SPEED = 130;  // 0-255

void encoderR_ISR() {
  if (digitalRead(ENC_B_R) == HIGH) encCountR++;
  else encCountR--;
}

void encoderL_ISR() {
  if (digitalRead(ENC_B_L) == HIGH) encCountL++;
  else encCountL--;
}

void motorA(int speed) {
  // speed > 0 = avance, speed < 0 = recule, 0 = stop
  digitalWrite(AIN1, speed >= 0 ? HIGH : LOW);
  digitalWrite(AIN2, speed >= 0 ? LOW : HIGH);
  analogWrite(PWMA, abs(speed));
}

void motorB(int speed) {
  digitalWrite(BIN1, speed >= 0 ? HIGH : LOW);
  digitalWrite(BIN2, speed >= 0 ? LOW : HIGH);
  analogWrite(PWMB, abs(speed));
}

void printState(const char* phase, unsigned long durationMs) {
  unsigned long start = millis();
  while (millis() - start < durationMs) {
    Serial.print(phase);      Serial.print('\t');
    Serial.print(encCountR);  Serial.print('\t');
    Serial.println(encCountL);
    delay(100);
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(STBY, OUTPUT);
  digitalWrite(STBY, LOW);

  pinMode(ENC_A_R, INPUT);
  pinMode(ENC_B_R, INPUT);
  pinMode(ENC_A_L, INPUT);
  pinMode(ENC_B_L, INPUT);
  attachInterrupt(digitalPinToInterrupt(ENC_A_R), encoderR_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_A_L), encoderL_ISR, RISING);

  pinMode(BTN_GO, INPUT_PULLUP);
  pinMode(BTN_RESET, INPUT_PULLUP);

  pinMode(LED_STATE, OUTPUT);
  digitalWrite(LED_STATE, LOW);

  Serial.println("Roues levees. Appuie sur GO pour lancer la sequence de test.");
}

void loop() {
  if (digitalRead(BTN_RESET) == LOW) {
    motorA(0);
    motorB(0);
    digitalWrite(STBY, LOW);
    digitalWrite(LED_STATE, LOW);
    Serial.println("RESET");
    delay(300);
  }

  if (digitalRead(BTN_GO) == LOW) {
    digitalWrite(STBY, HIGH);
    digitalWrite(LED_STATE, HIGH);
    Serial.println("phase\tencR\tencL");

    encCountR = 0;
    encCountL = 0;
    motorA(SPEED);
    printState("A_avant", 2000);
    motorA(0);

    encCountR = 0;
    encCountL = 0;
    motorB(SPEED);
    printState("B_avant", 2000);
    motorB(0);

    encCountR = 0;
    encCountL = 0;
    motorA(-SPEED);
    motorB(-SPEED);
    printState("AB_arriere", 2000);
    motorA(0);
    motorB(0);

    digitalWrite(STBY, LOW);
    digitalWrite(LED_STATE, LOW);
    Serial.println("Sequence terminee. Appuie sur GO pour recommencer.");
  }
}

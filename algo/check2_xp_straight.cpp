// avance tout droit sur 30cm (en distance calculee via les encodeurs), puis stop complet
// correction combinee : encodeurs (distance) + gyro (cap reel, evite la derive laterale)

#include <Wire.h>

const int MPU_ADDR = 0x68;
float headingDeg = 0;
float gyroZ_bias = 0;
unsigned long lastTime = 0;
float lastDt = 0;          // dt de la derniere updateHeading(), reutilise pour l'integrale
float headingIntegral = 0; // cumul de l'erreur de cap, annule le biais residuel (P seul ne le peut pas)

const int PWMA = 2;
const int AIN1 = 3;
const int AIN2 = 4;

const int PWMB = 5;
const int BIN1 = 6;
const int BIN2 = 20;

const int STBY = 8;

const int ENC_A_R = 11;
const int ENC_B_R = 12;
const int ENC_A_L = 9;
const int ENC_B_L = 10;

volatile long encCountR = 0;
volatile long encCountL = 0;

const int BTN_GO = 17;
const int BTN_RESET = 22;

const int LED_STATE = 21;

const int SPEED = 130;  // 0-255

// 🧭 
const float KP_ENC = 0;  // 0.25   gain correction encodeurs (distance)
const float KP_GYRO = 3.0;  // 3.0   gain correction gyro (cap), a ajuster
const float KI_GYRO = 0;      // 0.3   gain integral (cap), annule le biais residuel, a ajuster
const int MAX_CORRECTION = 40;  // limite de correction, evite les a-coups
const float INTEGRAL_LIMIT = 50.0;  // anti-windup, borne l'integrale seule

// 🧭 Cap de consigne, en degres : le robot tient cet angle au lieu de 0 pour compenser une derive.
// NEGATIF = vise a droite (verifie : +2.3 a aggrave une derive a gauche).
// Le P seul ne tient qu'une partie de la consigne, donc calibrer par essais :
//   mesures : 0 -> 4 cm a gauche sur 1 m ; +2.3 -> 6 cm a gauche  => ~0.9 cm par degre
//   => -4.6 pour annuler. Ajuster de +-1 selon la regle.
const float HEADING_TRIM = -1.6;


// calibration : CPR ~1151, circonference roue ~100mm (Ø32mm)
const float MM_PER_TICK = 100.0 / 1151.0;
const float DIST_MM = 900.0;   
// distance a parcourir, en mm : 30 cm = 300, 1 m = 900

// Facteur empirique si les encodeurs perdent des ticks (ENC_SCALE = demande / mesure a la regle).
// Observe en USB seul : les moteurs font s'effondrer le 5 V, le 3,3 V des encodeurs suit, et les
// deux encodeurs ratent des fronts au hasard -> 36 cm pour 30. Sur batterie, laisser 1.0.
const float ENC_SCALE = 1.0;
const long TICKS_TARGET = (long)(DIST_MM * ENC_SCALE / MM_PER_TICK);

enum State { IDLE, FORWARD, DONE };
State state = IDLE;

// 🧭 le sens moteur de comptage
void encoderR_ISR() {
  if (digitalRead(ENC_B_R) == HIGH) encCountR++;
  else encCountR--;
}
void encoderL_ISR() {
  if (digitalRead(ENC_B_L) == HIGH) encCountL++;
  else encCountL--;
}

// 🧭 le sens moteur
// speed → + : avance, - : recule
void motorA(int speed) {
  digitalWrite(AIN1, speed >= 0 ? HIGH : LOW);
  digitalWrite(AIN2, speed >= 0 ? LOW : HIGH);
  analogWrite(PWMA, abs(speed));
}
void motorB(int speed) { // ici pour le moteur B
  digitalWrite(BIN1, speed >= 0 ? HIGH : LOW);
  digitalWrite(BIN2, speed >= 0 ? LOW : HIGH);
  analogWrite(PWMB, abs(speed));
}

void stopMotors() {
  motorA(0);
  motorB(0);
}

void updateHeading() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x47);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 2, true);
  int16_t gyroZraw = Wire.read() << 8 | Wire.read();

  // 🧭 calibrage du gyro au démarage
  float gyroZ_dps = (gyroZraw / 131.0) - gyroZ_bias;

  unsigned long now = millis();
  float dt = (now - lastTime) / 1000.0;
  lastTime = now;
  headingDeg += gyroZ_dps * dt;
  lastDt = dt;
}

void setup() {
  Serial.begin(115200);
  Wire.begin();

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

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

  delay(500);

  Serial.println("Calibration du gyro, ne touche pas au robot...");
  const int N = 200;
  float sum = 0;
  for (int i = 0; i < N; i++) {
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x47);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_ADDR, 2, true);
    int16_t raw = Wire.read() << 8 | Wire.read();
    sum += raw / 131.0;
    delay(5);
  }
  gyroZ_bias = sum / N;
  Serial.print("Biais gyro : ");
  Serial.println(gyroZ_bias);

  Serial.print("Cible : ");
  Serial.print(TICKS_TARGET);
  Serial.print(" ticks pour ");
  Serial.print(DIST_MM);
  Serial.println(" mm. Pose le robot, GO pour avancer.");
}

void loop() {
  if (digitalRead(BTN_RESET) == LOW) {
    stopMotors();
    digitalWrite(STBY, LOW);
    digitalWrite(LED_STATE, LOW);
    state = IDLE;
    encCountR = 0;
    encCountL = 0;
    Serial.println("RESET");
    delay(300);
  }

  if (digitalRead(BTN_GO) == LOW && state == IDLE) {
    digitalWrite(STBY, HIGH);
    digitalWrite(LED_STATE, HIGH);
    encCountR = 0;
    encCountL = 0;
    headingDeg = 0;
    headingIntegral = 0;
    lastTime = millis();
    state = FORWARD;
    Serial.println("GO -> avance");
    delay(300);
  }

  if (state == FORWARD) {
    updateHeading();

    // 🧭 correction encodeurs : equilibre la distance parcourue par roue
    long error = abs(encCountL) - abs(encCountR);  // >0 => L en avance
    int correctionEnc = constrain((int)(error * KP_ENC), -MAX_CORRECTION, MAX_CORRECTION);


    // correction gyro : garde le cap a 0 (evite la derive laterale)
    // signe verifie empiriquement : celui-ci corrige, l'inverse fait partir
    // le robot franchement a gauche (boucle positive)
    // partie I : un biais residuel (asymetrie moteur, calibration gyro imparfaite)
    // n'est jamais annule par un P seul -- l'integrale le pousse a zero, evite la
    // derive en fin de parcours (l'erreur laterale d'un P seul croit en carre du temps)
    float headingError = headingDeg - HEADING_TRIM;   // ecart au cap de consigne, pas a 0
    headingIntegral = constrain(headingIntegral + headingError * lastDt, -INTEGRAL_LIMIT, INTEGRAL_LIMIT);
    int correctionGyro = constrain((int)(headingError * KP_GYRO + headingIntegral * KI_GYRO), -MAX_CORRECTION, MAX_CORRECTION);

    int correction = constrain(correctionEnc + correctionGyro, -MAX_CORRECTION, MAX_CORRECTION);

    // 🧭 : correction rééquilibrage d'un côté ou de l'autre
    // selon correction >0 ou <0, on ↘ la vitesse d'un moteur 
    // et ↗ l'autre
    motorA(-(SPEED + correction));
    motorB(SPEED - correction);

    // 🚧 v1 : les deux encodeurs perdent des ticks par intermittence moteurs ON. Un tick perdu
    // ne revient jamais, donc le compteur le plus HAUT est toujours le plus proche du vrai.
    // Remettre (|R|+|L|)/2 quand les encodeurs seront fiables (v2).
    long ticksR = abs(encCountR), ticksL = abs(encCountL);
    long avgTicks = ticksR > ticksL ? ticksR : ticksL;

    // DIAGNOSTIC : format lisible par le Serial Plotter (Tools -> Serial Plotter)
    // heading et correction sont sur une echelle comparable (±quelques dizaines), lisibles ensemble
    Serial.print("heading:");    Serial.print(headingDeg);
    Serial.print(",correction:"); Serial.println(correction);

    if (avgTicks >= TICKS_TARGET) {
      stopMotors();
      digitalWrite(STBY, LOW);
      digitalWrite(LED_STATE, LOW);
      state = DONE;
      Serial.print("Termine. encR=");
      Serial.print(encCountR);
      Serial.print(" encL=");
      Serial.print(encCountL);
      Serial.print(" -- mesure la distance reelle a la regle et compare a ");
      Serial.print(DIST_MM);
      Serial.println(" mm.");
    }
    delay(20);
  }
}

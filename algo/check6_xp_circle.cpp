#include <Wire.h>

// avance en faisant 1 tour de cercle complet (les 2 roues tournent vers
// l'avant, mais a des vitesses differentes -- ca courbe la trajectoire),
// puis stop des que le gyro mesure 360 degres de cap parcouru.
// SPEED_OUTER/SPEED_INNER regle le rayon du cercle et la vitesse globale :
// change juste ces 2 constantes pour tester lente / croisiere / fast.

const int MPU_ADDR = 0x68;
float headingDeg = 0;
float gyroZ_bias = 0;
unsigned long lastTime = 0;

const int PWMA = 2;
const int AIN1 = 3;
const int AIN2 = 4;

const int PWMB = 5;
const int BIN1 = 6;
const int BIN2 = 20;

const int STBY = 8;

const int BTN_GO = 17;
const int BTN_RESET = 22;

const int LED_STATE = 21;

// -- vitesses des 2 roues : le RAPPORT (pas la valeur absolue) fixe le rayon.
// rayon R = (L/2) * (Vext+Vint)/(Vext-Vint), L = ecart entre les 2 roues (~65mm).
// Valeurs choisies ici pour un rayon d'environ 15cm (diametre ~30cm),
// a verifier/ajuster car L est une estimation -- mesure-le sur ton chassis.
// exemple "lente"     : SPEED_OUTER=100, SPEED_INNER=64
// exemple "croisiere" : SPEED_OUTER=160, SPEED_INNER=103
// exemple "fast"      : SPEED_OUTER=220, SPEED_INNER=142
const int SPEED_OUTER = 160;  // roue exterieure du cercle (droite ici)
const int SPEED_INNER = 103;  // roue interieure -> rayon ~15cm

const float TARGET_ANGLE = 360.0;  // 1 tour complet

enum State { IDLE, CIRCLING, DONE };
State state = IDLE;

void motorA(int speed) {
  digitalWrite(AIN1, speed >= 0 ? HIGH : LOW);
  digitalWrite(AIN2, speed >= 0 ? LOW : HIGH);
  analogWrite(PWMA, abs(speed));
}

void motorB(int speed) {
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
  float gyroZ_dps = (gyroZraw / 131.0) - gyroZ_bias;

  unsigned long now = millis();
  float dt = (now - lastTime) / 1000.0;
  lastTime = now;
  headingDeg += gyroZ_dps * dt;
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

  Serial.println("Pose le robot avec de la place autour. GO pour faire 1 tour de cercle.");
}

void loop() {
  if (digitalRead(BTN_RESET) == LOW) {
    stopMotors();
    digitalWrite(STBY, LOW);
    digitalWrite(LED_STATE, LOW);
    state = IDLE;
    headingDeg = 0;
    Serial.println("RESET");
    delay(300);
  }

  if (digitalRead(BTN_GO) == LOW && state == IDLE) {
    digitalWrite(STBY, HIGH);
    digitalWrite(LED_STATE, HIGH);
    headingDeg = 0;
    lastTime = millis();
    state = CIRCLING;
    motorA(-SPEED_OUTER);  // roue droite
    motorB(SPEED_INNER);   // roue gauche, plus lente -> courbe vers la gauche
    Serial.println("GO -> cercle en cours...");
    delay(300);
  }

  if (state == CIRCLING) {
    updateHeading();
    Serial.println(headingDeg);

    if (abs(headingDeg) >= TARGET_ANGLE) {
      stopMotors();
      digitalWrite(STBY, LOW);
      digitalWrite(LED_STATE, LOW);
      state = DONE;
      Serial.print("Tour termine. Angle mesure au gyro : ");
      Serial.println(headingDeg);
      Serial.println("Mesure le rayon reel du cercle et compare a la trajectoire attendue. RESET pour recommencer.");
    }
    delay(20);
  }
}

#include <Wire.h>

// test isole : avance le long d'un mur droit -> detecte l'ouverture (confirmee sur
// plusieurs lectures, pas juste une) -> UN SEUL virage a droite -> stop complet

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

const int ENC_A_R = 9;
const int ENC_B_R = 10;
const int ENC_A_L = 11;
const int ENC_B_L = 12;
volatile long encCountR = 0;
volatile long encCountL = 0;

const int WHISKER_R = A0;

const int BTN_GO = 17;
const int BTN_RESET = 22;

const int LED_STATE = 21;

const int SPEED = 130;
const int TURN_SPEED = 100;
const float TURN_ANGLE = 90.0;

const float KP_ENC = 0.25;
const float KP_GYRO = 3.0;
const int MAX_CORRECTION = 40;

// -- A CALIBRER avec ton mini-troncon de test --
const int WALL_PRESENT_THRESHOLD = 500;
const int OPENING_CONFIRM_COUNT = 8;  // nb de lectures consecutives sans mur (x20ms)

// distance entre la moustache et le centre de rotation du robot -- A AJUSTER
// (mesure la distance reelle entre le bout de la moustache et le milieu du chassis)
const float MM_PER_TICK = 100.0 / 1151.0;
const long TICKS_APPROACH = (long)(90.0 / MM_PER_TICK);  // ~9cm, a ajuster encore si besoin

int openingCounter = 0;
int prevRight = 0;
bool sawFastDrop = false;
const int DROP_THRESHOLD = 100;  // delta minimal en 1 cycle pour compter comme "chute rapide" -- A CALIBRER

enum State { IDLE, DRIVING, APPROACH, TURNING, DONE };
State state = IDLE;

void encoderR_ISR() {
  if (digitalRead(ENC_B_R) == HIGH) encCountR++;
  else encCountR--;
}

void encoderL_ISR() {
  if (digitalRead(ENC_B_L) == HIGH) encCountL++;
  else encCountL--;
}

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

  Serial.println("Pose le robot le long du mur, ouverture devant lui. GO pour avancer.");
}

void loop() {
  if (digitalRead(BTN_RESET) == LOW) {
    stopMotors();
    digitalWrite(STBY, LOW);
    digitalWrite(LED_STATE, LOW);
    state = IDLE;
    openingCounter = 0;
    Serial.println("RESET");
    delay(300);
  }

  if (digitalRead(BTN_GO) == LOW && state == IDLE) {
    digitalWrite(STBY, HIGH);
    digitalWrite(LED_STATE, HIGH);
    encCountR = 0;
    encCountL = 0;
    headingDeg = 0;
    lastTime = millis();
    openingCounter = 0;
    state = DRIVING;
    Serial.println("GO -- avance le long du mur");
    delay(300);
  }

  if (state == DRIVING) {
    updateHeading();

    long error = abs(encCountL) - abs(encCountR);
    int correctionEnc = constrain((int)(error * KP_ENC), -MAX_CORRECTION, MAX_CORRECTION);
    int correctionGyro = constrain((int)(headingDeg * KP_GYRO), -MAX_CORRECTION, MAX_CORRECTION);
    int correction = constrain(correctionEnc + correctionGyro, -MAX_CORRECTION, MAX_CORRECTION);
    motorA(-(SPEED + correction));
    motorB(SPEED - correction);

    int right = analogRead(WHISKER_R);
    int delta = right - prevRight;
    prevRight = right;

    Serial.print(headingDeg); Serial.print('\t');
    Serial.print(right);      Serial.print('\t');
    Serial.print(delta);      Serial.print('\t');
    Serial.println(openingCounter);

    if (right < WALL_PRESENT_THRESHOLD) {
      if (openingCounter == 0) sawFastDrop = false;  // debut d'une nouvelle fenetre
      if (delta < -DROP_THRESHOLD) sawFastDrop = true;
      openingCounter++;
    } else {
      openingCounter = 0;
      sawFastDrop = false;
    }

    // confirme seulement si assez de lectures basses ET une chute rapide detectee
    if (openingCounter >= OPENING_CONFIRM_COUNT && sawFastDrop) {
      Serial.println("OUVERTURE CONFIRMEE -- avance encore un peu avant de tourner");
      encCountR = 0;
      encCountL = 0;
      state = APPROACH;
    }
    delay(20);
  }

  if (state == APPROACH) {
    updateHeading();

    long error = abs(encCountL) - abs(encCountR);
    int correctionEnc = constrain((int)(error * KP_ENC), -MAX_CORRECTION, MAX_CORRECTION);
    int correctionGyro = constrain((int)(headingDeg * KP_GYRO), -MAX_CORRECTION, MAX_CORRECTION);
    int correction = constrain(correctionEnc + correctionGyro, -MAX_CORRECTION, MAX_CORRECTION);
    motorA(-(SPEED + correction));
    motorB(SPEED - correction);

    long avgTicks = (abs(encCountR) + abs(encCountL)) / 2;
    if (avgTicks >= TICKS_APPROACH) {
      stopMotors();
      Serial.println("Position atteinte -- debut du virage");
      headingDeg = 0;
      lastTime = millis();
      state = TURNING;
      motorA(-TURN_SPEED);
      motorB(-TURN_SPEED);
    }
    delay(20);
  }

  if (state == TURNING) {
    updateHeading();
    Serial.println(headingDeg);

    if (abs(headingDeg) >= TURN_ANGLE) {
      stopMotors();
      digitalWrite(STBY, LOW);
      digitalWrite(LED_STATE, LOW);
      state = DONE;
      Serial.print("Virage termine, angle final : ");
      Serial.println(headingDeg);
      Serial.println("Sequence terminee. RESET pour recommencer.");
    }
    delay(20);
  }
}

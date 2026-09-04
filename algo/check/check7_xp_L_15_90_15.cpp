#include <Wire.h>

// parcours en L : 1 cellule (18cm) -> virage 90 (gyro) -> 1 cellule (18cm) -> stop complet
// distance = pas de cellule TMIRC (centre a centre), pas la largeur libre du couloir

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

const int BTN_GO = 17;
const int BTN_RESET = 22;

const int LED_STATE = 21;

const int SPEED = 130;
const int TURN_SPEED = 100;
const float TARGET_ANGLE = 90.0;

const float KP_ENC = 0.25;      // gain correction encodeurs (distance)
const float KP_GYRO = 3.0;      // gain correction gyro (cap)
const int MAX_CORRECTION = 40;

const float MM_PER_TICK = 100.0 / 1151.0;
// pas de cellule TMIRC = 18cm (centre a centre), pas la largeur libre du couloir (16.8cm)
const long TICKS_CELL = (long)(180.0 / MM_PER_TICK);

enum State { IDLE, LEG1, TURNING, LEG2, DONE };
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

void startLeg() {
  encCountR = 0;
  encCountL = 0;
  headingDeg = 0;
  lastTime = millis();
}

void driveStraightCorrected() {
  updateHeading();

  long error = abs(encCountL) - abs(encCountR);
  int correctionEnc = constrain((int)(error * KP_ENC), -MAX_CORRECTION, MAX_CORRECTION);
  int correctionGyro = constrain((int)(headingDeg * KP_GYRO), -MAX_CORRECTION, MAX_CORRECTION);
  int correction = constrain(correctionEnc + correctionGyro, -MAX_CORRECTION, MAX_CORRECTION);

  motorA(-(SPEED + correction));
  motorB(SPEED - correction);
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

  Serial.print("Cible par jambe : ");
  Serial.print(TICKS_CELL);
  Serial.println(" ticks (1 cellule, 18cm). Pose le robot, GO pour lancer le L.");
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
    state = LEG1;
    startLeg();
    Serial.println("GO -> jambe 1 (1 cellule)");
    delay(300);
  }

  if (state == LEG1) {
    driveStraightCorrected();
    long avgTicks = (abs(encCountR) + abs(encCountL)) / 2;
    Serial.print("LEG1\t"); Serial.print(headingDeg); Serial.print('\t');
    Serial.print(encCountR); Serial.print('\t'); Serial.println(encCountL);

    if (avgTicks >= TICKS_CELL) {
      stopMotors();
      headingDeg = 0;
      lastTime = millis();
      state = TURNING;
      motorA(-TURN_SPEED);
      motorB(-TURN_SPEED);
      Serial.println("Jambe 1 terminee -- debut du virage");
    }
    delay(20);
  }

  if (state == TURNING) {
    updateHeading();
    Serial.print("TURN\t"); Serial.println(headingDeg);

    if (abs(headingDeg) >= TARGET_ANGLE) {
      stopMotors();
      Serial.print("Virage termine, angle final : ");
      Serial.println(headingDeg);
      state = LEG2;
      startLeg();
      Serial.println("Debut jambe 2 (1 cellule)");
    }
    delay(20);
  }

  if (state == LEG2) {
    driveStraightCorrected();
    long avgTicks = (abs(encCountR) + abs(encCountL)) / 2;
    Serial.print("LEG2\t"); Serial.print(headingDeg); Serial.print('\t');
    Serial.print(encCountR); Serial.print('\t'); Serial.println(encCountL);

    if (avgTicks >= TICKS_CELL) {
      stopMotors();
      digitalWrite(STBY, LOW);
      digitalWrite(LED_STATE, LOW);
      state = DONE;
      Serial.println("Parcours en L termine. Mesure la position finale et compare. RESET pour recommencer.");
    }
    delay(20);
  }
}

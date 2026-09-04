// avance tout droit sur 30cm (en distance calculee via les encodeurs), puis stop complet
// correction combinee : encodeurs (distance) + gyro (cap reel, evite la derive laterale)

#include <Wire.h>

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

const int SPEED = 130;  // 0-255
const float KP_ENC = 0.25;      // gain correction encodeurs (distance)
const float KP_GYRO = 3.0;      // gain correction gyro (cap), a ajuster
const int MAX_CORRECTION = 40;  // limite de correction, evite les a-coups

// calibration : CPR ~1151, circonference roue ~100mm (Ø32mm)
const float MM_PER_TICK = 100.0 / 1151.0;
const long TICKS_TARGET = (long)(300.0 / MM_PER_TICK);  // 30cm

enum State { IDLE, FORWARD, DONE };
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

  Serial.print("Cible : ");
  Serial.print(TICKS_TARGET);
  Serial.println(" ticks pour 30cm. Pose le robot, GO pour avancer.");
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
    lastTime = millis();
    state = FORWARD;
    Serial.println("GO -> avance");
    delay(300);
  }

  if (state == FORWARD) {
    updateHeading();

    // correction encodeurs : equilibre la distance parcourue par roue
    long error = abs(encCountL) - abs(encCountR);  // >0 => L en avance
    int correctionEnc = constrain((int)(error * KP_ENC), -MAX_CORRECTION, MAX_CORRECTION);

    // correction gyro : garde le cap a 0 (evite la derive laterale)
    // si le robot a devie, headingDeg s'ecarte de 0 -- ajuste en consequence
    // si la derive empire au lieu de se corriger, inverse le signe ici
    int correctionGyro = constrain((int)(headingDeg * KP_GYRO), -MAX_CORRECTION, MAX_CORRECTION);

    int correction = constrain(correctionEnc + correctionGyro, -MAX_CORRECTION, MAX_CORRECTION);

    motorA(-(SPEED + correction));
    motorB(SPEED - correction);

    long avgTicks = (abs(encCountR) + abs(encCountL)) / 2;

    Serial.print(headingDeg); Serial.print('\t');
    Serial.print(encCountR); Serial.print('\t');
    Serial.print(encCountL); Serial.print('\t');
    Serial.print(avgTicks);  Serial.print('\t');
    Serial.println(correction);

    if (avgTicks >= TICKS_TARGET) {
      stopMotors();
      digitalWrite(STBY, LOW);
      digitalWrite(LED_STATE, LOW);
      state = DONE;
      Serial.print("Termine. encR=");
      Serial.print(encCountR);
      Serial.print(" encL=");
      Serial.print(encCountL);
      Serial.println(" -- mesure la distance reelle a la regle et compare a 30cm.");
    }
    delay(20);
  }
}

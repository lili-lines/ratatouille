#include <Wire.h>

// avance -> detecte l'ouverture (relachement brusque de la moustache)
// au premier croisement -> vrai virage 90 (gyro) -> stop complet (fin du test)

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

const int WHISKER1 = A0;  // droite
const int WHISKER2 = A2;  // gauche

const int BTN_GO = 17;
const int BTN_RESET = 22;

const int LED_STATE = 21;

const int SPEED = 130;       // 0-255, avance tout droit
const int TURN_SPEED = 100;  // 0-255, vitesse du pivot
const int RELEASE_THRESHOLD = 150;
const float TARGET_ANGLE = 90.0;

enum State { IDLE, FORWARD, TURNING, DONE };
State state = IDLE;

int prevWhisker1 = 0;
int prevWhisker2 = 0;
int turnDir = -1;  // -1 = droite, 1 = gauche

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

  prevWhisker1 = analogRead(WHISKER1);
  prevWhisker2 = analogRead(WHISKER2);
  lastTime = millis();
  Serial.println("Pose le robot dans le mini labyrinthe. GO pour avancer.");
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
    prevWhisker1 = analogRead(WHISKER1);
    prevWhisker2 = analogRead(WHISKER2);
    state = FORWARD;
    motorA(-SPEED);
    motorB(SPEED);
    Serial.println("GO -> avance");
    delay(300);
  }

  if (state == FORWARD) {
    updateHeading();
    int value1 = analogRead(WHISKER1);
    int value2 = analogRead(WHISKER2);
    int delta1 = value1 - prevWhisker1;
    int delta2 = value2 - prevWhisker2;
    prevWhisker1 = value1;
    prevWhisker2 = value2;

    Serial.print(headingDeg); Serial.print('\t');
    Serial.print(value1);     Serial.print('\t');
    Serial.print(delta1);     Serial.print('\t');
    Serial.print(value2);     Serial.print('\t');
    Serial.println(delta2);

    if (delta1 > RELEASE_THRESHOLD) {
      Serial.println("OUVERTURE A DROITE -- virage a droite");
      stopMotors();
      headingDeg = 0;
      lastTime = millis();
      state = TURNING;
      turnDir = -1;
      motorA(-TURN_SPEED);
      motorB(-TURN_SPEED);
    } else if (delta2 > RELEASE_THRESHOLD) {
      Serial.println("OUVERTURE A GAUCHE -- virage a gauche");
      stopMotors();
      headingDeg = 0;
      lastTime = millis();
      state = TURNING;
      turnDir = 1;
      motorA(TURN_SPEED);
      motorB(TURN_SPEED);
    }
    delay(20);
  }

  if (state == TURNING) {
    updateHeading();
    Serial.println(headingDeg);

    if (abs(headingDeg) >= TARGET_ANGLE) {
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

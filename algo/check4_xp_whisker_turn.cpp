#include <Wire.h>

// Test combine : moustaches -> virage.
// - Moustache DROITE ou GAUCHE : un virage 90 degres du meme cote,
//   seulement declenche par un RELACHEMENT VIF (chute rapide de la valeur).
//   Un pli lent ou un pli tout court ne declenche rien.
// - Moustache AVANT : un virage 360 degres, declenche par N'IMPORTE QUEL
//   mouvement (pli ou relachement, vif ou lent).

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

const int WHISKER_R = A0;  // droite
const int WHISKER_F = A1;  // avant
const int WHISKER_L = A2;  // gauche

// -- seuils, a garder synchronises avec xp_wall_follower.cpp / check4_xp_whisker.cpp --
const int DROP_THRESHOLD = 400;   // relachement VIF (gauche/droite) : chute >= ce delta
const int MOVE_THRESHOLD = 120;   // n'importe quel mouvement (avant) : |delta| >= ce seuil

const int TURN_SPEED = 100;  // 0-255

int prevRight = 0;
int prevFront = 0;
int prevLeft = 0;

enum State { IDLE, TURNING, DONE };
State state = IDLE;
float targetAngle = 0;
int turnSign = 1; // +1 = un sens, -1 = l'autre (a verifier/inverser selon le cablage reel)

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

void startTurn(float angle, int sign, const char* why) {
  digitalWrite(STBY, HIGH);
  headingDeg = 0;
  lastTime = millis();
  targetAngle = angle;
  turnSign = sign;
  state = TURNING;
  motorA(sign * TURN_SPEED);
  motorB(-sign * TURN_SPEED);
  Serial.print(why);
  Serial.print(" -> virage ");
  Serial.print(angle);
  Serial.println(" en cours...");
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);   // moustaches lues sur 0..4095
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

  prevRight = analogRead(WHISKER_R);
  prevFront = analogRead(WHISKER_F);
  prevLeft = analogRead(WHISKER_L);

  Serial.println("Pret. Plie/relache une moustache pour declencher un virage.");
}

void loop() {
  int right = analogRead(WHISKER_R);
  int front = analogRead(WHISKER_F);
  int left = analogRead(WHISKER_L);

  int deltaRight = right - prevRight;
  int deltaFront = front - prevFront;
  int deltaLeft = left - prevLeft;

  prevRight = right;
  prevFront = front;
  prevLeft = left;

  if (state == IDLE) {
    // Droite : relachement vif -> virage 90 a droite
    if (deltaRight <= -DROP_THRESHOLD) {
      startTurn(90.0, +1, "Moustache DROITE, relachement vif");
    }
    // Gauche : relachement vif -> virage 90 a gauche
    else if (deltaLeft <= -DROP_THRESHOLD) {
      startTurn(90.0, -1, "Moustache GAUCHE, relachement vif");
    }
    // Avant : n'importe quel mouvement (pli OU relachement, vif ou lent) -> 360
    else if (abs(deltaFront) >= MOVE_THRESHOLD) {
      startTurn(360.0, +1, "Moustache AVANT, mouvement detecte");
    }
  }

  if (state == TURNING) {
    updateHeading();
    if (abs(headingDeg) >= targetAngle) {
      stopMotors();
      digitalWrite(STBY, LOW);
      state = DONE;
      Serial.print("Virage termine. Angle mesure au gyro : ");
      Serial.println(headingDeg);
    }
  }

  if (state == DONE) {
    // petite pause avant de re-armer, pour ne pas redeclencher sur le rebond de la moustache
    delay(500);
    state = IDLE;
    prevRight = analogRead(WHISKER_R);
    prevFront = analogRead(WHISKER_F);
    prevLeft = analogRead(WHISKER_L);
    Serial.println("Pret pour un nouveau virage.");
  }

  delay(20);
}

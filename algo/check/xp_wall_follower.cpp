#include <Wire.h>

// suivi de mur (main droite) -- algo de repli, plus simple/robuste qu'un floodfill
// avance en longeant le mur droit, tourne a droite des qu'une ouverture est CONFIRMEE
// (plusieurs lectures consecutives sans mur, pas juste une), sinon tout droit,
// si mur devant (stall) -> tente la gauche -> sinon demi-tour

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
const int WHISKER_L = A2;

const int BTN_GO = 17;
const int BTN_RESET = 22;

const int LED_STATE = 21;

const int SPEED = 130;
const int TURN_SPEED = 100;
const float TURN_ANGLE = 90.0;

const float KP_ENC = 0.25;
const float KP_GYRO = 3.0;      // utilise seulement pendant approachThenTurn (court, apres alignement)
const float KP_WALL = 0.15;     // pilotage sur la distance au mur droit, utilise en suivi normal -- A CALIBRER
const int MAX_CORRECTION = 40;

// valeur moustache droite visee en suivi normal (a mi-chemin entre "mur colle" et "pas de mur")
// -- A CALIBRER : pose le robot a la distance de suivi ideale, lis la valeur, mets-la ici
const int WALL_FOLLOW_TARGET = 1200;

// -- A CALIBRER avec ton mini-tronco de test --
const int WALL_PRESENT_THRESHOLD = 2000;  // au-dessus = mur present (moustache pliee)
const int OPENING_CONFIRM_COUNT = 8;      // nb de lectures consecutives sans mur pour confirmer (x20ms)
const unsigned long STALL_TIMEOUT_MS = 400;

// distance entre la moustache et le centre de rotation -- validee sur check10 (~9cm)
const float MM_PER_TICK = 100.0 / 1151.0;
const long TICKS_APPROACH = (long)(90.0 / MM_PER_TICK);

int openingCounter = 0;
int prevRight = 0;
bool sawFastDrop = false;
const int DROP_THRESHOLD = 400;  // delta minimal en 1 cycle pour compter comme "chute rapide" -- A CALIBRER

enum State { IDLE, DRIVING };
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

void turn90(int dir);  // declaration anticipee, utilisee par approachThenTurn ci-dessous

// avance encore TICKS_APPROACH avant de tourner, pour que le centre de rotation
// du robot (pas juste la moustache) soit bien dans l'ouverture
void approachThenTurn(int dir) {
  encCountR = 0;
  encCountL = 0;
  headingDeg = 0;
  lastTime = millis();

  long avgTicks = 0;
  while (avgTicks < TICKS_APPROACH) {
    updateHeading();

    long error = abs(encCountL) - abs(encCountR);
    int correctionEnc = constrain((int)(error * KP_ENC), -MAX_CORRECTION, MAX_CORRECTION);
    int correctionGyro = constrain((int)(headingDeg * KP_GYRO), -MAX_CORRECTION, MAX_CORRECTION);
    int correction = constrain(correctionEnc + correctionGyro, -MAX_CORRECTION, MAX_CORRECTION);
    motorA(-(SPEED + correction));
    motorB(SPEED - correction);

    avgTicks = (abs(encCountR) + abs(encCountL)) / 2;
    delay(20);
  }
  stopMotors();
  turn90(dir);
}

// pivot 90, dir = 1 (droite) ou -1 (gauche)
void turn90(int dir) {
  headingDeg = 0;
  lastTime = millis();
  motorA(dir * -TURN_SPEED);
  motorB(dir * -TURN_SPEED);
  while (abs(headingDeg) < TURN_ANGLE) {
    updateHeading();
    delay(10);
  }
  stopMotors();
  delay(100);
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

  Serial.println("Pose le robot le long d'un mur droit. GO pour suivre.");
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
    Serial.println("GO -- suivi de mur droit");
    delay(300);
  }

  if (state == DRIVING) {
    updateHeading();

    int right = analogRead(WHISKER_R);
    int delta = right - prevRight;
    prevRight = right;    // correction encodeurs (equilibre les 2 roues) + correction mur (garde la distance
    // au mur droit constante, via la valeur continue de la moustache -- PAS le gyro ici :
    // tenir un cap fige ne corrige pas la distance reelle au mur, seul le suivi mur le fait)
    long error = abs(encCountL) - abs(encCountR);
    int correctionEnc = constrain((int)(error * KP_ENC), -MAX_CORRECTION, MAX_CORRECTION);
    int wallError = right - WALL_FOLLOW_TARGET;  // >0 = trop pres du mur, <0 = trop loin
    // signe inverse : va vers le mur au lieu de s'en eloigner -- corrige ici
    int correctionWall = constrain((int)(-wallError * KP_WALL), -MAX_CORRECTION, MAX_CORRECTION);
    int correction = constrain(correctionEnc + correctionWall, -MAX_CORRECTION, MAX_CORRECTION);
    motorA(-(SPEED + correction));
    motorB(SPEED - correction);

    Serial.print(headingDeg); Serial.print('\t');
    Serial.print(right);      Serial.print('\t');
    Serial.print(delta);      Serial.print('\t');
    Serial.println(openingCounter);

    // ouverture a droite : confirmee seulement apres plusieurs lectures consecutives
    // ET une chute rapide (derivee) -- distingue une vraie ouverture d'une derive lente
    if (right < WALL_PRESENT_THRESHOLD) {
      if (openingCounter == 0) sawFastDrop = false;
      if (delta < -DROP_THRESHOLD) sawFastDrop = true;
      openingCounter++;
    } else {
      openingCounter = 0;
      sawFastDrop = false;
    }

    if (openingCounter >= OPENING_CONFIRM_COUNT && sawFastDrop) {
      Serial.println("OUVERTURE DROITE CONFIRMEE -- avance puis virage");
      approachThenTurn(1);
      encCountR = 0;
      encCountL = 0;
      headingDeg = 0;
      lastTime = millis();
      openingCounter = 0;
      sawFastDrop = false;
    }

    // mur devant : stall si pas d'avancee prolongee
    static long lastTicks = 0;
    static unsigned long lastProgress = 0;
    long avgTicks = (abs(encCountR) + abs(encCountL)) / 2;
    if (avgTicks > lastTicks) {
      lastTicks = avgTicks;
      lastProgress = millis();
    }
    if (lastProgress == 0) lastProgress = millis();

    if (millis() - lastProgress > STALL_TIMEOUT_MS) {
      stopMotors();
      int left = analogRead(WHISKER_L);
      if (left < WALL_PRESENT_THRESHOLD) {
        Serial.println("MUR DEVANT -- ouverture a gauche -- virage gauche");
        turn90(-1);
      } else {
        Serial.println("MUR DEVANT -- cul-de-sac -- demi-tour");
        turn90(1);
        turn90(1);
      }
      encCountR = 0;
      encCountL = 0;
      headingDeg = 0;
      lastTime = millis();
      lastTicks = 0;
      lastProgress = millis();
      openingCounter = 0;
    }

    delay(20);
  }
}

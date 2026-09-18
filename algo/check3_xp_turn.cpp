#include <Wire.h>

// test isole du virage 90 (pivot), moteur reel -- compare l'angle final
// affiche au vrai angle mesure au rapporteur/repere au sol

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

const int TURN_SPEED = 100;  // 0-255
const float TARGET_ANGLE = 360.0; // 360.0 90.0, 45.0

// 🧭 Facteur d'echelle du gyro. La sensibilite nominale (131 LSB/deg/s) a une
// tolerance de +-3 % selon la datasheet : chaque puce devie un peu.
// Calibration : viser 360, mesurer l'angle REEL au rapporteur, puis
//   GYRO_SCALE = GYRO_SCALE_actuel * angle_reel / 360   (en partant de 1.0 la 1re fois)
// Historique : 365/360 -> le robot faisait encore 363 reel
// -> 365/360 * 363/360 = 1.0223 👍 pas mal
// -> 365/360 * 364/360 = 1.0251 🚧 todo essayer
const float GYRO_SCALE = 1.0223;
// pour 360 il fait legerement +
// 90 même rmq

enum State { IDLE, TURNING, DONE };
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
  
  // 🧭 GYRO_SCALE corrige la sensibilite reelle de la puce (voir la constante en haut)
  float gyroZ_dps = ((gyroZraw / 131.0) - gyroZ_bias) * GYRO_SCALE;

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

  // force le gyro en +-250 deg/s (FS_SEL=0), sensibilite 131 LSB/(deg/s) --
  // sans ca, un reglage herite d'un test precedent fausserait l'echelle de headingDeg
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x1B);
  Wire.write(0x00);
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

  Serial.println("Pose le robot sur ton repere/rapporteur (position 0). GO pour pivoter 90.");
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
    delay(300);  // anti-rebond AVANT de demarrer : sinon les 300 premieres ms de rotation
                 // sont integrees d'un seul coup avec la vitesse de fin, alors que le robot accelerait
    digitalWrite(STBY, HIGH);
    digitalWrite(LED_STATE, HIGH);
    headingDeg = 0;
    lastTime = millis();
    state = TURNING;
    motorA(-TURN_SPEED);
    motorB(-TURN_SPEED);
    Serial.println("GO -> pivot en cours...");
  }

  if (state == TURNING) {
    updateHeading();
    Serial.println(headingDeg);

    if (abs(headingDeg) >= TARGET_ANGLE) {
      stopMotors();
      digitalWrite(STBY, LOW);
      digitalWrite(LED_STATE, LOW);
      state = DONE;
      Serial.print("Virage termine. Angle mesure au gyro : ");
      Serial.println(headingDeg);
      Serial.println("Compare au vrai angle sur ton repere/rapporteur. RESET pour recommencer.");
    }
    delay(1);  // baisse encore -- le plancher reel est le temps de lecture I2C (~qq centaines de µs)
  }
}

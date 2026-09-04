#include <Wire.h>
#include <SPI.h>
#include <SD.h>

// ---- gyro ----
const int MPU_ADDR = 0x68;
float headingDeg = 0;
float gyroZ_bias = 0;
unsigned long lastTime = 0;

// ---- moteur A (droit) ----
const int PWMA = 2;
const int AIN1 = 3;
const int AIN2 = 4;

// ---- moteur B (gauche) ----
const int PWMB = 5;
const int BIN1 = 6;
const int BIN2 = 20;

const int STBY = 8;

// ---- encodeurs ----
const int ENC_A_R = 9;
const int ENC_B_R = 10;
const int ENC_A_L = 11;
const int ENC_B_L = 12;
volatile long encCountR = 0;
volatile long encCountL = 0;

// ---- moustaches ----
const int WHISKER1 = A0;
const int WHISKER2 = A1;
const int WHISKER3 = A2;

// ---- boutons ----
const int BTN_GO = 17;
const int BTN_RESET = 22;
const int BTN_RETRY = 0;

// ---- LED d'état ----
const int LED_STATE = 21;

// ---- SD (SPI, pins alternatives) ----
const int SD_CS = 23;

void encoderR_ISR() {
  if (digitalRead(ENC_B_R) == HIGH) encCountR++;
  else encCountR--;
}

void encoderL_ISR() {
  if (digitalRead(ENC_B_L) == HIGH) encCountL++;
  else encCountL--;
}

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // gyro
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  // encodeurs
  pinMode(ENC_A_R, INPUT);
  pinMode(ENC_B_R, INPUT);
  pinMode(ENC_A_L, INPUT);
  pinMode(ENC_B_L, INPUT);
  attachInterrupt(digitalPinToInterrupt(ENC_A_R), encoderR_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_A_L), encoderL_ISR, RISING);

  // driver moteur -- STBY LOW au demarrage, moteurs endormis par securite
  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(STBY, OUTPUT);
  digitalWrite(STBY, LOW);
  analogWrite(PWMA, 0);
  analogWrite(PWMB, 0);

  // boutons
  pinMode(BTN_GO, INPUT_PULLUP);
  pinMode(BTN_RESET, INPUT_PULLUP);
  pinMode(BTN_RETRY, INPUT_PULLUP);

  // LED
  pinMode(LED_STATE, OUTPUT);
  digitalWrite(LED_STATE, LOW);

  delay(500);

  // calibration gyro
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

  // SD, SPI matériel sur pins alternatives (7/1/13 au lieu de 11/12/13)
  SPI.setMOSI(7);
  SPI.setMISO(1);
  SPI.setSCK(13);
  if (!SD.begin(SD_CS)) {
    Serial.println("Erreur : carte SD non detectee");
  } else {
    Serial.println("SD OK");
  }

  // SD.begin() peut reconfigurer 11/12 en mode SPI malgre le remap --
  // on reprend la main dessus pour l'encodeur gauche
  pinMode(ENC_A_L, INPUT);
  pinMode(ENC_B_L, INPUT);
  attachInterrupt(digitalPinToInterrupt(ENC_A_L), encoderL_ISR, RISING);

  lastTime = millis();
  Serial.println("Setup termine. Appuie sur GO pour reveiller le driver.");
}

void loop() {
  // gyro
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

  // moustaches
  int w1 = analogRead(WHISKER1);
  int w2 = analogRead(WHISKER2);
  int w3 = analogRead(WHISKER3);

  // boutons -- LOW = appuye (pull-up)
  if (digitalRead(BTN_RESET) == LOW) {
    headingDeg = 0;
    encCountR = 0;
    encCountL = 0;
    digitalWrite(STBY, LOW);
    digitalWrite(LED_STATE, LOW);
    Serial.println("RESET");
  }

  if (digitalRead(BTN_GO) == LOW) {
    digitalWrite(STBY, HIGH);   // reveille le driver
    digitalWrite(LED_STATE, HIGH);
    Serial.println("GO");
  }

  if (digitalRead(BTN_RETRY) == LOW) {
    Serial.println("RETRY");
  }

  // rapport complet, une ligne par cycle
  Serial.print(headingDeg);   Serial.print('\t');
  Serial.print(encCountR);    Serial.print('\t');
  Serial.print(encCountL);    Serial.print('\t');
  Serial.print(w1);           Serial.print('\t');
  Serial.print(w2);           Serial.print('\t');
  Serial.println(w3);

  delay(20);
}

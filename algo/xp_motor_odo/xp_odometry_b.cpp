#include <Wire.h>

const int MPU_ADDR = 0x68;
float headingDeg = 0;
float gyroZ_bias = 0;
unsigned long lastTime = 0;

const int ENC_A = 9;
const int ENC_B = 10;
volatile long encoderCount = 0;

const int PWMA = 2;
const int AIN1 = 3;
const int AIN2 = 4;
const int STBY = 8;

// baseline = 0 (moteur OFF), puis re-upload avec ex. 130 pour le test moteur ON
const int MOTOR_SPEED = 0;

void encoderISR() {
  if (digitalRead(ENC_B) == HIGH) encoderCount++;
  else encoderCount--;
}

void setup() {
  Serial.begin(115200);
  Wire.begin();

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  pinMode(ENC_A, INPUT);
  pinMode(ENC_B, INPUT);
  attachInterrupt(digitalPinToInterrupt(ENC_A), encoderISR, RISING);

  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(STBY, OUTPUT);

  digitalWrite(STBY, HIGH);
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  analogWrite(PWMA, MOTOR_SPEED);

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
  Serial.print("Biais mesure : ");
  Serial.println(gyroZ_bias);

  lastTime = millis();
}

void loop() {
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

  Serial.print(headingDeg);
  Serial.print('\t');
  Serial.println(encoderCount);

  delay(20);
}

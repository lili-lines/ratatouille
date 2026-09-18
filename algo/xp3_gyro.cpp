#include <Wire.h>

const int MPU_ADDR = 0x68;
float headingDeg = 0;
unsigned long lastTime = 0;

const int ENC_A = 9;
const int ENC_B = 10;
volatile long encoderCount = 0;

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

  lastTime = millis();
}

void loop() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x47);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 2, true);
  int16_t gyroZraw = Wire.read() << 8 | Wire.read();

  float gyroZ_dps = gyroZraw / 131.0;

  unsigned long now = millis();
  float dt = (now - lastTime) / 1000.0;
  lastTime = now;

  headingDeg += gyroZ_dps * dt;

  Serial.print(headingDeg);
  Serial.print('\t');
  Serial.println(encoderCount);

  delay(20);
}

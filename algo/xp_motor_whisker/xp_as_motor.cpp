const int PWMA = 2;
const int AIN1 = 3;
const int AIN2 = 4;
const int STBY = 8;
const int WHISKER = A0;
const int ENC_A = 9;
const int ENC_B = 10;

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);   // moustache lue sur 0..4095

  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(STBY, OUTPUT);
  pinMode(ENC_A, INPUT);
  pinMode(ENC_B, INPUT);

  digitalWrite(STBY, HIGH);
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  analogWrite(PWMA, 130);  // mettre (PWMA, 0) pour stop motor
}
// vitesse du motor (?) 0-255

void loop() {
  int whisker = analogRead(WHISKER);
  Serial.println(whisker);
  delay(20);
}

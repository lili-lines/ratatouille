#include <Wire.h>

// Test combine : moustaches -> virage. Version simple : un MOUVEMENT de moustache declenche.
// - DROITE bouge : virage 90 a droite
// - GAUCHE bouge : virage 90 a gauche
// - AVANT bouge dans le sens FRONT_SIGN : virage 180 (l'autre sens est ignore)
//
// "Bouge" = |valeur - repos| > CONTACT_ON, d'apres l'enregistrement xp1_as5600_log_1789539115
// (post "The whisker", section 2) :
//   . repos : ~2350-2560 selon la moustache, mesure au demarrage
//   . une pousse ecarte la valeur du repos de 255 a 470 counts, le bruit au repos fait ~90 crete-a-crete
//   . le SENS depend de la moustache (droite baisse, gauche monte) -> valeur absolue
// Pas de distinction pousse / relachement / vif / lent pour ce test.

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
const int CONTACT_ON  = 150;     // |valeur - repos| au-dela : ca bouge (pousse mesuree >= 255, bruit <= 45)
const int CONTACT_OFF = 100;     // et en dessous : revenu au repos (hysteresis, evite le clignotement)
const int REST_SAMPLES = 50;     // lectures moyennees au demarrage pour le repos
const int FRONT_SIGN  = +1;      // l'avant ne reagit que dans un sens : -1 = valeur qui BAISSE, +1 = qui MONTE (repos ~2100, pousse -> ~2700)

const int LOOP_MS = 20;
const int TURN_SPEED = 100;  // 0-255

const unsigned long STUCK_MS = 3000;  // ecartee plus longtemps que ca = l'aimant a glisse, c'est le nouveau repos

struct Whisker {
  int pin;
  const char* name;
  float rest;               // valeur au repos, mesuree au demarrage puis suivie lentement
  bool moved;               // ecartee du repos en ce moment ?
  unsigned long movedSince; // depuis quand
};

Whisker wR = {WHISKER_R, "DROITE", 0, false, 0};
Whisker wF = {WHISKER_F, "AVANT",  0, false, 0};
Whisker wL = {WHISKER_L, "GAUCHE", 0, false, 0};

// vrai une seule fois, au moment ou la moustache s'ecarte du repos.
// sign = 0 : dans n'importe quel sens ; -1 / +1 : seulement si la valeur baisse / monte
bool whiskerMoved(Whisker& w, int sign = 0) {
  int v = analogRead(w.pin);
  int delta = v - (int)w.rest;
  int dev = sign == 0 ? abs(delta) : delta * sign;   // negatif si mauvais sens -> jamais > CONTACT_ON

  if (!w.moved) {
    w.rest += (v - w.rest) * 0.01;   // au repos, le repos suit la derive lente (tau ~2 s)
    if (dev > CONTACT_ON) {
      w.moved = true;
      w.movedSince = millis();
      Serial.print(w.name); Serial.print(" bouge, ecart="); Serial.println(dev);
      return true;
    }
    return false;
  }

  if (dev < CONTACT_OFF) {
    w.moved = false;                 // revenue au repos
  } else if (millis() - w.movedSince > STUCK_MS) {
    w.rest = v;                      // l'aimant a glisse : on repart de la
    w.moved = false;
    Serial.print(w.name); Serial.print(" : nouveau repos = "); Serial.println(v);
  }
  return false;
}

int measureRest(int pin) {
  long sum = 0;
  for (int i = 0; i < REST_SAMPLES; i++) {
    sum += analogRead(pin);
    delay(5);
  }
  return sum / REST_SAMPLES;
}

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
  // moteurs montes en miroir : avancer = A(-v) B(+v). Pour pivoter, MEME signe sur les deux
  // (roues en sens contraire au sol), comme check3. +1 : roue droite recule, gauche avance -> a droite
  motorA(sign * TURN_SPEED);
  motorB(sign * TURN_SPEED);
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

  Serial.println("Mesure du repos des moustaches, ne les touche pas...");
  wR.rest = measureRest(wR.pin);
  wF.rest = measureRest(wF.pin);
  wL.rest = measureRest(wL.pin);
  Serial.print("Repos : droite="); Serial.print(wR.rest);
  Serial.print("  avant=");        Serial.print(wF.rest);
  Serial.print("  gauche=");       Serial.println(wL.rest);

  Serial.println("Pret. Plie/relache une moustache pour declencher un virage.");
}

void loop() {
  // les etats sont suivis en permanence, meme pendant un virage, pour ne pas
  // redeclencher sur un mouvement qui date d'avant
  bool mR = whiskerMoved(wR);
  bool mF = whiskerMoved(wF, FRONT_SIGN);
  bool mL = whiskerMoved(wL);

  if (state == IDLE) {
    // DIAGNOSTIC : valeurs brutes toutes les 250 ms, pour voir si une moustache bouge vraiment
    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 250) {
      lastPrint = millis();
      Serial.print("R="); Serial.print(analogRead(wR.pin)); Serial.print("/"); Serial.print((int)wR.rest); Serial.print(wR.moved ? "*" : " ");
      Serial.print("  F="); Serial.print(analogRead(wF.pin)); Serial.print("/"); Serial.print((int)wF.rest); Serial.print(wF.moved ? "*" : " ");
      Serial.print("  L="); Serial.print(analogRead(wL.pin)); Serial.print("/"); Serial.print((int)wL.rest); Serial.println(wL.moved ? "*" : " ");
    }

    if (mR)      startTurn(90.0,  +1, "Moustache DROITE");
    else if (mL) startTurn(90.0,  -1, "Moustache GAUCHE");
    else if (mF) startTurn(180.0, +1, "Moustache AVANT");
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
    // petite pause avant de re-armer, pour ne pas redeclencher sur le rebond de la moustache.
    // On ne remet PAS moved a false : une moustache encore ecartee redeclencherait aussitot.
    // L'hysteresis s'en charge quand elle revient au repos.
    delay(500);
    state = IDLE;
    Serial.println("Pret pour un nouveau virage.");
  }

  delay(LOOP_MS);   // la derivee suppose cette periode, ne pas changer l'un sans l'autre
}

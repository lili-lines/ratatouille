#include <Wire.h>

// resolution de labyrinthe TMIRC (16x16, cellule 18cm) par floodfill
// NON TESTE EN CONDITIONS REELLES -- a valider etape par etape comme le reste du projet :
//  1. d'abord juste senseWalls() (poser le robot dans une cellule, verifier au Serial
//     Monitor que les murs detectes correspondent a la realite)
//  2. puis un seul deplacement (moveForwardOneCell / turnTo)
//  3. puis l'exploration complete

// ---------------- pins ----------------
const int MPU_ADDR = 0x68;

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

const int WHISKER_R = A0;  // droite
const int WHISKER_F = A1;  // avant -- A AJUSTER si la 3e moustache n'est pas montee a l'avant
const int WHISKER_L = A2;  // gauche

const int BTN_GO = 17;
const int BTN_RESET = 22;
const int BTN_RETRY = 0;

const int LED_STATE = 21;

// ---------------- calibration -- A AJUSTER avec tes vraies mesures ----------------
const int SPEED = 130;
const int TURN_SPEED = 100;
const float TURN_ANGLE = 90.0;

const float KP_ENC = 0.25;
const float KP_GYRO = 3.0;
const int MAX_CORRECTION = 40;

const float MM_PER_TICK = 100.0 / 1151.0;
const long TICKS_CELL = (long)(180.0 / MM_PER_TICK);  // pas de cellule TMIRC (centre a centre)

// seuil moustache pliee = mur present -- A CALIBRER (compare une lecture mur-colle vs libre)
const int WALL_THRESHOLD = 2000;

// si aucun tick pendant ce temps alors que le moteur avance = mur devant (stall)
const unsigned long STALL_TIMEOUT_MS = 400;

// ---------------- maze ----------------
const int MAZE_SIZE = 16;
// bits : 0=Nord 1=Est 2=Sud 3=Ouest -- 1 = mur connu present
uint8_t walls[MAZE_SIZE][MAZE_SIZE];
uint8_t dist[MAZE_SIZE][MAZE_SIZE];

enum Dir { NORTH = 0, EAST = 1, SOUTH = 2, WEST = 3 };
int heading = NORTH;  // cap absolu actuel, cran de 90
int posX = 0, posY = 0;

const int GOAL_COUNT = 4;
const int goalX[GOAL_COUNT] = { 7, 7, 8, 8 };
const int goalY[GOAL_COUNT] = { 7, 8, 7, 8 };

// ---------------- etat gyro ----------------
float headingDeg = 0;
float gyroZ_bias = 0;
unsigned long lastTime = 0;

enum RunState { IDLE, EXPLORING, RETURNING, DONE };
RunState runState = IDLE;

// ---------------- bas niveau : moteurs, encodeurs, gyro ----------------

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

// ---------------- mouvement ----------------

// avance d'une cellule, avec correction encodeurs+gyro et detection de mur (stall)
// retourne true si arrivee complete, false si bloque par un mur (stall detecte)
bool moveForwardOneCell() {
  encCountR = 0;
  encCountL = 0;
  headingDeg = 0;
  lastTime = millis();

  long lastAvgTicks = 0;
  unsigned long lastProgress = millis();

  while (true) {
    updateHeading();

    long error = abs(encCountL) - abs(encCountR);
    int correctionEnc = constrain((int)(error * KP_ENC), -MAX_CORRECTION, MAX_CORRECTION);
    int correctionGyro = constrain((int)(headingDeg * KP_GYRO), -MAX_CORRECTION, MAX_CORRECTION);
    int correction = constrain(correctionEnc + correctionGyro, -MAX_CORRECTION, MAX_CORRECTION);

    motorA(-(SPEED + correction));
    motorB(SPEED - correction);

    long avgTicks = (abs(encCountR) + abs(encCountL)) / 2;

    if (avgTicks > lastAvgTicks) {
      lastProgress = millis();
      lastAvgTicks = avgTicks;
    }

    if (millis() - lastProgress > STALL_TIMEOUT_MS) {
      stopMotors();
      Serial.println("STALL -- mur devant detecte");
      return false;
    }

    if (avgTicks >= TICKS_CELL) {
      stopMotors();
      return true;
    }

    delay(10);
  }
}

// pivote vers la nouvelle direction absolue demandee, met a jour heading
void turnTo(int newHeading) {
  int diff = (newHeading - heading + 4) % 4;  // 0,1,2,3 = pas de virage, droite, demi-tour, gauche
  if (diff == 0) return;

  int steps = (diff == 3) ? -1 : diff;  // -1 = gauche, 1 = droite, 2 = demi-tour (2x droite)
  int turnsToDo = (diff == 3) ? 1 : diff;
  bool turnRight = (diff != 3);

  for (int i = 0; i < turnsToDo; i++) {
    headingDeg = 0;
    lastTime = millis();
    if (turnRight) {
      motorA(-TURN_SPEED);
      motorB(-TURN_SPEED);
    } else {
      motorA(TURN_SPEED);
      motorB(TURN_SPEED);
    }

    while (abs(headingDeg) < TURN_ANGLE) {
      updateHeading();
      delay(10);
    }
    stopMotors();
    delay(100);
  }

  heading = newHeading;
}

// ---------------- capteurs murs ----------------

// lit les 3 moustaches et met a jour walls[posY][posX] pour les 3 cotes visibles
// (le mur arriere n'est jamais mesure directement, il est deduit -- cf floodfill)
void senseWalls() {
  int right = analogRead(WHISKER_R);
  int front = analogRead(WHISKER_F);
  int left = analogRead(WHISKER_L);

  Serial.print("sense R/F/L: ");
  Serial.print(right); Serial.print('\t');
  Serial.print(front); Serial.print('\t');
  Serial.println(left);

  int dirRight = (heading + 1) % 4;
  int dirLeft = (heading + 3) % 4;

  if (right > WALL_THRESHOLD) walls[posY][posX] |= (1 << dirRight);
  if (left > WALL_THRESHOLD) walls[posY][posX] |= (1 << dirLeft);
  if (front > WALL_THRESHOLD) walls[posY][posX] |= (1 << heading);
}

bool hasWall(int x, int y, int dir) {
  return walls[y][x] & (1 << dir);
}

// ---------------- floodfill ----------------

void computeFloodfill(bool toStart) {
  for (int y = 0; y < MAZE_SIZE; y++)
    for (int x = 0; x < MAZE_SIZE; x++)
      dist[y][x] = 255;

  // file simple (BFS) -- tableau statique, assez grand pour toute la grille
  int qx[MAZE_SIZE * MAZE_SIZE], qy[MAZE_SIZE * MAZE_SIZE];
  int qHead = 0, qTail = 0;

  if (toStart) {
    dist[0][0] = 0;
    qx[qTail] = 0; qy[qTail] = 0; qTail++;
  } else {
    for (int i = 0; i < GOAL_COUNT; i++) {
      dist[goalY[i]][goalX[i]] = 0;
      qx[qTail] = goalX[i]; qy[qTail] = goalY[i]; qTail++;
    }
  }

  const int dx[4] = { 0, 1, 0, -1 };  // N, E, S, O
  const int dy[4] = { -1, 0, 1, 0 };

  while (qHead < qTail) {
    int cx = qx[qHead], cy = qy[qHead];
    qHead++;

    for (int d = 0; d < 4; d++) {
      if (hasWall(cx, cy, d)) continue;
      int nx = cx + dx[d], ny = cy + dy[d];
      if (nx < 0 || nx >= MAZE_SIZE || ny < 0 || ny >= MAZE_SIZE) continue;
      if (dist[ny][nx] <= dist[cy][cx] + 1) continue;
      dist[ny][nx] = dist[cy][cx] + 1;
      qx[qTail] = nx; qy[qTail] = ny; qTail++;
    }
  }
}

// choisit la direction voisine de plus petite valeur floodfill, sans mur connu
int chooseNextDir() {
  const int dx[4] = { 0, 1, 0, -1 };
  const int dy[4] = { -1, 0, 1, 0 };

  int best = -1;
  uint8_t bestDist = 255;

  for (int d = 0; d < 4; d++) {
    if (hasWall(posX, posY, d)) continue;
    int nx = posX + dx[d], ny = posY + dy[d];
    if (nx < 0 || nx >= MAZE_SIZE || ny < 0 || ny >= MAZE_SIZE) continue;
    if (dist[ny][nx] < bestDist) {
      bestDist = dist[ny][nx];
      best = d;
    }
  }
  return best;  // -1 si aucune sortie connue (ne devrait pas arriver si mur devant = stall gere)
}

bool atGoal() {
  for (int i = 0; i < GOAL_COUNT; i++)
    if (posX == goalX[i] && posY == goalY[i]) return true;
  return false;
}

// ---------------- un pas d'exploration ----------------

void exploreStep() {
  senseWalls();
  computeFloodfill(runState == RETURNING);

  int dir = chooseNextDir();
  if (dir < 0) {
    Serial.println("Pas de sortie connue -- verifier la carte ou le seuil moustache");
    stopMotors();
    digitalWrite(STBY, LOW);
    digitalWrite(LED_STATE, LOW);
    runState = IDLE;
    return;
  }

  turnTo(dir);
  bool moved = moveForwardOneCell();

  if (!moved) {
    // mur non detecte par la moustache mais stall physique -- corrige la carte
    walls[posY][posX] |= (1 << dir);
    return;  // recalculera au prochain exploreStep()
  }

  const int dx[4] = { 0, 1, 0, -1 };
  const int dy[4] = { -1, 0, 1, 0 };
  posX += dx[dir];
  posY += dy[dir];

  bool arrived = (runState == RETURNING) ? (posX == 0 && posY == 0) : atGoal();
  if (arrived) {
    stopMotors();
    if (runState == EXPLORING) {
      Serial.println("Objectif atteint -- retour au depart");
      runState = RETURNING;
      digitalWrite(LED_STATE, HIGH);  // fixe = retour en cours (cf convention LED)
    } else {
      Serial.println("Retour au depart termine.");
      digitalWrite(STBY, LOW);
      digitalWrite(LED_STATE, LOW);
      runState = DONE;
    }
  }
}

// ---------------- setup / loop ----------------

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
  pinMode(BTN_RETRY, INPUT_PULLUP);

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

  Serial.println("Pose la souris case (0,0), face au Nord. GO pour explorer.");
}

void loop() {
  if (digitalRead(BTN_RESET) == LOW) {
    stopMotors();
    digitalWrite(STBY, LOW);
    digitalWrite(LED_STATE, LOW);
    runState = IDLE;
    posX = 0; posY = 0; heading = NORTH;
    for (int y = 0; y < MAZE_SIZE; y++)
      for (int x = 0; x < MAZE_SIZE; x++)
        walls[y][x] = 0;
    Serial.println("RESET -- carte effacee");
    delay(300);
  }

  if (digitalRead(BTN_GO) == LOW && runState == IDLE) {
    digitalWrite(STBY, HIGH);
    digitalWrite(LED_STATE, LOW);  // clignote = exploration -- cf boucle plus bas
    runState = EXPLORING;
    Serial.println("GO -- exploration");
    delay(300);
  }

  if (runState == EXPLORING || runState == RETURNING) {
    // clignote pendant l'exploration (etat "exploration en cours")
    digitalWrite(LED_STATE, (millis() / 300) % 2);
    exploreStep();
  }
}

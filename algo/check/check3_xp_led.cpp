// test des 3 etats de la LED, pilotes par les boutons
// LED eteinte  = en attente (etat par defaut / apres RESET)
// LED clignote = exploration en cours (apres GO)
// LED fixe     = retour termine (apres RETRY)

const int LED_STATE = 21;
const int BTN_GO = 17;
const int BTN_RESET = 22;
const int BTN_RETRY = 0;

enum State { WAITING, EXPLORING, DONE };
State state = WAITING;

unsigned long lastBlink = 0;
bool blinkOn = false;
const unsigned long BLINK_MS = 300;

void setup() {
  Serial.begin(115200);

  pinMode(LED_STATE, OUTPUT);
  pinMode(BTN_GO, INPUT_PULLUP);
  pinMode(BTN_RESET, INPUT_PULLUP);
  pinMode(BTN_RETRY, INPUT_PULLUP);

  digitalWrite(LED_STATE, LOW);
  Serial.println("LED eteinte (WAITING). GO = clignote, RETRY = fixe, RESET = eteinte.");
}

void loop() {
  if (digitalRead(BTN_RESET) == LOW) {
    state = WAITING;
    digitalWrite(LED_STATE, LOW);
    Serial.println("RESET -> LED eteinte");
    delay(300);
  }

  if (digitalRead(BTN_GO) == LOW) {
    state = EXPLORING;
    Serial.println("GO -> LED clignote");
    delay(300);
  }

  if (digitalRead(BTN_RETRY) == LOW) {
    state = DONE;
    digitalWrite(LED_STATE, HIGH);
    Serial.println("RETRY -> LED fixe");
    delay(300);
  }

  if (state == EXPLORING) {
    unsigned long now = millis();
    if (now - lastBlink >= BLINK_MS) {
      lastBlink = now;
      blinkOn = !blinkOn;
      digitalWrite(LED_STATE, blinkOn ? HIGH : LOW);
    }
  }
}

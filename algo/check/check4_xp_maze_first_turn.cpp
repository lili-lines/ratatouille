// test logique uniquement -- USB seul, pas de puissance moteur (VM non alimente)
// avance simulee -> detecte l'ouverture (relachement brusque de la moustache)
// au premier croisement -> annonce le virage 90 -> stop complet (fin du test)

const int WHISKER1 = A0;  // moustache qui suit le mur

const int BTN_GO = 17;
const int BTN_RESET = 22;

int prevValue = 0;
const int RELEASE_THRESHOLD = 150;  // a ajuster selon tes valeurs reelles observees

bool running = false;
bool done = false;

void setup() {
  Serial.begin(115200);
  pinMode(BTN_GO, INPUT_PULLUP);
  pinMode(BTN_RESET, INPUT_PULLUP);
  prevValue = analogRead(WHISKER1);
  Serial.println("Pret. Appuie sur GO pour lancer le test (avance simulee).");
}

void loop() {
  if (digitalRead(BTN_RESET) == LOW) {
    running = false;
    done = false;
    prevValue = analogRead(WHISKER1);
    Serial.println("RESET");
    delay(300);
  }

  if (digitalRead(BTN_GO) == LOW && !running && !done) {
    running = true;
    Serial.println("GO -> avance simulee, surveille la moustache...");
    delay(300);
  }

  if (running) {
    int value = analogRead(WHISKER1);
    int delta = value - prevValue;

    Serial.print(value);
    Serial.print('\t');
    Serial.println(delta);

    if (delta > RELEASE_THRESHOLD) {
      Serial.println("OUVERTURE DETECTEE -- virage 90 (simulation, moteurs non alimentes)");
      Serial.println("Sequence terminee. RESET pour recommencer.");
      running = false;
      done = true;
    }

    prevValue = value;
    delay(50);
  }
}

// aucun moteur -- bouge juste la moustache droite a la main et regarde
// comment le code interprete chaque mouvement dans le Serial Monitor

const int WHISKER_R = A0;

// -- memes valeurs que xp_wall_follower.cpp, a garder synchronisees --
const int WALL_PRESENT_THRESHOLD = 500;
const int WALL_FOLLOW_TARGET = 300;
const int DROP_THRESHOLD = 100;

int prevRight = 0;

void setup() {
  Serial.begin(115200);
  prevRight = analogRead(WHISKER_R);
  Serial.println("valeur\tdelta\tinterpretation");
  Serial.println("Bouge la moustache a la main : vers la souris, puis relache-la.");
}

void loop() {
  int right = analogRead(WHISKER_R);
  int delta = right - prevRight;
  prevRight = right;

  Serial.print(right);
  Serial.print('\t');
  Serial.print(delta);
  Serial.print('\t');

  // meme logique que le suivi de mur : interprete ce que le code "croit" en ce moment
  if (right >= WALL_PRESENT_THRESHOLD) {
    Serial.print("MUR PRESENT (colle)");
  } else if (right < WALL_PRESENT_THRESHOLD && delta < -DROP_THRESHOLD) {
    Serial.print("RELACHEMENT RAPIDE (ouverture probable)");
  } else {
    Serial.print("pas de mur / suivi");
  }

  Serial.print('\t');
  if (right > WALL_FOLLOW_TARGET) {
    Serial.println("-> trop pres, la souris doit s'ELOIGNER");
  } else {
    Serial.println("-> trop loin, la souris doit se RAPPROCHER");
  }

  delay(100);
}

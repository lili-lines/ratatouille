# Annexe 3 — BitRacer Pro Max : conception et applications d'un robot à roues bi-plateforme (traduction FR)

*Fichier original : `附件3-2026TMIRC暑期營-雙平台輪型機器人.pdf` (60 diapositives)*
*Date : 2026/07/13 · Enseignants : Su Jing-Hui (蘇景暉), Li Bing-Hui (李炳輝) · Développeur : Zheng Li-Wei (鄭力瑋) · Lunghwa University*

---

## Sommaire (p.2)
- Introduction
- Conception matérielle — mécanique & circuit imprimé
- Schémas électriques
- Architecture de développement bi-plateforme
- Programmation par blocs **MakeCode**
- Programmation **Arduino IDE**
- Applications concrètes & conclusion

## Introduction — le robot pédagogique BitRacer Pro Max (p.3)
Conçu pour l'apprentissage de la programmation et des robots à roues (programme scolaire taïwanais « 108課綱 »), intégré à l'**éducation STEM**. Un robot dédié pour apprendre la robotique intelligente, avec une **architecture de développement bi-plateforme** (Arduino + MakeCode) adaptée à différents niveaux.

---

## Conception mécanique — ensemble moteur + roue (p.4)
Vue éclatée d'une roue motrice :
- **馬達** = Moteur · **檔板** = plaque/support · **齒輪** = pignon
- **培林** = roulement (bearing) · **墊片** = rondelle/entretoise
- **輪軸** = axe de roue · **輪框** = jante · **輪胎** = pneu
- **防鬆螺帽** = écrou anti-desserrage (nyloc)

## Conception mécanique — module infrarouge (p.5)
Photos de la carte robot avec les capteurs infrarouges et le télémètre ultrason HC-SR04.

## Conception du circuit imprimé (p.6–7)
Annotations de la carte **BitRacer Pro Max** (p.7) :
- Slot d'extension ultrason (超音波擴充插槽) — **HC-SR04**
- **5× capteurs infrarouges** (紅外線感測器 ×5) · slot d'extension IR
- **2× LED** · slot écran LCD (**1.8" TFT**)
- **Microcontrôleur** (微控制器) · **buzzer passif** (無源蜂鳴器)
- **micro:bit** · 2× moteurs · carte encodeur (編碼器電路板)
- Interrupteur d'alimentation · **batterie Li-ion 14500** · 2× boutons · Micro USB

---

## Schémas électriques (p.9–12)
- **p.9 — Contrôleur :** micro:bit + **ATSAMD21G18** (le MCU). Quartz 32,768 kHz, bus I2C (SDA_MCB/SCL_MCB), PWMA/PWMB, encodeurs L/R, IR1–5, USB D+/D−.
- **p.10 — IR & buzzer & boutons/LED :** buzzer via transistor S9013, LED_L/LED_R, MOSFET SI2302 pour l'alim IR, 5 capteurs IR **VCNT2020**.
- **p.11 — Alim/charge & interrupteur & encodeur :** USB Micro, chargeur **TP4056**, régulateur **MCP1826** (4,2 V → 3,3 V), protection batterie **DW01A + FS8205A**, interrupteur 3 positions.
- **p.12 — Moteur & LCD & ultrason :** driver **TB6612FNG** (U3), 2 moteurs, écran LCD 1,8", HC-SR04.

## Architecture système (p.13–14)
Chaîne d'alimentation :
`Micro USB 5V → batterie Li-ion 14500 → chargeur TP4056 → 4,2 V → LDO MCP1826 → 3,3 V`
Le MCU **SAMD21G18** pilote : le driver **TB6612FNG** (moteurs) en **PWM**, le **micro:bit** en **I2C**, l'écran **TFT 1,8"** en **SPI**, les capteurs IR **VCNT2020** en **ADC**, plus l'encodeur magnétique, le buzzer, l'ultrason **HC-SR04P** et les boutons.

## Architecture logicielle bi-plateforme (p.15)
`BitRacer Pro Max → ATSAMD21G18 (réglage firmware/communication) → intégration des périphériques →` **2 voies au choix** : **MakeCode** (développement graphique) ou **Arduino IDE** (développement texte).

## Protocole I2C (p.16)
Un **maître** (micro:bit) et plusieurs **esclaves** (ATSAMD21G18, HuskyLens, Slave #3) partagent le bus **SDA/SCL** avec des résistances de tirage (pull-up) Rp.

---

## Commandes de base (p.17) — *fonction / n° commande / R(lecture) ou W(écriture)*
| Fonction | Commande | R/W |
|---|---|---|
| Moteur roue gauche | 0x20 | W |
| Moteur roue droite | 0x21 | W |
| Moteur deux roues | 0x22 | W |
| Lire IR-1 … IR-5 | 0x30 … 0x34 | R |
| Lire IR-L | 0x35 | R |
| Lire IR-R | 0x36 | R |
| Interrupteur IR (on/off) | 0x37 | W |
| Lire tous les IR | 0x38 | R |

## Commandes périphériques (p.18)
| Fonction | Commande | R/W |
|---|---|---|
| Lire valeur encodeur | 0x40 | R |
| Remise à zéro encodeur | 0x41 | W |
| Contrôle LED | 0x42 | W |
| Lire distance ultrason | 0x43 | R |
| Afficher texte/chiffre | 0x44 | W |
| Effacer l'écran | 0x45 | W |
| Afficher image | 0x46 | W |
| Définir couleur de fond | 0x47 | W |

---

## MakeCode — sommaire (p.19)
1. Environnement & blocs d'extension
2. Valeurs IR & normalisation
3. Estimation de la position sur la piste
4. Contrôle proportionnel-dérivé (PD)
5. Surveillance de la distance ultrason
6. Encodeur & affichage LCD

## Environnement & blocs d'extension (p.20–21)
Utilise l'environnement **Microsoft MakeCode**. Création de **blocs d'extension dédiés** au BitRacer pour simplifier la communication (fonctions `setIRPower`, `getIR(index)`, `getAllIRValues()` via I2C).

## 2 — Valeurs IR & normalisation (p.22–24)
**Objectif : calibrer/uniformiser les sorties des différents capteurs IR.**

$$y_i = \frac{y_{max}-y_{min}}{AD_{i,max}-AD_{i,min}}\,(AD_i - AD_{i,min}) + y_{min}$$

- $y_{max}/y_{min}$ = valeurs max/min voulues (par défaut)
- $AD_{i,max}/AD_{i,min}$ = max/min mesurés par le capteur
- $AD_i$ = valeur mesurée à l'instant

Exemple : $\frac{5000-0}{4000-100}(2000-0)+0 \approx 2500$.

## 3 — Estimation de la position sur la piste (p.25–27)
**Objectif : calculer la position de la ligne à partir des 5 valeurs IR** (moyenne pondérée).

$$L_p = \frac{S_1x_1+S_2x_2+S_3x_3+S_4x_4+S_5x_5}{S_1+S_2+S_3+S_4+S_5}$$

- $S_i$ = valeur du capteur · $x_i$ = position du capteur (**-2, -1, 0, 1, 2**)

Exemple : $\frac{700·2+1000·1+600·0+0+0}{700+1000+600} = \frac{2400}{2300} = 1{,}0434$.

## 4 — Contrôle proportionnel-dérivé PD (p.28–30)
**Objectif : régler les gains Kp et Kd pour suivre la ligne.**

$$\Delta PWM = K_p\,e[n] + K_d\,(e[n]-e[n-1])$$
$$PWM_R = PWM_B + \Delta PWM \qquad PWM_L = PWM_B - \Delta PWM$$

- $K_p$ gain proportionnel · $K_d$ gain dérivé
- $e[n]$ erreur au centre · $e[n-1]$ erreur précédente
- $PWM_B$ vitesse de base · $PWM_L/PWM_R$ vitesses roues gauche/droite

## 5 — Surveillance distance ultrason (p.31–32)
**Objectif : mesurer la distance à un objet par ultrason.**

$$D = \frac{t \times 340}{2}$$

Distance (cm) = durée × vitesse du son (340 m/s) ÷ 2 (aller-retour).

## 6 — Encodeur & affichage LCD (p.33–34)
**Objectif : calculer la distance parcourue via l'encodeur et l'afficher sur le LCD.**

$$d = \frac{P_L + P_R}{2} \times \frac{\pi \times 23}{32}$$

Distance (mm) = moyenne des impulsions des 2 roues × (π × diamètre roue **23 mm**) / (**32** impulsions/tour).

## Programme complet MakeCode (p.35–43)
Captures de blocs : boucle principale avec **modes de calibration** (校正模式 1/2/3), fonctions `PD`, `poids (weights)`, `min/max`, `échantillonnage IR`, `normalisation`, et initialisation des variables (target max 4000, target min 1, bassSPD 350, pError, etc.).

---

## Arduino IDE (p.44–53)

### Définition des broches (p.44)
```cpp
//------ Capteurs infrarouges ------//
#define IR_1 A6
#define IR_2 A5
#define IR_3 A4
#define IR_4 A3
#define IR_5 A0
#define IR_L A2
#define IR_R A1
#define IRcontrol 13
//------ LED & bouton ------//
#define LED_R 30   // LED droite
#define LED_L 31   // LED gauche
#define BUTTON 25  // bouton
//------ Moteurs ------//
#define PWML 2     // PWM moteur gauche
#define BIN_1 28   // sens moteur gauche 1
#define BIN_2 29   // sens moteur gauche 2
#define AIN_1 32   // sens moteur droit 1
#define AIN_2 33   // sens moteur droit 2
#define PWMR 3     // PWM moteur droit
```

### Paramètres (p.45)
```cpp
//------ Contrôle PD ------//
#define kp 100
#define kd 200
#define Basic_speed 100
//------ Tableaux ------//
int   IR[7]     = {0};                                  // valeurs IR
int   IR_Max[7] = {1,1,1,1,1,1,1};                      // max IR
int   IR_Min[7] = {1023,1023,1023,1023,1023,1023,1023}; // min IR
float IR_Nor[7] = {0};                                  // IR normalisés
//------ Variables ------//
float x=0, err=0, ekd=0, errold=0;
int   pd=0;
int   L_Cnt=0, R_Cnt=0;
```

### `setup()` — initialisation (p.46)
Règle le débit série (9600), met les IR/bouton en **entrée**, les LED en **sortie** ; attend l'appui bouton ; avance (`Motor(100,100)`) en échantillonnant IR/min-max 50 fois, puis s'arrête.

### `loop()` + `follow()` (p.47)
Sur appui bouton : boucle `follow()` (suivi de ligne) + `prompt()` ; s'arrête après avoir croisé **2 marques à droite** (`R_Cnt == 2`).
`follow()` appelle : `Ir()` (lecture), `Nor()` (normalisation), `weights()` (pondération/PD), `Print1()` (affichage).

### `Ir()` — lecture infrarouge (p.48)
Active l'IR (`IRcontrol` HIGH), lit les 7 capteurs via `analogRead` (IR_L→IR[6], IR_R→IR[0], IR_1..5→IR[1..5]), puis coupe l'IR.

### `Print1()` — moniteur série (p.49)
Sous-programme d'affichage (lignes `Serial.print` commentées / désactivées).

### `Maxmin()` & `Nor()` — min/max & normalisation (p.50)
```cpp
void Maxmin() {           // met à jour min/max de chaque capteur
  Ir();
  for (int j=0; j<6; j++) {
    if (IR[j] > IR_Max[j]) IR_Max[j] = IR[j];
    if (IR[j] < IR_Min[j]) IR_Min[j] = IR[j];
  }
}
void Nor() {              // normalise sur 0..1023
  for (int i=0; i<6; i++) {
    IR_Nor[i] = (1023.0/(IR_Max[i]-IR_Min[i])) * (IR[i]-IR_Min[i]);
    if (IR_Nor[i] > 1023) IR_Nor[i] = 1023;
    if (IR_Nor[i] < 1)    IR_Nor[i] = 1;
  }
}
```

### `weights()` — pondération & contrôle PD (p.51)
Calcule la position `x` (moyenne pondérée des IR, poids -2..+2), l'erreur `err = 0 - x`, puis :
```cpp
pd = kp*err + kd*(err - errold);
Motor(Basic_speed - pd, Basic_speed + pd);
```

### `Motor()` — pilotage moteurs (p.52)
Limite les PWM à ±255, choisit le sens (BIN/AIN HIGH/LOW selon le signe) et applique la vitesse via `analogWrite`.

### `prompt()` — gestion des marques/LED (p.53)
Détecte les marques latérales (IR-L/IR-R) et centrales (IR1/IR3/IR5) pour allumer les LED et incrémenter les compteurs `L_Cnt`/`R_Cnt` (états `L_status`, `R_status`, `C_status`).

---

## Applications & conclusion (p.54–59)
- Démonstrations sur piste de suivi de ligne, en **MakeCode** (p.55) et en **Arduino IDE** (p.56).
- **Calcul de la longueur totale du circuit** (départ → arrivée) : l'écran affiche **208,372** (cm), après « System Ready » (p.57).
- **Conclusion (p.58) :** le BitRacer Pro Max repose sur l'architecture **bi-plateforme** : il conserve le matériel et les drivers de capteurs Arduino, tout en offrant des **blocs MakeCode dédiés**. Résultat : une grande **facilité d'usage pédagogique**, permettant à des élèves de tous âges de franchir facilement la barrière d'entrée.
- **GitHub (p.59) :** blocs d'extension partagés → <https://github.com/CorgiQQQ/BitRacer_Pro_Max>

## Q & A (p.60)

---

> 🐭 **Lien avec ton projet Ratatouille :** ce robot partage beaucoup avec ta micromouse — driver **TB6612FNG**, contrôle **PD**, **encodeurs**, capteurs analogiques lus en **ADC**, normalisation des capteurs, calcul de distance par impulsions d'encodeur. Les formules (position pondérée, PD, distance encodeur) sont directement réutilisables. Différence clé : eux suivent une **ligne** avec des IR ; toi tu longes des **murs** avec des **moustaches AS5600**.

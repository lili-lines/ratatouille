Ref expiquation composant condensateur :
https://fr.vecteezy.com/art-vectoriel/25747550-l-eau-reservoir-analogie-pour-une-condensateur-adapte-pour-educatif-des-produits


Memo assemblage :

VISU MD
Top-down layout of the mouse: where each component sits, and how they connect (power in solid lines, signal/data in dashed lines).

<figure style="margin:0;">
  <img src="{{ '/assets/img/mouse_layout.svg' | relative_url }}" alt="Ratatouille micromouse layout and wiring diagram" style="max-width:100%; display:block; margin:0 auto;">
  <figcaption style="text-align:center; font-size:0.85rem;">Component placement and connections, schematic (not to scale)</figcaption>
</figure>



Zone "PUISSANCE" (bruyante)  ←→  Zone "SIGNAL" (sensible)
   batterie, moteurs,              Teensy, AS5600, gyro,
   driver, câbles 7,4V              PMW3901, câbles I2C/SPI

minimum de proximité entre elle

👌 Ce qui DOIT être proche (obligation physique/électrique)

. Condensateur découplage ↔ sa puce	doit être à quelques mm des pattes VCC/GND, sinon il ne stabilise plus rien

. Condensateur réservoir ↔ Driver (VM)	doit absorber les pics de courant à la source, un fil trop long retarde sa réaction

. Driver ↔ Moteurs	fils courts = moins de perte de tension/résistance sur les gros courants (jusqu'à 3A)

. use/Switch ↔ Batterie	la protection doit être au plus près de la source, pas en bout de chaîne

⛔ Ce qui NE DOIT PAS être proche (à éviter absolument)

. Moteurs ↔ AS5600 (moustaches)	le bruit électrique du moteur (commutation brushed DC) pollue le signal analogique fragile de l'AS5600 → lecture qui tremble

. Moteurs ↔ Gyro	les vibrations mécaniques du moteur faussent la mesure du gyro (bruit dans la lecture d'orientation)

. Câbles de **puissance** (7,4V) ↔ câbles de **signal** (I2C, SPI, analog)	
s'ils courent en parallèle et collés, le courant qui varie dans le fil de puissance induit du bruit dans le fil de signal à côté (couplage électromagnétique) — c'est pour ça que le star ground impose une masse commune à un seul point, pas des masses qui se croisent partout

. MP1584 (buck) ↔ AS5600	le MP1584 est lui-même une source de bruit (il découpe à haute fréquence, comme un mini-PWM) — le garder à distance du signal analogique fragile


🐭 DONC :
Moustaches + PMW3901 + Gyro groupés à l'avant/centre → loin des moteurs, zone "calme"
Moteurs + Driver + Batterie groupés sur les côtés/arrière → zone "puissance", loin des capteurs sensibles
Le Teensy au centre exact → c'est le seul composant qui doit toucher les deux zones (il reçoit les signaux ET commande la puissance), donc il sert de frontière/pont entre les deux

⚠️ code : Le code qui ignore le signal moustache pendant un virage actif

⚠️ GND en étoile = 1 seul point car signal bruyant et on ne veut pas perturber l'information avec du bruit, donc 1 ligne comme la breadboard. 

lazer print:
. faire une piece pour aligner les moteurs
. ressort


💲💲💲 ACHAT :

. English: micro SD card module SPI Arduino
  中文: micro SD卡模組 SPI
  model qui accept directement 3.3V
  我要買 micro SD卡模組，SPI介面，支援3.3V
  Je veux un module carte micro SD, interface SPI, compatible 3.3V.
  + une carte micro SD
    microSD記憶卡

. fuse :
  5x20mm 2A slow blow glass fuse 250V
  5x20mm 2A 延時熔斷器 250V (ou 慢熔保險絲)
  + porte fuse : (boitier avec fils)
   inline fuse holder 5x20mm with wire leads
   5x20mm 保險絲座 帶線 (ou 熔斷器座)

. caoutchou
. vis, écrou, rondel (plastique ou métal)
. mouse anti bruit, mouse/gel


POST 
. assemblage des composants
la structure : composant absorbant de vibration : tampon en mouse/caoutchouc
. pour PWM : Amortir les vibrations du mât avec tampon en mousse/caoutchouc entre chassis et mat, ajout d'une jupe anti lumiere autour du capteur (? a voir si besoin)


🚧 in process :
alim de labo <=> batterie, fuse, interrupteur

  AVANT CHAQUE XP
. xps : mesurer les composants au multimetre

. post : materiel et construction : 3D print

. XP : where is ratatouille ?

. XP : moustache AS5600
Seuil de relâchement de moustache
Détecter la dérivée du signal pour distinguer dérive lente vs mur qui finit (déjà expliqué dans ton post whisker, mais pas encore testé avec du bruit moteur réel).


🚧 TODO : une fois le robot assemblé :

. xps : lecture signal avec motor PWM

. xps : tester la vitesse en virage, rouler en rond

. Une fois le robot assemblé (physiquement) :
+ Lecture signal moustache avec moteurs qui tournent sur le vrai robot (pas juste breadboard)
+ Test de virage / rouler en rond (vérifier l'odométrie en conditions réelles)

🚧 je sais pas :
. xps : faire tourner les moteurs vitesse de moteur (battery/power) Teensy → driver → 1 moteur, en PWM, alimenté par l'alim de labo (7,4 V, limite de courant). Vérifier vitesse/sens.


Batterie + (rouge) ─→ interrupteur ─→ fusible ─→ ligne "+" de la breadboard
Batterie − (noir)  ─────────────────────────────→ ligne "−" de la breadboard

&&&

ligne "+" (7.4V) ─┬─→ MP1584  IN+   →  MP1584 OUT+ (5V réglé) → Teensy VIN
                  │
                  └─→ TB6612FNG  VM   (7.4V direct, pour les moteurs)

ligne "−" (0V)  ──┬─→ MP1584  IN−  et  OUT−
                  ├─→ Teensy GND
                  └─→ TB6612FNG  GND


pour le teste avec le power de laboratoire :
To test the full power chain instead, feed 7.4 V through the MP1584 with the current limit set to 300–500 mA as a safety cap
⚠️ pour ne pas tout griller on va vérifier la sorite réel de chaque composant du circuit avant de tout relier ensemble

. Pour chaque composant :
  une photo avec sur la photo situer l'in/out avec les cristères
  in-/in+ : 1 ou + à respecter pour ne pas griller le truc
  out-/out+ : 1 ou + attention au prochain composant
  une photo de la vérif avec un multimètre
  quel composant avant et quel composant aprés

. Faire un cercle avec les composant pour avoir une vue d'ensemble

> The signal vs power topic moved to its own post: [Signal vs Power]({{ '/2026/07/07/signal-vs-power.html' | relative_url }}).

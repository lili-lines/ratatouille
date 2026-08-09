lazer print:
. faire une piece pour aligner les moteurs

. ressort

ACHAT :
. cable alim labo → breadbord
    banana plug alligator clip to breadboard jumper wire set
    香蕉插頭鱷魚夾杜邦線組
    ou banana plug to alligator clip test leads
       香蕉插頭轉鱷魚夾測試線
  
. chargeur baterrie 1 element: 
  2S 鋰電池平衡充電器 SM接頭輸出
  2S LiPo balance charger with SM connector output
ou plusieur :
  . chargeur :
    2S LiPo balance charger, JST-XH connector
    2S 鋰電池平衡充電器, JST-XH接頭 (ok en ligne)
    + JST-XH to SM 2S adapter cable
      JST-XH 轉 SM 轉接線 2S ⚠️
    + prise si no inside :
      12V DC power adapter, 5.5x2.1mm barrel plug
      12V DC電源供應器 5.5x2.1mm接頭

. LED : Rouge (630–660 nm) ou Rouge Proche-Infrarouge (850 nm)
LED 5mm à trous (through-hole), pas CMS/SMD (trop petit à souder à la main) 20mA
  English: red LED 5mm high brightness
  中文: 紅色LED 5mm 高亮度
  Résistance : Vf ≈ 2,0V → 270Ω (7,4−2,0)/0,02 (x5)
  ou
  red LED 5mm high brightness 20mA
  紅色LED 5mm 高亮度 20mA (x4)
    + 220 ohm resistor 1/4W
      220歐姆電阻 1/4W (x4)

  English: infrared LED 850nm 5mm
  中文: 紅外線LED 850nm 5mm
  Résistance : Vf ≈ 1,4V → 300Ω (7,4−1,4)/0,02
  
  English: blue LED 5mm high brightness
  中文: 藍色LED 5mm 高亮度
  Résistance : Vf ≈ 3,2V → 220Ω (7,4−3,2)/0,02

  → éclairage en biais 30° à 45° par rapport à la surface

a checker : Résistance = (7.4V − 3.2V) / 0.02A ≈ 210Ω → 220Ω standard, ça convient
. que le module PMW3901 gere la conversion 3V3 → 2.1V

. fuse :
  5x20mm 2A slow blow glass fuse 250V
  5x20mm 2A 延時熔斷器 250V (ou 慢熔保險絲)
  + porte fuse : (boitier avec fils)
   inline fuse holder 5x20mm with wire leads
   5x20mm 保險絲座 帶線 (ou 熔斷器座)

. interupteur :
  SPST (單刀單擲)	2 broches, juste ON/OFF 3A 250V ou 5A 125V (l'un ou l'autre convient, tant que ≥3A)
  我要買滑動開關，10mm，單刀單擲(SPST)，至少3A


. caoutchou
. vis
. mouse/gel





POST
. creation de post :
. assemblage
. pour PWM : Amortir les vibrations du mât avec tampon en mousse/caoutchouc entre chassis et mat, ajout d'une jupe anti lumiere autour du capteur (? a voir si besoin)
. bom update 

la structure : composant absorbant de vibration : tampon en mouse/caoutchouc



🚧 in process :
alim de labo = batterie, fuse, interrupteur

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

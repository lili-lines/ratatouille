
🚧 TODO :

. xps : mesurer les composants au multimetre

. xps : faire tourner les moteurs vitesse de moteur (battery/power)
Teensy → driver → 1 moteur, en PWM, alimenté par l'alim de labo (7,4 V, limite de courant). Vérifier vitesse/sens.
& lire les encoders des moteurs, les N20 ont un encodeur hall intégré, à lire en quadrature

. post : materiel et construction : 3D print

. Lire le gyro (MPU-6050) en I2C
 Boot + lecture des données sur le bus I2C.
& Fusion capteurs (encodeurs + gyro)
Combiner distance (encodeurs) + cap (gyro) pour une position stable — proto Python (semaine 3).

. Seuil de relâchement de moustache
Détecter la dérivée du signal pour distinguer dérive lente vs mur qui finit (déjà expliqué dans ton post whisker, mais pas encore testé avec du bruit moteur réel).

🚧 TODO : une fois le robot assemblé :

. xps : lecture signal avec motor PWM

. xps : tester la vitesse en virage, rouler en rond

. Une fois le robot assemblé (physiquement) :
+ Lecture signal moustache avec moteurs qui tournent sur le vrai robot (pas juste breadboard)
+ Test de virage / rouler en rond (vérifier l'odométrie en conditions réelles)



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

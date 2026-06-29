---
layout: post
title: "A clean whisker signal under PWM"
date: 2026-07-02 10:00:00 +0200
tags: [electronics, signal]
---


Teensy on bench power supply 

2 params: the voltage (7.4 V to mimic the battery) & the current limit (300–500 mA max microcontrol), in line with the Teensy.



"Clean whisker signal **under PWM**" means: the whisker reading stays clean
**while the motors are running**.

## The problem

PWM (Pulse Width Modulation) switches the motor power on/off thousands of times
per second. Those fast switches create **electrical noise** that leaks into the
whole circuit — and can pollute the fragile analog signal of the AS5600 whisker
sensor. The value then "shakes".

## The test

Getting a clean signal when everything is still is easy. The real challenge is
keeping it clean **while the motors switch and make noise**.

If the whisker stays readable with the motors running, it proves the noise
defenses work:
- the **RC filter** (resistor + capacitor) on each whisker line,
- the **decoupling** capacitors next to each chip,
- the **star ground** (one common ground point).

## Takeaway

**Under PWM = with the motor noise present.** A clean signal then = the filtering
is doing its job.




PWM = Pulse Width Modulation (en français : Modulation de Largeur d'Impulsion, MLI).

L'idée en une phrase
C'est une façon de régler une puissance moyenne en allumant/éteignant très vite un signal, plutôt qu'en faisant varier la tension.

Au lieu de fournir une tension continue ajustable (compliqué), on envoie un signal tout ou rien (0 V / 7,4 V) qui clignote des milliers de fois par seconde. On joue sur le temps « allumé » vs « éteint » :


Rapport cyclique 25 %   ▔▁▁▁▔▁▁▁▔▁▁▁   → puissance faible
Rapport cyclique 50 %   ▔▔▁▁▔▔▁▁▔▔▁▁   → mi-puissance
Rapport cyclique 75 %   ▔▔▔▁▔▔▔▁▔▔▔▁   → puissance forte

Le pourcentage de temps « allumé » s'appelle le rapport cyclique (duty cycle). C'est lui qui fixe la moyenne.

🐭 Dans ta MicroMouse
Moteurs : le Teensy ne « baisse » pas la tension des moteurs, il envoie du PWM au driver TB6612FNG. Un duty cycle de 80 % ≈ 80 % de la vitesse max. C'est exactement le « PWM-limited so the average stays ≈ 6 V » de ta BOM : la batterie fait 7,4 V, mais avec un duty cycle ~81 % la moyenne vue par le moteur ≈ 6 V.
C'est aussi ce qui crée du bruit électrique (les commutations rapides) → d'où ton filtre RC pour garder un « clean whisker signal under PWM » (signal de moustache propre malgré le PWM).

En résumé
PWM = faire varier une puissance moyenne en hachant un signal tout-ou-rien très vite. Le réglage se fait via le rapport cyclique (% de temps allumé). 




MOUSTACHE


 discrimination = détecter/trier sélectivement un évènement selon un critère

Fusion encodeurs (sur les roues) + IMU (gyro) = Position + orientation du robot : où il est (x, y) et dans quel sens il regarde (le cap). précis au millimètre (mm). combiner 2 capteurs pour composé les défauts l'un de l'autre

Détente = le moment où la moustache, pliée contre un mur, se relâche. 2 possibilité : (1) le robot s'éloigne un peu du mur → la moustache se détend lentement. (2) le mur s'arrête (on arrive à une ouverture/intersection) → la moustache se détend d'un coup.

on les distingue avec la dérivée = la vitesse à laquelle le signal change :
(1) détente lente → petite dérivée
(2) détente brusque → grosse dérivée (pic)
🟰 on met un seuil sur cette dérivée : si ça dépasse → « le mur s'est arrêté, il y a une ouverture ici ». Sinon → « je dérive juste un peu, je continue de longer »

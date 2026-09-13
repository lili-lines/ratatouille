---
layout: post
title: Flash the critter
date: 2026-08-20 10:00:00 +0200
tags: [electronics, firmware]
---

* TOC
{:toc}

Après avoir validé l'assemblage des pièces, il faut checker si tout est correctement connecté, que le courant circule, que les informations sont transmises et interprétées correctement. Il est également important de définir la marge d'erreur acceptable pour la validation de chaque test.

**La marge d'erreur** <br>
Entre le mur et la souris il y a 44 mm de chaque côté en ligne droite, mais au moment de tourner comme l'essieu est situer à l'arrière le rayon balayer est de 🚧???mm, c.f. calcule dans [Mechanical design]({% post_url 2026-08-12-mechanical-design %}). Cette valeur va définir nos marges d'erreur. Pour le cap, l'erreur s'accumule durant le parcours.

<div style="display:flex; flex-wrap:wrap; gap:24px; align-items:flex-start;">
<div style="flex:0 0 auto;" markdown="1">
| erreur de<br>cap | décalage par<br>cellule | cellules avant de<br>💥 le mur |
|---|---|---|
| ±5° | 15,7 mm | 2,8 |
| ±3° | 9,4 mm | 4,7 |
| ±2° | 6,3 mm | 7,0 |

Tableau 1: Erreur de cap, décalage latéral par cellule parcourue
{: .table-caption}
</div>
<div style="flex:1; min-width:240px;" markdown="1">
(1) 2 tolérances d'erreur d'angle la lecture du gyro comparée à l'angle réel ±2° c.f. mesures post Odometry. Et l'erreur de virage ±3°, erreur qui s'accumule durant le parcours, d'apres le tableau 1 en 4 cellules elle dépasse les 44mm.
</div>
</div>

<div style="display:flex; flex-wrap:wrap; gap:24px; align-items:flex-start;">
<div style="flex:0 0 auto;" markdown="1">
| erreur de<br>distance | décalage par<br>cellule | cellules avant<br> > 44 mm |
|---|---|---|
| ±1 % | 1,8 mm | 24 |
| ±2 % | 3,6 mm | 12 |
| ±3 % | 5,4 mm | 8 |
| ±4 % | 7,2 mm | 6 |

Tableau 2: Erreur de distance,<br>devenue décalage latéral après un virage à 90°
{: .table-caption}
</div>
<div style="flex:1; min-width:240px;" markdown="1">
(2) L'erreur de distance est proportionnelle à la distance parcourue. Elle coûte au virage, tourner 20 mm et le nez peut toucher le mur. À ±2 % on tient 12 cellules avant de toucher.
</div>
</div>

(3) L'erreur de répétabilité tolérence ≤ 10 mm (soit 1 % sur 1m) entre les runs à vitesses différentes. Cette erreur vient du glissement eu démarrage et à l'arrêt, c'est le même phénomène independant de la distance.

(4) Erreur de singal : la moustache crête-à-crête < ..???... counts selon les chiffres à définir dans l'expèrience [Clean whisker signal]({% post_url 2026-07-28-clean-signal-under-pwm %}) 
🚧 todo : expérience à refaire mais aussi à faire en bit = 12, sur le robot monté juste avec all encodeur également.

ℹ️ Remarque : une erreur constante est plus facile à corriger qu'une erreur instable.


## 1. Test en USB réponse gyro, compteur, moustache

[photo du dispositif avec monitor]

<div class="two-col">
<div class="col" markdown="1">

**Objectif** <br>
L'idée ici est de vérifier si les capteurs fonctionnent et sont lus correctement. C'est un test d'observation.

**Matériel** <br>
. mouse en USB sans battery

**Protocole** <br>
Les moteurs sont à l'arrêt, `STBY LOW`. On tourne les roues à la main.
Observer les valeurs des capteurs avec le Serial Monitor sur Arduino : les valeurs sont lues en continu.

Upload sur le Teensy : <br>
[check1_xp_system.cpp]({{ site.repo }}/algo/check1_xp_system.cpp) <br>

</div>
<div class="col" markdown="1">
**Critère de réussite** <br>

|---|---|
| observable | affichage et réaction des valeurs |
| gyro | ap calibrage, la valeur correspond ~ le mouvement de la souris + une dérive dans le temps |
| 2 encodeurs de roue | comptent quand on tourne les roues à la main, d'un côté (+) et de l'autre (−) |
| 3 AS5600 | les puces réagissent au mouvement de leur aimant sur la plage 0-4095|

Tableau 3: observation serial monitor
{: .table-caption}

🛈 pas d'erreur chiffrée à ce stade : on vérifie que la chaîne capteur → Teensy → Serial Monitor fonctionne.
</div>
</div>

**Résultat** <br>
[monitor gif capture d'écran]
[ajouter un tableau des releves exacte de l'xp]

Compteur roues : le compteur de la roue droite ne renvoie pas de signal. 1ère chose à faire : vérifier le chemin du signal au multimètre pour savoir si tout est bien connecté. C'est là qu'on remarque que certaines pattes ne sont pas soudées. Les 2 compteurs de roues fonctionnent 👌 <br>
Tester le gyro en bougeant la souris sur les angles : 45°, 90°, 360°. On fait pivoter la souris avec un repère au sol et on lit le signal du gyro. 👌 <br>
Les 3 AS5600 étaient initialement positionnés à l'avant les uns à côté des autres, mais à la lecture des signaux il y a un problème. J'en ai déduit que la proximité des puces AS5600 perturbait les champs magnétiques. J'ai déplacé le module du milieu à l'étage 1 en tournant la puce vers le bas. Ainsi, j'ai pu garder la position de son pivot sans modifier la structure. 👌 <br>


<div style="display:flex; flex-wrap:nowrap; gap:25px; align-items:center;" markdown="1">
<img src="{{ '/assets/img/taxis_bouton.gif' | relative_url }}" alt="Boutons de pilotage" style="width:350px; flex-shrink:0; display:block; margin-top:35px; border-radius:10px;">
<div style="flex:1; min-width:0;" markdown="1">
### Piloter : exploration, retry, reset

<style>
.led-dot { display:inline-block; width:14px; height:14px; border-radius:50%; background:#3F00FF; margin:0 4px; vertical-align:middle; }
.led-slow { animation: led-blink 2s steps(1) infinite; }
.led-fast { animation: led-blink 0.4s steps(1) infinite; }
.led-triple { animation: led-blink 0.3s steps(1) 6; }
.led-fixed { animation: none; opacity: 1; }
.led-orange { background:#ff8c00; }
@keyframes led-blink { 0%, 49% { opacity: 1; } 50%, 100% { opacity: 0.15; } }
</style>

Ajouter des boutons : <br>
<span class="led-dot led-slow"></span> GO run d'exploration <br>
<span class="led-dot led-fast"></span> RETRY recommence l'étape actuelle <br>
<span class="led-dot led-triple led-orange"></span> RESET tout effacer <br>
<span class="led-dot led-fixed"></span> Phase d'exploration ✅ retour commence
</div>
</div>


## 2. Test en USB, moteur, boutons, LED, trajectoire, virage

Tolérance : <br>
. moustache au repos : crête-à-crête < ??? counts (12 bits)

**Objectif** <br>
Le driver en mode `STBY LOW` sleep reste au démarrage. Les boutons : RESET remet tout à zéro, GO réveille le driver `STBY HIGH`, RETRY redémarre l'étape en cours en cas de problème sur le terrain. 
LED d'état fixe et clignotante selon GO, RETRY et RESET.
Vérifier que les capteurs de positionnement et de contact renvoient les bonnes informations. Et que le Teensy les interprète correctement.

**Matériel** <br>
. mouse sans battery + USB + serial monitor

#### 2.1 À la main, moteurs éteints

Tolérance : <br>
. distance poussée à la main ±2 %
. cap du parcours en L ±2°

**Protocole**
<div class="two-col">
<div class="col" style="flex:1;" markdown="1">
Distance en ligne droite de 30 cm : pousser le robot à la main, en ligne droite, sur une distance connue, mesurée à la marque du milieu de l'essieu.<br>
Lire la dernière ligne `encCountR` et `encCountL` sur serial monitor qui est en ticks bruts et calculer la distance. RESET.
</div>
<div class="col" style="flex:1.1;" markdown="1">
$\text{dist}_{mm} = \dfrac{encCountR + encCountL}{2} \times 0{,}087$

$0,087$ = mm parcourus par un tick d'encodeur, résolution qu'on a calculée dans le post [Odometry]({% post_url 2026-08-01-odometry %}) via la circonférence ⌀32 mm
</div>
</div>

<div class="two-col">
<div class="col" style="flex:1;" markdown="1">
Parcours combiné 30 cm + 90° : pousser le robot en L, et comparer la position finale calculée distance et le cap à la position réelle mesurée. Lire la dernière ligne de `headingDeg` sur serial monitor. <br>
</div>
<div class="col" style="flex:1.1;" markdown="1">
```cpp
float gyroZ_dps = (gyroZraw / 131.0) - gyroZ_bias;   // vitesse de rotation, °/s
headingDeg += gyroZ_dps * dt;                        // cumulée en degrés
```
ℹ️ le gyro une vitesse de rotation en °/s, l'angle est calculé en ° dans le script c'est la valeur `headingDeg`
</div>
</div>

| Critères | Mesure | Tolèrance | Validation | 
|---|---|---|---|
| LED clignotement GO, RESET, RETRY | observation | - | ? |
| ligne droite 30cm | ? | ±2 %, ±6 mm | ? |
| cap 90° | ? | ±2° | ? |

Tableau 4: Résultats expérience à la main, moteurs OFF
{: .table-caption}


#### 2.2 Autonome, moteurs allumés

**Protocole** <br>
. avec les moteurs allumés le robot doit parcourir seul et en ligne droite la distance demandée de 30 cm, et faire les virages selon l'angle demandé, 45, 90 et 360°.
. pour tester la réaction des moutaches si on moustache bouge le robot tournera à 90° du cote de la moustache qui a bougé, et pour la moustache de devant le robot fera un 360.
. Appuyer sur les boutons pour tester les LEDs. Voir les clignotements correspondant section 1.

Upload sur le Teensy : <br>
[> check2_xp_straight.cpp]({{ site.repo }}/algo/check2_xp_straight.cpp) <br>
[> check3_xp_turn.cpp]({{ site.repo }}/algo/check3_xp_turn.cpp) - parametrage `TARGET_ANGLE = 360 & 90 & 45` <br>
[> check4_xp_whisker_turn.cpp]({{ site.repo }}/algo/check4_xp_whisker_turn.cpp)


| Critères | Mesure | Tolèrance | Validation | 
|---|---|---|---|
| ligne droite 30cm  | ? | ±2 %, ±6 mm | ? |
| virage 45° | ? | ±3° | ? |
| virage 90° | ? | ±3° | ? |
| virage 360° | ? | ±3° | ? |
| as56 gauche, reaction 90° | ? | observation | ? |
| as56 droit, reaction 90° | ? | observation  | ? |
| as56 avant, reaction 360° | ? | observation | ? |

Tableau 5: Résultats expérience moteurs ON
{: .table-caption}

[video ligne droite 30 cm] [video moustache L, R, F reaction] <br>
[video  360°] [video  90°] [video  45°] <br>


## 3. Test sur batterie

<div class="two-col">
<div class="col" markdown="1">
**Objectif** <br>
Vérification finale du montage et de la sensibilité des capteurs sur batterie avant de tester le labyrinthe.
</div>
<div class="col" markdown="1">
**Matériel** <br>
. 🐭 mouse complètes
</div>
</div>

**Protocole** <br>
On marque au sol la distance demandée ainsi que les angles et un rond comme repère. Pour le cercle, le robot doit revenir sur la marque de départ.
Et dans cette expérience on détourne exceptionnelement les boutons, pour ne pas avoir à reflasher la souris à chaque fois, on attribue aux boutons les fonctions suivantes : <br>
go=lent, retry=croisière, reset=fast

🚧todo : prévoir dans le code d'enregistre les données de chaque changement de vitesse

Upload sur le Teensy : <br>
[> check2_xp_straight.cpp]({{ site.repo }}/algo/check2_xp_straight.cpp) - parametrage `TICKS_TARGET = (long)(1000.0 / MM_PER_TICK)`, soit 11510 ticks pour 1 m <br>
[> check6_xp_circle.cpp]({{ site.repo }}/algo/check6_xp_circle.cpp)

| Critères | Mesure | Tolèrance | Validation | 
|---|---|---|---|
| ligne droite 1 m, lent | ? | ±2 %, ±20 mm | ? |
| ligne droite 1 m, croisière | ? | ±2 %, ±20 mm | ? |
| ligne droite 1 m, fast | ? | ±2 %, ±20 mm | ? |
| écart entre les runs | ? | ≤ 10 mm | ? |
| cercle, lent | ? | ±20 mm | ? |
| cercle, croisière | ? | ±20 mm | ? |
| cercle, fast | ? | ±20 mm | ? |
| écart entre les runs | ? | ≤ 10 mm | ? |
| suivre le tracé du cercle | observation | - | ? |

Tableau 6: Résultats expérience sur battery
{: .table-caption}

[video ligne droite 1m, lente]
[video ligne droite 1m, croisière]
[video ligne droite 1m, fast]

[video 1 tour cercle, lente]
[video 1 tour cercle, croisière]
[video 1 tour cercle, fast]


# References

- [joshuaccl/Micromouse (GitHub)](https://github.com/joshuaccl/Micromouse), STM32 micromouse firmware with dedicated branches for gyro/encoder calibration and wall tracking

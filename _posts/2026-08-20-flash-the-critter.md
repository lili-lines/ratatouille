---
layout: post
title: Flash the critter
date: 2026-08-20 10:00:00 +0200
tags: [electronics, firmware]
---

* TOC
{:toc}

Après avoir validé l'assemblage, il faut checker si tout est correctement connecté et que le courant circule.


## 1. Test en USB, réponse gyro, compteur, moustache

[photo du dispositif avec monitor]

L'idée ici est de vérifier si les capteurs fonctionnent et sont lus correctement. On upload donc le script suivant sur le Teensy : <br>
[check1_xp_system.cpp]({{ '/algo/check1_xp_system.cpp' | relative_url }}) <br>
Les moteurs sont à l'arrêt, `STBY LOW`. On tourne les roues à la main pour le test.

#### 1.1 Critères de réussite

Le gyro doit se calibrer et indiquer le bon angle d'orientation du robot, à vérifier en prenant un repère au sol. Les 2 encodeurs des roues doivent compter le nombre de tours, d'un côté (+) et de l'autre (-). Les 3 modules AS5600 doivent lire l'angle de l'aimant. <br>
Vérifier les valeurs avec le Serial Monitor sur Arduino : les valeurs sont lues en continu. Les signaux doivent être justes et propres.

#### 1.2 Validation

[monitor gif capture d'écran]

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


## 3. Test en USB, moteur, boutons, LED, trajectoire, virage

Le driver en mode `STBY LOW` sleep reste au démarrage. Les boutons : RESET remet tout à zéro, GO réveille le driver `STBY HIGH`, RETRY redémarre l'étape en cours en cas de problème sur le terrain. LED d'état fixe et clignotante selon GO et reset.

Les scripts à upload sur le teensy : <br>
[> check2_xp_straight.cpp]({{ '/algo/check2_xp_straight.cpp' | relative_url }}) <br>
[> check3_xp_turn.cpp]({{ '/algo/check3_xp_turn.cpp' | relative_url }}) - parametrage `TARGET_ANGLE = 360 & 90 & 45` <br>
[> check4_xp_whisker_turn.cpp]({{ '/algo/check4_xp_whisker_turn.cpp' | relative_url }})

#### 3.1 Critères
Le robot doit avancer en ligne droite, faire un virage à 360°, 90°, 45°. Les moustaches doivent réagir au contact.

#### 3.2 Validation
[video ligne droite 30cm] <br>
[video  360°] <br>
[video  90°] <br>
[video  45°] <br>
[video moustache L, R, F reaction]


## 4. Test sur batterie

[> check2_xp_straight.cpp]({{ '/algo/check2_xp_straight.cpp' | relative_url }}) - parametrage `TICKS_TARGET = 1000` <br>
[> check6_xp_circle.cpp]({{ '/algo/check6_xp_circle.cpp' | relative_url }})

#### 4.1 Critères
Faire une ligne droite 1m : lente, croisière, fast <br>
Faire 1 tour de rond : lente, croisière, fast <br>
. + test boutons : go=lent, retry=croisière, reset=fast

#### 4.2 Validation
[video ligne droite 1m, lente]
[video ligne droite 1m, croisière]
[video ligne droite 1m, fast]

[video 1 tour cercle, lente]
[video 1 tour cercle, croisière]
[video 1 tour cercle, fast]


# Références
[joshuaccl/Micromouse (GitHub)](https://github.com/joshuaccl/Micromouse)

---
layout: post
title: Mechanical Design
date: 2026-08-12 10:00:00 +0200
tags: [mechanics, hardware]
---

* TOC
{:toc}

🚧 todo : ajouter la position de l'essieu, la distance avec le nez, justifier la position des moustache également + indiquer la marque de la mesure d'odometry

<figure style="margin:0;">
  <img src="{{ '/assets/img/mouse_v0.png' | relative_url }}" alt="Design initial de la souris" style="max-width:100%; border-radius:6px;">
  <figcaption style="text-align:center; font-size:0.85rem;">Design d'assemblage des composants V0, en marron les plaques acrylique découpées au laser.</figcaption>
</figure>
<br>

Ce post détaille le processus de fabrication du corps et l'assemblage des composants sur le châssis. Ci-dessus le design initial imaginé. Pour une première composition j'utilise des supports en plastique acrylique découpés au laser.

Au départ le châssis était prévu en impression 3D (c.f. [BOM]({% post_url 2026-06-28-bom %})), mais pour un premier montage la découpe laser est plus simple et les itérations sont plus rapide. Il est plus facile de repercer ou d'élargir un trou à la main pour ajuster le positionnement. L'impression 3D reste pertinente pour une v2, une fois la géométrie figée.

Pour **le corps**, des supports en forme de rectangle pour valider le positionnement des composants. C'est ici qu'on va déterminer la taille de la souris : longueur, largeur, hauteur. Le mur mesurant 50mm, les moustaches doivent être positionnées en dessous pour détecter le mur.

Pour **les moustaches**, une structure en sandwich qui va maintenir le pivot des moustaches stable et droit d'un côté et de l'autre. La matière de la moustache doit être solide pour plus de précision. Enfin un mécanisme doit être prévu pour ramener la moustache à un état neutre défini.

## 1. Les contraintes

#### 1.1 Mécaniques

La souris doit être suffisamment petite pour circuler dans le labyrinthe et avoir de la marge dans les virages. Et également pouvoir faire les diagonales en ligne droite si on y arrive. Donc calculons l'espace libre dans une diagonale : <br>

<div style="display:flex; flex-wrap:nowrap; gap:16px; align-items:center;" markdown="1">
<figure style="margin:0; flex-shrink:0;">
  <img src="{{ '/assets/img/diag_mesure_transp.png' | relative_url }}" alt="Calcul de l'espace libre en diagonale" style="width:500px; border-radius:6px;">
  <figcaption style="text-align:center; font-size:0.85rem;">Schéma du labyrinthe illustrant le calcul de l'espace libre.</figcaption>
</figure>
<div style="flex:1; min-width:0; font-size:0.85em;" markdown="1">
**Théorème de Pythagore :**

$$\begin{aligned} \text{diag}^2 &= c^2 + c^2 \\ \text{diag} &= \sqrt{2} \times c \end{aligned}$$

Notre côté connu 180mm devient l'hypoténuse, on cherche donc le côté perpendiculaire à la trajectoire :

$$\begin{aligned} c &= \frac{\text{diag}}{\sqrt{2}} = \frac{180}{\sqrt{2}} \approx 127.28\text{mm} \end{aligned}$$

On calcule la diagonale du poteau 12mm :

$$\text{diag}_{\text{poteau}} = \sqrt{2} \times 12 \approx 16.97\text{mm}$$

On soustrait cette diagonale à notre côté :

$$\begin{aligned} \text{Espace libre} &= c - \text{diag}_{\text{poteau}} \\ &= 127.28 - 16.97 \\ & \approx 110.31\text{mm} \end{aligned}$$
</div>
</div>
<br>

Laissons une marge donc on va fixer **70-80mm** de large. Plus la souris est petite et plus elle pardonne les petites erreurs de positionnement. Et plus on peut optimiser la vitesse en ligne droite et en diagonale, mais c'est pas forcement la priorité pour la v1.

Chiffrons ce « pardon ». Dans un couloir droit, la largeur libre est la cellule moins l'épaisseur du poteau, soit $180 - 12 = 168$ mm. La souris centrée à 80 mm laisse donc de chaque côté :

$$\frac{168 - 80}{2} = 44\ \text{mm}$$

**44 mm par côté, c'est tout le budget d'erreur de navigation.** Chaque degré de dérive du cap et chaque millimètre d'erreur d'odométrie viennent y puiser. C'est ce chiffre qui fixe les tolérances des tests dans [Flash the critter]({% post_url 2026-08-20-flash-the-critter %}). À 70 mm de large on monterait à 49 mm, cinq de plus.

Dans la longueur, le point de référence est le **milieu de l'essieu** : c'est la position que l'odométrie calcule, et le point autour duquel le robot pivote. Le nez est à **70 mm** devant. Essieu au centre d'une cellule, il reste entre le nez et le mur d'en face :

$$84 - 70 = 14\ \text{mm}$$

C'est la course de la moustache avant, dont le rôle est précisément de toucher ce mur.

Concernant les moteurs, ils doivent être aligné sur le même axe, sinon le robot ne roule pas droit.

Le pivot de la moustache doit être parfaitement stable et aligné avec la puce AS5600 qui va lire l'angle de l'aimant. Les moustaches doivent être positionnées à une hauteur < 50mm.

Pour le placement des vis la distance du centre du trou au bord de la plaque doit être ≥ à 2 × le diamètre du trou, donc pour les vis : <br>
M2 = 2x Ø2,2mm → 5mm du bord <br>
M3 = 2x Ø3,2mm → 7mm du bord

#### 1.2 Signal

Pour que le signal soit propre il faut faire attention aux composants bruyants. Donc séparer physiquement les composants sensibles au bruit : le gyro et les capteurs AS5600, des composants générant du bruit : le moteur, buck et driver.

<div style="display:flex; flex-wrap:nowrap; gap:25px; align-items:center;" markdown="1">
<img src="{{ '/assets/img/taxis_bouton.gif' | relative_url }}" alt="Boutons de pilotage" style="width:350px; flex-shrink:0; display:block; margin-top:35px; border-radius:10px;">
<div style="flex:1; min-width:0;" markdown="1">
#### 1.3 Piloter : exploration, retry, reset

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


## 2. Châssis et moustaches

Dessiner les pièces avec le logiciel vectoriel = Inkscape <br>
Caractéristiques des supports : <br>
. matériel = acrylique car + léger et solide <br>
. couleur = transparent <br>
. épaisseur = 2mm <br>
. taille = on va essayer plusieurs longueurs <br>
. fichier SVG = [body_v1.svg]({{ site.repo }}/mecha/body_v1.svg) <br>
N'ayant pas de découpeuse laser, j'ai passé commande dans un magasin.

[photo du résultat après découpe = body_lazer_cut.jpg]

Les matières des moustaches à tester :

[photo alu, plastique, corde de piano, cuivre]

Pour la v1 on part sur **la tige alu Ø0.5mm**. C'est le meilleur compromis parmi les matières testées : assez rigide pour transmettre le contact au pivot sans se plier n'importe comment (contrairement au plastique), mais assez souple pour ne pas repousser le robot loin du mur (contrairement à la corde de piano). Elle se coupe et se met en forme facilement, ce qui compte pour une v1 où on ajuste souvent. Les autres matières restent à retester en v2, une fois la géométrie du pivot stabilisée.


## 3. Validation

AUSSI = check each component with a multimeter

Cette partie est itérative prototype, critère de validation, mesure, si le critère n'est pas respecté, alors on modifie le prototype est on remesure.

Les critères sont : la hauteur des moustaches < 50mm, l'ensemble est stable, le centre de gravité et le poids sont ok, la prise teensy est accessible, les boutons egalement.

Validation empiriquement :

**L'assemblage** <br>
Premier problème les moustaches sont trop haute, alors il faut descendre les AS5600 à l'étage 0 et la c'est ok.
Il est difficile de faire tombé les trous en face, car certain composants sont mals coupés comme le module AS5600. Alors prévoir une marge ou alors positionné à la main et utiliser une perceuse. Puis mesuré et ajuster le dessin vectoriel pour une v2. A part les moustaches le reste des critères sont respectés. <br>
C'est l'étape des galères Ikéa, les pièces qui finalement vont mieux ailleurs, celle qu'on ajoute et retire. Montage, démontage... prévoir des supports pour les tests, percer plusieurs trous pour essayer des positions de vis différentes. Iterer jusqu'à ce que le résultats satisfasse les critères et ensuite faire au propre pour la v1. <br>

<figure style="margin:0;">
  <img src="{{ '/assets/img/floor_mouse.png' | relative_url }}" alt="Positionnement v1 sur le plan du labyrinthe" style="max-width:100%; border-radius:6px;">
  <figcaption style="text-align:center; font-size:0.85rem;">Positionnement validé pour la v1</figcaption>
</figure>


## 4. Réflexion
Il faudrait améliorer le pivot des moustaches qui pour l'instant est un peu bancal, mais ça sera pour la v2.
La forme rectangle peut s'accrocher + facilement au coin en cas de dérive angulaire. Donc pour une v2 tester une forme circulaire ou octogonale.

# References

- [Micromouse Online — Chassis Layout](https://micromouseonline.com/micromouse-book/the-chassis/chassis-layout/)
- [Demo Micromouse Robot — Sam Dale](https://www.spdale.com/projects/demo-micromouse)


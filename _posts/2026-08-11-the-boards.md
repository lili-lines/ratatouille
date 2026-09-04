---
layout: post
title: The boards
date: 2026-08-11 10:00:00 +0200
tags: [electronics, hardware]
---

* TOC
{:toc}

🚧 todo : <br>
. stripboard vs perfboard, lequel pour quelle partie <br>
. comment faire les chemins (stripboard: couper les bandes / perfboard: fil point-à-point) <br>
. masse en étoile, comment l'organiser sur la carte <br>
. fil de câblage : calibre, isolation <br>
. photos avant/après soudure

. Comment connecter tout ensemble :
  * **Stripboard** — tu coupes au lieu de dessiner
    Avant (bande continue) :
  ●───●───●───●───●───●───●   ← tous ces trous sont déjà reliés entre eux

  Après avoir coupé entre 2 trous (avec une petite mèche ou un cutter) :
  ●───●───●  ✂  ●───●───●───●   ← maintenant 2 groupes séparés
  SI = Tu veux relier 2 bandes différentes (perpendiculaires) → il n'y a pas de connexion automatique entre bandes → tu ajoutes un fil de pontage (jumper) soudé entre les deux points.
  * **Perfboard** — tout est isolé, tu câbles fil par fil
    ●   ●   ●   ●   ●   ●   ●   ← chaque trou = isolé
    pour connecter on soude un fil entre les 2 pattes à relier ou  si les deux trous sont à coté on peut faire une goutte de soudure, attention risque de court-circuit si il y en a trop
    Pour la GND = un fil-tronc que tu ajoutes toi-même en selectionnant une rangé de trous
  * **breadboard**
  * **photosensible**
  * **PCB**
⚠️ faire courrir toute une ligne long ou larg pour la masse GND
  LES CHEMINS : Fil de câblage isolé (ok pour gros courant), Fil wire-wrap (pas de gros courrant sinon fond, ok pour les chemins de l'information/signal, attention fragile donc à fixer/coller pour qu'il ne bouge pas)

# References

- [Breadboard vs Protoboard: Which One Should You Use — JLCPCB](https://jlcpcb.com/blog/breadboard-vs-protoboard-guide)



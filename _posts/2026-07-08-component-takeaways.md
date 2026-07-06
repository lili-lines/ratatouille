---
layout: post
title: Component takeaways
date: 2026-07-08 10:00:00 +0200
tags: [electronics]
---

couleur fil :
. noir- /rouge+
. autre couleur pour les commandes

🚧 : puissance vs signal
 le signal et la puissance sont deux mondes séparés, avec chacun sa tension.
. garder l'analogie de l'eau et présenter tous les composants
. c'est quoi input et output, l'unité, les valeurs
. avec quel autre composant il est connecté

A confusion I had: in the chain **Teensy → driver → motor**, what does the **battery** send? A PWM too?

## No — the battery sends *power*, not a *signal*

The battery gives **constant DC power**: **7.4 V, all the time**. It's raw energy,
not information — it never "blinks" and never "says" anything.

Two different things reach the driver:

```
POWER :  Battery 7.4 V (constant) ─────────────┐
                                                ▼
                                        [ DRIVER TB6612FNG ] ──► Motor
                                                ▲
SIGNAL : Teensy ── PWM 3.3 V (on/off) ──────────┘
```

- **Battery → power**: 7.4 V constant, big current available. *(the energy)*
- **Teensy → signal**: PWM at 3.3 V, tiny current. *(the order)*
- **Driver**: takes the battery's 7.4 V and **chops** it on/off, following the
  Teensy's PWM order. The motor gets **chopped** 7.4 V.

## Water analogy 💧

- **Battery** = the pressurised **tank** (always full, constant).
- **Teensy (PWM)** = the **hand** on the tap, saying "open / close" very fast.
- **Driver** = the **tap** that opens and closes, following the hand.

The tank doesn't decide anything — it just supplies water. The hand commands, the
tap executes.

## Takeaway

**The battery sends power (constant 7.4 V), not a signal. The only signal (PWM)
comes from the Teensy.** The driver marries the two.


Pourquoi 2 tensions différentes
3,3 V = la tension du signal (l'ordre). Le Teensy « parle » en 3,3 V (c'est son niveau logique). Un signal a juste besoin d'être « haut » ou « bas » → 3,3 V suffit pour dire « on ». Ça ne transporte quasi aucune énergie (courant minuscule).
7,4 V = la tension de la puissance (l'énergie). Les moteurs, eux, ont besoin de force pour tourner → il leur faut la vraie puissance de la batterie (7,4 V, gros courant).
Le driver fait le pont entre les deux
Le TB6612FNG a exprès deux entrées séparées :

VCC = côté logique, alimenté en 3,3 V (il écoute l'ordre du Teensy).
VM = côté moteur, alimenté en 7,4 V (la puissance de la batterie).
Il lit le petit signal 3,3 V et l'utilise pour commuter le gros 7,4 V vers le moteur. Le 3,3 V ne « devient » jamais du 7,4 V : ce sont deux circuits différents que le driver relie.

L'analogie de l'interrupteur 💡
Pense à un interrupteur mural :

Ton doigt (faible, sans danger) appuie sur l'interrupteur = le signal 3,3 V.
La lampe est alimentée par le 230 V du mur = la puissance 7,4 V.
Ton doigt ne transporte pas le 230 V ! Il commande juste. L'interrupteur (= le driver) relie ta commande faible au circuit puissant.

En une phrase
Un signal n'a pas besoin d'avoir la même tension que la puissance qu'il commande. Le Teensy commande en 3,3 V, le driver commute la puissance en 7,4 V vers le moteur. Deux côtés, deux tensions, un seul composant qui les relie.

💡 C'est exactement ce que dit ta BOM : « logic VCC 2.7–5.5 V → 3.3 V from Teensy » (le signal) et « VM 2.7–13.5 V from 7.4 V » (la puissance).

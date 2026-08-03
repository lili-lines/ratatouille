---
layout: post
title: Clean whisker signal, PWM
date: 2026-07-28 10:00:00 +0200
tags: [electronics, signal, experiment]
---

* TOC
{:toc}

## 1. Pulse Width Modulation (PWM)

Continuously varying the motor's voltage is hard. Instead, the Teensy generates a PWM signal, a simple on/off command (3.3 V logic). The motor driver TB6612FNG follows it and switches the motor's 7.4 V supply on/off thousands of times per second.
What sets the power is the duty cycle = the share of time the signal stays **on**. <br>
80% **on** ≈ 80% max speed. Fast, and with no voltage regulation needed.

<div class="two-col">
<div class="col" style="text-align:center">
<img src="{{ '/assets/img/pwm_duty.gif' | relative_url }}" alt="PWM duty cycle animation" height="200" style="max-width:100%">
</div>
<div class="col" style="text-align:center">
<img src="{{ '/assets/img/jour_nuit.gif' | relative_url }}" alt="Day / night analogy" height="200" style="max-width:100%">
<br><small><em>Les Visiteurs, 1993 - jour, nuit, jour, nuit</em></small>
</div>
</div>

These fast switches create electrical noise that leaks into the whole circuit and can pollute the AS5600 whisker's fragile analog signal → the reading shakes.

🐭 In the MicroMouse: this is the "PWM-limited so the average stays ≈ 6 V" line from the BOM. The battery is 7.4 V, but at a duty cycle of ~81% the average the motor sees ≈ 6 V.


## 2. Clean whisker signal under PWM

It means the reading stays clean while the motors run, that's the challenge, since a clean signal at standstill is easy.

The noise defenses :
- the RC filter (resistor + capacitor) on each whisker line,
- the decoupling capacitors next to each chip,
- the star ground (one common ground point).


#### **Experiment**

blablabla

Hardware :
. power = on bench power supply :
    params in line :
    . voltage (7.4 V to mimic the battery) 
    . the current limit  (300-500 mA)
. Teensy
. 1-2 AS5600 + aimant
. 1 motor N20
. driver
. Breadboard + câbles Dupont
. Multimètre (vérifier les tensions avant de tout relier)

Les élèments annti-bruit :
ça serait interressant de checker leur efficacité 1/1.
. RC filter /moustache (R 1 kΩ + C 100 nF)
. condensateur decouplage (100 nF) près de chaque puce
. condensateur réservoir (≥ 470 µF)
. contrainte de câblage : câblage en étoile (masse commune unique)
. la proximité compte car le but est d'absorber les pics de courant au moment précis ou le moteur tire (démarrage changement PWM), si on branche loin, le fil/distance retarde sa reaction


🚧 todo : <br>
. xp : analyse du signal
. graph
. photo

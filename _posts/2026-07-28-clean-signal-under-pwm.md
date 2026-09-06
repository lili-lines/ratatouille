---
layout: post
title: Clean whisker signal, PWM
date: 2026-07-28 10:00:00 +0200
tags: [electronics, signal, experiment]
---

* TOC
{:toc}

## 1. Pulse Width Modulation (PWM)

Continuously varying the motor's voltage is hard. Instead, the Teensy generates a PWM signal, a simple on/off command 3.3 V logic. The motor driver TB6612FNG follows it and switches the motor's 7.4 V supply on/off thousands of times per second.
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

These fast switches create electrical noise that leaks into the whole circuit and can pollute the AS5600 whisker's fragile analog signal → the reading shakes. This is the signal side and the power side of [Signal vs power]({% post_url 2026-07-07-signal-vs-power %}) sharing the same circuit, and here the power side wins.

🐭 In the MicroMouse: this is the "PWM-limited so the average stays ≈ 6 V" line from the BOM. The battery is 7.4 V, but at a duty cycle of ~81% the average the motor sees ≈ 6 V.


## 2. Clean whisker signal under PWM

It means the reading stays clean while the motors run, that's the challenge, since a clean signal at standstill is easy.

The noise defenses :
- the RC filter (resistor + capacitor) on each whisker line,
- the decoupling capacitors next to each chip,
- the star ground (one common ground point).


#### 2.1 Experiment

 **Objectif** <br>
 🚧

**Matériel** <br>
. power = on bench power supply : <br>
    params in line : <br>
    . voltage (7.4 V to mimic the battery) <br>
    . the current limit  (300-500 mA) <br>

. Teensy <br>
. 1-2 AS5600 + magnet <br>
. 1 motor N20 <br>
. driver <br>
. Breadboard + Dupont wires <br>
. Multimeter, check the voltages before wiring everything together <br>

The noise defenses, to check their efficacy 1 by 1 : <br>
. RC filter / whisker (R 1 kΩ + C 100 nF) <br>
. decoupling capacitor (100 nF) next to each chip <br>
. reservoir capacitor (≥ 470 µF) <br>
. wiring constraint: star ground (one common ground point) <br>

<figure style="margin:0;">
  <img src="{{ '/assets/img/xp_signal_PWM.png' | relative_url }}" style="max-width:100%; display:block; margin:0 auto;">
  <figcaption style="text-align:center; font-size:0.85rem;">Schematic to wire the components of the experiment</figcaption>
</figure>

**Protocol:** <br>
. Baseline = motor OFF, with 4 defenders → record the signal <br>
. all 4 defenders, motor ON (PWM running) → record the signal, compare to the baseline <br>
. remove the RC filter only, motor ON → record, compare, then put it back
. remove the decoupling capacitor only, motor ON → record, compare, then put it back <br>
. remove the reservoir capacitor only, motor ON → record, compare, then put it back <br>
. break the star ground (daisy-chain), motor ON → record, compare, then restore it <br>

**Success criterion** <br>
with the motor running, the whisker reading at rest stays within the same noise band as with the motor off

Each step changes only one thing at a time, so any change in noise can be attributed to that one defender.

**Result** <br>
🚧 todo <br>
. graph : signal with vs without each defender <br>


# References

- [PWM 'noise' on AnalogRead input — Arduino Forum](https://forum.arduino.cc/t/pwm-noise-on-analogread-input/295606)

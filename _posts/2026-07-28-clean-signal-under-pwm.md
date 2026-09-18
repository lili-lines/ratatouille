---
layout: post
title: Clean whisker signal, PWM
date: 2026-07-28 10:00:00 +0200
tags: [electronics, hardware, signal, experiment]
---

**What I learned** <br>
The noise on the whisker line is real (~3.5 mV rms, the same at 10-bit and 12-bit), and it has two parts. The constant fuzz is reduced 3× by the four defenders working together, but no single one stands out. The rare spikes are not touched by them, they come from the setup. The experiment gives the value of the normal noise, ~90 counts peak-to-peak, which becomes the whisker tolerance for the rest of the project.


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

The noise defenses:
- the RC filter, resistor + capacitor, on each whisker line,
- the decoupling capacitors next to each chip,
- the reservoir capacitor on the motor rail,
- the star ground, one common ground point.

#### 2.1 Experiment

**Goal** <br>
Read the AS5600 signal while the motors are ON, and test how well the noise filters work.

**Material** <br>
. a bench power supply: voltage 7.4 V, current limit 300-500 mA <br>

<div class="two-col">
<div class="col">
. Teensy <br>
. 1-2 AS5600 + magnet <br>
. 1 motor N20 <br>
. driver <br>
. Breadboard + Dupont wires <br>
. Multimeter, check the voltages before wiring everything together <br>
</div>
<div class="col">
The noise defenses: <br>
. RC filter / whisker (R 1 kΩ + C 100 nF) <br>
. decoupling capacitor (100 nF) next to each chip <br>
. reservoir capacitor (≥ 470 µF) <br>
. wiring: star ground, one common GND point <br>
</div>
</div>

<div style="display:flex; flex-wrap:wrap; gap:16px; align-items:flex-start;">
  <figure style="flex:1; min-width:260px; margin:0;">
    <img src="{{ '/assets/img/xp_signal_PWM.png' | relative_url }}" alt="Wiring schematic of the experiment" style="width:100%; display:block;">
    <figcaption style="text-align:center; font-size:0.85rem;">Schematic to wire the components of the experiment</figcaption>
  </figure>
  <figure style="flex:1; min-width:260px; margin:0;">
    <img src="{{ '/assets/img/IMG_9324.JPG' | relative_url }}" alt="The experiment on the breadboard: Teensy, driver, reservoir capacitor, RC filter, AS5600 and the N20 motor" style="width:100%; display:block; border-radius:6px;">
    <figcaption style="text-align:center; font-size:0.85rem;">The breadboard, the magnet on its holder facing the AS5600</figcaption>
  </figure>
</div>

**Protocol:** <br>
Each step changes only one thing at a time, at 2 motor speeds, PWM 130 and 206. <br>

<div class="two-col">
<div class="col">
. baseline = motor OFF, with 4 defenders <br>
. 4 defenders, motor ON PWM running <br>
. no defenders, motor ON <br>
. star ground only, motor ON <br>
</div>
<div class="col">
. remove RC filter only, motor ON <br>
. remove decoupling capacitor only, motor ON <br>
. remove reservoir capacitor only, motor ON <br>
. break star ground (daisy-chain), motor ON <br>
</div>
</div>
<br>

Upload on the Teensy: <br>
[xp2_pwm.cpp]({{ site.repo }}/algo/xp2_pwm.cpp) <br>
To record the data: [xp2_pwm_log.py]({{ site.repo }}/algo/xp2_pwm_log.py) <br>
To extract the stats: [xp2_compare_12bit.py]({{ site.repo }}/algo/xp2_compare_12bit.py)

**Success criterion** <br>
With the motor ON, removing a defender makes the reading measurably noisier than with all four

**Result** <br>
A first run was done at 10-bit (see Appendix A). Since the AS5600 is a 12-bit sensor, the experiment was redone at 12-bit, `analogReadResolution(12)` 0–4095.

<figure style="margin:0;">
  <img src="{{ '/assets/plot_xp/xp_compare_all_12b.png' | relative_url }}" alt="Whisker signal for the 11 conditions at 12-bit, each centered on its own mean" style="max-width:100%; display:block; margin:0 auto;">
  <figcaption style="text-align:center; font-size:0.85rem;">12-bit, in blue the baseline, in green all defenders, in purple one defender removed at a time.</figcaption>
</figure>
<br>

<figure style="margin:0;">
  <img src="{{ '/assets/plot_xp/xp_stats_table.png' | relative_url }}" alt="Table of the mean, standard deviation, relative std and peak-to-peak per condition at 12-bit" style="max-width:100%; display:block; margin:0 auto;">
  <figcaption style="text-align:center; font-size:0.85rem;">The same numbers as a table. std/mean rather than std alone, because the magnet moves a little between conditions. MAD = median absolute deviation, the constant fuzz, blind to the rare spikes. 5-95 % = width of the band that holds 90 % of the samples.</figcaption>
</figure>
<br>

Data: the 15 recordings and the stats table, in [data_xp/as5600_12bit]({{ site.repo }}/assets/data_xp/as5600_12bit)

. The noise is real. In volts, 10-bit and 12-bit give the same std, ~3.5 mV. A rounding artefact would have collapsed at 12-bit. <br>
. The defenders do reduce the noise, but not the part the std was looking at. The noise has two parts. The constant fuzz (MAD) is 1 count with the four defenders and 3 counts with none, three times thicker, and the 5-95 % band goes from 10 to 15 counts. The rare spikes, which set the peak-to-peak, are not reduced: 56-66 counts with the defenders, 35-41 without. They come from somewhere else, probably the magnet on its holder. <br>

Success criterion partly met: without the defenders the reading is measurably fuzzier (×3), but no single defender stands out, they work as a group. The spikes are not their job.

What the experiment gives instead is the value of the normal noise: motor ON or OFF, the reading stays within ~90 counts peak-to-peak. This is the whisker tolerance used in [Flash the critter]({% post_url 2026-08-20-flash-the-critter %}). It was measured on a breadboard with one unloaded motor, so it must be checked again on the assembled mouse.
<br><br>

# References

- [TI — Four ways to improve your ADC system power supply rejection](https://e2e.ti.com/blogs_/archives/b/precisionhub/posts/four-ways-to-protect-your-adc-system-power-supply-rejection)
- [Solo — Electrical Noise Reduction Techniques for Motor Controllers](https://www.solomotorcontrollers.com/blog/electrical-noise-reduction/)
<br><br>

# Appendix A
The 10-bit run, for comparison

<figure style="margin:0;">
  <img src="{{ '/assets/plot_xp/xp_compare_all.png' | relative_url }}" alt="Whisker signal for the 11 conditions, with std and peak-to-peak" style="max-width:100%; display:block; margin:0 auto;">
  <figcaption style="text-align:center; font-size:0.85rem;">10bits precision, in blue the baseline, in green all defenders, in purple one defender removed at a time</figcaption>
</figure>
<br>

<figure style="margin:0;">
  <img src="{{ '/assets/plot_xp/xp_stats_table_10b.png' | relative_url }}" alt="Table of the mean, standard deviation and peak-to-peak per condition" style="max-width:100%; display:block; margin:0 auto;">
  <figcaption style="text-align:center; font-size:0.85rem;">The same numbers as a table</figcaption>
</figure>
<br>
---
layout: post
title: Insights & Takeaways
date: 2026-06-27 10:00:00 +0200
tags: [electricity, electronics]
---

### 📚 key Concepts

#### 1. Voltage and Current
In electricity there are 2 important values: Voltage and Current. They drive every decision.
To understand them, let's use the waterfall analogy.

![Voltage and current]({{ "/assets/img/volt_ampere.png" | relative_url }})

<div class="two-col" markdown="1">

<div class="col" markdown="1">
⚡ **Voltage, in Volts (V)** = the height: the taller the waterfall, the more speed and force the water gains. (tension)
In electricity, voltage is that "push" (the potential difference) that drives the electrons through the circuit.

🐭 In the MicroMouse: the logic part (the brain/microcontroller) doesn't need much force to move its information around — it runs on a small waterfall of 3.3V or 5V. The motors, however, need more force to spin and move the mouse, so we give them a taller waterfall, for example 7.4V.
</div>

<div class="col" markdown="1">
⚡ **Current, in Amperes (A)** = the width: how much water flows, the flow rate passing every second. (intensity)

🐭 In the MicroMouse: the sensors use very little power — just a tiny trickle of water (a few mA). The motors, especially when the robot has to stop suddenly or speed up through a turn in the maze, open the valves wide and need a big flow of water (several A).
</div>

</div>

💪 Power (Watts) = the strength of the water wheel at the bottom of the waterfall that produces energy (like the motors that turn the wheels). Its power depends on both factors:
 
 $$\text{Power (Watts)} = \text{Voltage (Volts)} \times \text{Current (Amperes)}$$
 
 If you have a very tall waterfall (high Volts) but only a tiny trickle of water (low Amperes), the wheel won't turn. If you have a low waterfall (low Volts) with lots of water (high Amperes), the water just flows slowly over the wheel without managing to push it.

 🐭 For a fast and powerful mouse, you need the right combination: enough height (7.4V) and a good water flow (2A or more).

#### 2. AC/DC & Polarity (+/- or Phase/Neutral)
* **DC (Direct Current / Courant Continu):** The current flows in one single direction (from **+** to **-**). <br>
🐭 This is what we use in MicroMouse! Batteries and microcontrollers run exclusively on DC. Polarity matters: reversing $+$ and $-$ can instantly destroy your components.
* **AC (Alternating Current / Courant Alternatif):** The current constantly changes direction. This is what comes out of your wall outlets (Phase and Neutral). We do not use AC directly inside the robot, but your battery charger connects to it.

#### 3. Microcontroller (Microcontrôleur)
The "brain 🧠" of the MicroMouse. It's a tiny computer on a single integrated circuit (like an Arduino, STM32, or ESP32). It contains a processor, memory, and programmable inputs/outputs (GPIO) to read sensors (infrared, encoders) and control actuators (motors).

#### 4. Fuse (Fusible)
The ultimate security 🧯 guard of your circuit. A fuse is a deliberate weak point designed to melt and break the circuit if the current ($A$) gets too high (e.g., in case of a short circuit). It prevents your expensive components or your LiPo battery from catching fire.

#### 5. Resistors & Filters (Résistances et Filtres)
* **Resistor:** Limits the flow of electrical current (like narrowing a water pipe). Crucial for protecting LEDs from burning out or creating specific voltage levels.
* **Filters:** Combinations of resistors and capacitors used to clean up electrical signals. They block unwanted noise (high frequencies) and let the useful signal pass through, which is essential for accurate sensor readings.

#### 6. Decoupling (Découplage)
When motors start or spin, they create heavy electrical noise and sudden voltage drops on the power line. This can glitch or reset the microcontroller. 
* **The Fix:** We place small capacitors (called decoupling capacitors) physically as close as possible to the microcontroller's power pins. They act like tiny local energy reservoirs to stabilize the voltage.

#### 7. Star vs. Delta Connections (Étoile / Triangle)
Star/Delta is just a way to wire a motor that has 3 phases (3 coils), so it only concerns brushless motors. The 3-phase idea is used in **high-voltage industry (380 V)** but also in **low voltage** (a brushless on a small battery) — it depends on the coils, not the voltage.

| Motor | Wires | Star/Delta? |
|---|---|---|
| **DC brushed** (with brushes) — *my N20* 🐭 | 2 (+/–) | ❌ no |
| **Stepper** | 4–6 | ❌ no |
| **Brushless / BLDC** (no brushes) | 3 phases | ✅ yes |

*BLDC = Brushless Direct Current*

🐭 We use simple 2-wire brushed N20 motors, so Star/Delta does not apply here. It is just background, useful only if you later switch to brushless.

#### 8. PCB (Printed Circuit Board / Circuit Imprimé)
The green/black/blue board that replaces messy jumper wires. It mechanically supports and electrically connects electronic components using conductive pathways (traces) etched from copper sheets laminated onto a non-conductive substrate. Designing a custom PCB is a major step in making a MicroMouse compact and reliable.



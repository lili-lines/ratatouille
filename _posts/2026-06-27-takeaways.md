---
layout: post
title: Takeaways, starter
date: 2026-06-27 10:00:00 +0200
tags: [electricity, electronics]
---

* TOC
{:toc}

To choose the parts of the mouse and understand how they work together, you need the basics of electronics: voltage, current, and the usual components with their role.


## 1. Voltage and Current

In electricity there are 2 important values: Voltage and Current. They drive every decision about component. To understand them, here is the waterfall analogy.

![Voltage and current]({{ "/assets/img/volt_ampere.png" | relative_url }})

<div class="two-col" markdown="1">
<div class="col" markdown="1">
⚡ **Voltage, in Volts (V)** = the height: the taller the waterfall, the more speed and force the water gains (tension).
So voltage is that force that drives the electrons through the circuit.

🐭 In the MicroMouse: the logic part (MCU) needs a little force to move its information around, a small waterfall 3.3V or 5V. The motors need more force to spin and move the mouse, a taller waterfall 7.4V.
</div>

<div class="col" markdown="1">
⚡ **Current, in Amperes (A)** = the width: how much water flows, the flow rate passing every second. (intensity)

🐭 In the MicroMouse: the sensors use little power, just a tiny trickle of water, few mA. The motors, especially when the robot has to stop suddenly or speed up through a turn in the maze, open the valves wide and need a big flow of water, several A.
</div>
</div>

💪 **Power (Watts)**, the strength of the water wheel at the bottom of the waterfall that produces energy. Its power depends on both factors:
 
 $$\text{Power (Watts)} = \text{Voltage (Volts)} \times \text{Current (Amperes)}$$
 
If you have a very tall waterfall (high Volts) but only a tiny trickle of water (low Amperes), the wheel won't turn. If you have a low waterfall (low Volts) with lots of water (high Amperes), the water just flows slowly over the wheel without managing to push it.

🐭 for a fast and powerful mouse, you need the right combination: enough height 7.4V and a good water flow ≥ 2A.

🔥 **Joule heating**, part of the power is lost as heat in every wire and component:

$$\text{Heat (Watts)} = \text{Resistance (Ohms)} \times \text{Current (Amperes)}^2$$

Only the current is in the equation, a wire heats because of how much water flows through it. And it is squared, so twice the current means four times the heat.

🐭 in the mouse, the wires are sized by the Amperes they carry, not by the Volts. The motor wires see several A, they need to be thicker than the sensor wires that see a few mA.

🚄 **Hertz (Hz)** describe how fast something changes over time. V & A describe how much, how much pressure (V), how much flow (A), at a given instant. 

**Direct Current (DC)** one direction $+$ → $-$ <br>
🐭 this is what the mouse uses. Batteries and microcontrollers run exclusively on DC. Polarity matters: reversing $+$ and $-$ can instantly destroy components. <br>

**Alternating Current (AC)** the current changes direction. This is what comes out of your wall outlets (phase & neutral). There is no AC inside the robot, but the battery charger connects to it.


## 2. Components

**Microcontroller** It's a tiny computer on a single integrated circuit (like an Arduino, STM32, ESP32). It contains a processor, memory, and programmable IN/OUT (GPIO) to read sensors (infrared, encoders) and control actuators (motors). <br>

**Fuse** The security 🧯 guard of your circuit. A fuse is a deliberate weak point designed to melt and break the circuit if the current (A) gets too high (e.g. a short circuit). It prevents components from catching fire 🔥. <br>

**Resistor:** Limits the flow of electrical current. Crucial for protecting LEDs from burning out or creating specific voltage levels. <br>

**Filters:** Combinations of resistors and capacitors used to clean up electrical signals. They block unwanted noise (high frequencies) and let the useful signal pass through, which is essential for accurate sensor readings.

The resistance value comes from Ohm's law[^1]: <br>

$$R = V / I$$

$V$ : voltage <br>
$I$ : current intensity A <br>
$R$ : resistance in ohms Ω <br>

**Decoupling** When motors start or spin, they create heavy electrical noise and sudden voltage drops on the power line. This can glitch or reset the microcontroller. <br>

**The Fix:** Place small decoupling capacitors physically as close as possible to the microcontroller's power pins. They act like tiny local energy reservoirs to stabilize the voltage (V). <br>

**Motor** Torque is the twisting force to push the robot forward. Speed is how fast. The two trade against each other. Asking for more torque always costs speed.

$$\text{Mechanical power} = \text{Torque} \times \text{Rotation speed}$$

A motor takes electrical power in and gives mechanical power out. So for a given power you choose where to spend it, in force or in speed.
A gearbox does that trade: it divides the speed and multiplies the torque. 
N20 has one, which turns a small fast motor into a slower but stronger wheel drive.

🐭 low PWM = the motor has little torque to spare, so any change of friction changes its speed. <br>

**Star vs Delta Connections** is a way to wire a motor that has 3 phases, only brushless motors. The 3-phase idea is used in high-voltage industry (380V) but also in low voltage, a brushless on a small battery, it depends on the coils, not the voltage.

| Motor | Wires | Concerned? |
|---|---|---|
| DC brushed (with brushes), N20 🐭 | 2 ($+$/$–$) | ❌ no |
| Stepper | 4–6 | ❌ no |
| Brushless / BLDC (no brushes) | 3 phases | ✅ yes |

The manufacturer choose : star = the 3 coils joined at one common point, delta = the 3 coils in a closed loop. Star draws less current and gives a smoother torque at low speed, delta gives more speed.

🐭 the mouse uses simple 2-wire brushed N20 motors, so Star/Delta does not apply here. It is just background, useful only for a later switch to brushless. <br>

**Printed Circuit Board (PCB)** green/black/blue board that replaces messy jumper wires. It mechanically supports and electrically connects electronic components using conductive pathways (traces) etched from copper sheets laminated onto a non-conductive substrate. <br>

🐭 designing a custom PCB is a major step in making a MicroMouse compact and reliable.

<br>

With the words in place, the next post picks the parts: the bill of materials, for the mouse and for the experiments.

<br>

[^1]: [SparkFun — Voltage, Current, Resistance, and Ohm's Law](https://learn.sparkfun.com/tutorials/voltage-current-resistance-and-ohms-law/all)


*[voltage]: also called the potential difference = the difference in electrical energy between 2 points in a circuit
*[BLDC]: Brushless Direct Current

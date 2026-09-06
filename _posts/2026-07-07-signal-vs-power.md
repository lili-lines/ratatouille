---
layout: post
title: Signal vs power
date: 2026-07-07 10:00:00 +0200
tags: [electricity]
---

* TOC
{:toc}


🟣🔴 + ⚡ <br> 
🟡🟢 + ℹ️🎮 <br>
⚫ - <br>

## 1. What flows

#### 1.1 Signal
Signal = the physical carrier of the information on a wire: a voltage or a pulse (e.g. 3.3 V = "on"). It carries the information, not energy, so its current is tiny.

**Information** ℹ️, what we want to know or command, e.g. the whisker is bent, go at 80%.

The information is encoded in a physical property of the signal, a voltage, a duration, a sequence of bits. Each component is built to read that property by a known rule, the protocol.

Three ways to encode it:

| Encoding | The info is in… | Who reads it |
|---|---|---|
| Logic | voltage: 3.3V=1, 0V=0 | a digital pin |
| Analog | value of the voltage, 1.6 V = a given angle | an ADC |
| Protocol (I2C, PWM…) | a coded sequence of pulses | dedicated hardware in the chip |

→ a component recognizes the info because it applies the same convention as the sender. Like two people speaking the same language.

**Order** 🎮, travelling the other way, it tells a component what to do:
. `STBY` HIGH wakes the driver, 
. `AIN1`/`AIN2` set the direction, 
. a PWM duty sets the speed.


Information and order are physically identical, a voltage on a wire. What changes is who decides: the sensor reports, the MCU commands.

#### 1.2 Power ⚡
Power = energy, what actually does the work, spin a motor. It needs a real voltage and a big current.

🐭 In the mouse: the Teensy sends information as a 3.3V signal, an order. The battery sends power 7.4V that moves the motors. Same wires, two different jobs.


#### 1.3 Common GND ⚫
GND is not a third kind of wire, it is the shared −: signals and power are both +. Separated on the + side, they meet on the − side.

A wire is not a perfect 0Ω, like a resistance it eats a bit of the voltage. Ohm's law tells how much:
<div style="display:flex; flex-wrap:wrap; gap:24px; align-items:flex-start;" markdown="1">
<div style="flex:1; min-width:0;" markdown="1">

$$V = R \times I$$

V = the voltage lost across the element, in volts <br>
R = its resistance, in ohms (Ω) <br>
I = the current going through it, in A
</div>
<div style="flex:1; min-width:0;" markdown="1">

$$R = \rho \times L / S$$

ρ (rho) = resistivity of the material, e.g. copper is a good conductor, ρ ≈ 1.7×10⁻⁸ Ω·m <br>
L = length of the wire → the longer the wire, the bigger R <br>
S = section, the thickness of the wire → the thicker the wire, the smaller R
</div>
</div>

A 20 cm AWG26 Dupont wire is about 0.03 Ω. The same wire, two uses: <br>
. power wires → motor → 3A → 0.03x3=90mV → must be short and thick <br>
. signal wires → AS5600 → 0.03A → 0.03x0.03=0.9mV → tolerant <br>

The same wire, 100× difference, depending on the current going through it. So the length matters as much for the + as for the -.

The + paths are separated, so they never touch each other. But everybody shares the same ground, so this is where the noise can travel. That's why the GND wires all meet at one precise rendez-vous point: no wire length left, so no R, so no path left for the noise.

To illustrate, take a measurement of the AS5600 read by the Teensy: <br>
. measurement = signal - reference, the signal comes from the AS5600 and the error is the GND of the AS5600 - the GND of the Teensy, so the bigger that gap, the more wrong the measurement.

That's why the ground is wired as a star, at one single point, so that no strong current travels on the ground path between the sensors and the microcontroller.


## 2. From order to power

The driver is where the two worlds meet:
- **VCC** = logic side, 3.3 V, it listens to the Teensy's order.
- **VM** = motor side, 7.4 V, the battery's power.

Sharing one circuit between the two has a cost: the power side is noisy and the signal side is fragile. Measured in [Clean whisker signal, PWM]({% post_url 2026-07-28-clean-signal-under-pwm %}).

# References

- [TB6612FNG Hookup Guide — SparkFun Learn](https://learn.sparkfun.com/tutorials/tb6612fng-hookup-guide/all)

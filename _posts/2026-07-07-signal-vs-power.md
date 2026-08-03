---
layout: post
title: Signal vs power
date: 2026-07-07 10:00:00 +0200
tags: [electronics]
---

* TOC
{:toc}

C'est quoi qui circul ?

couleur fil: <br> 
. noir- /violet+ ou rouge <br>
. autre couleur pour les commandes (vert & jaune) <br>

#### **Signal**
Signal = the physical carrier of the information on a wire: a voltage or a pulse (e.g. 3.3 V = "on"). It carries the information, not energy, so its current is tiny.

**Information**, what we want to know or command, e.g. the whisker is bent, go at 80%.

The information is encoded in a physical property of the signal, a voltage, a duration, a sequence of bits. Each component is built to read that property by a known rule, the protocol.

Three ways to encode it:

| Encoding | The info is in… | Who reads it |
|---|---|---|
| Logic | voltage: 3.3V=1, 0V=0 | a digital pin |
| Analog | value of the voltage, 1.6 V = a given angle | an ADC |
| Protocol (I2C, PWM…) | a coded sequence of pulses | dedicated hardware in the chip |

→ a component recognizes the info because it applies the same convention as the sender. Like two people speaking the same language.

**Order**
🚧 todo :

#### **Power**
Power = energy: what actually does the work, spin a motor. It needs a real voltage and a big current.

🐭 In the mouse: the Teensy sends information as a 3.3$V$ signal, an order. The battery sends power 7.4$V$ that moves the motors. Same wires, two different jobs.


#### **Voltages**

- 3.3 V = the signal, logic, the order. It carries almost no energy, tiny current.
- 7.4 V = the power, the energy.

The driver bridges the two. The TB6612FNG has two separate inputs on purpose:
- **VCC** = logic side, powered at 3.3 V (it listens to the Teensy's order).
- **VM** = motor side, powered at 7.4 V (the battery's power).

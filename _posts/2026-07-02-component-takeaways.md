---
layout: post
title: 🥡 Takeaways, component
date: 2026-07-02 10:00:00 +0200
tags: [electronics]
---

One card per component: what it does, what goes IN and OUT, the traps ⚠️, and a closer look 🔍.

**Legend** what flows on each IN / OUT: ⚡ power, ℹ️ information, 🎮 order — the difference between the three is explained in [Signal vs power]({% post_url 2026-07-07-signal-vs-power %}).

{% include component-carousel.html %}

<br>

The connectors :

<div style="display:flex; flex-wrap:wrap; justify-content:center; gap:16px; margin-top:16px;">
  <figure style="margin:0; flex:1 1 130px; max-width:170px;">
    <img src="{{ '/assets/img/SM_connector.png' | relative_url }}" alt="SM connector" style="width:100%; height:110px; object-fit:contain; border-radius:6px;">
    <figcaption style="text-align:center; font-size:0.85rem;">SM connector (male/female)</figcaption>
  </figure>
  <figure style="margin:0; flex:1 1 130px; max-width:170px;">
    <img src="{{ '/assets/img/JST_XH_n_pins_connector.jpg' | relative_url }}" alt="JST-XH connector" style="width:100%; height:110px; object-fit:contain; border-radius:6px;">
    <figcaption style="text-align:center; font-size:0.85rem;">JST-XH (2 & 3 pins)</figcaption>
  </figure>
  <figure style="margin:0; flex:1 1 130px; max-width:170px;">
    <img src="{{ '/assets/img/banana_plug.png' | relative_url }}" alt="Banana plug" style="width:100%; height:110px; object-fit:contain; border-radius:6px;">
    <figcaption style="text-align:center; font-size:0.85rem;">Banana plug 🍌</figcaption>
  </figure>
  <figure style="margin:0; flex:1 1 130px; max-width:170px;">
    <img src="{{ '/assets/img/aligator_connector.png' | relative_url }}" alt="Alligator clip" style="width:100%; height:110px; object-fit:contain; border-radius:6px;">
    <figcaption style="text-align:center; font-size:0.85rem;">Alligator clip 🐊</figcaption>
  </figure>
  <figure style="margin:0; flex:1 1 130px; max-width:170px;">
    <img src="{{ '/assets/img/dupont_fils.jpg' | relative_url }}" alt="Dupont jumper wires" style="width:100%; height:110px; object-fit:contain; border-radius:6px;">
    <figcaption style="text-align:center; font-size:0.85rem;">Dupont / jumper wire</figcaption>
  </figure>
  <figure style="margin:0; flex:1 1 130px; max-width:170px;">
    <img src="{{ '/assets/img/barrel_plug.png' | relative_url }}" alt="Barrel plug" style="width:100%; height:110px; object-fit:contain; border-radius:6px;">
    <figcaption style="text-align:center; font-size:0.85rem;">Barrel plug</figcaption>
  </figure>
</div>
<br>

What I keep from writing these cards: every component boils down to IN → OUT, and most of the traps are voltage ones. 3.3 V logic that is not 5 V tolerant, power lines that must never reach the MCU, an analog output that scales with VCC.

# References

The sources behind the cards :

- [Teensy 4.0 — pjrc.com](https://www.pjrc.com/store/teensy40.html), the 40 pins pinout
- [AS5600 — ams-osram](https://ams-osram.com/products/sensor-solutions/position-sensors/ams-as5600-position-sensor), datasheet of the magnetic angle sensor
- [MPU-6050 module — components101](https://components101.com/sensors/mpu6050-module), pinout & registers
- [TB6612FNG Hookup Guide — SparkFun](https://learn.sparkfun.com/tutorials/tb6612fng-hookup-guide/all), how to drive the 2 motors
- [MP1584 — Monolithic Power Systems](https://www.monolithicpower.com/en/mp1584.html), datasheet of the buck converter

---
layout: post
title: The component
date: 2026-07-02 10:00:00 +0200
tags: [electronics, hardware]
---

Before wiring anything, get to know each component, Teensy[^1], AS5600[^2], MPU-6050[^3], TB6612FNG[^4], MP1584[^5], wheel encoders[^6]: one card each, with what it does, what goes IN and OUT, the traps ⚠️, and a closer look 🔍. The IN and OUT are what decide where each one sits on the board.

**Legend** what flows on each IN / OUT: ⚡ power, ℹ️ information, 🎮 order — the difference between the three is explained in [Signal vs power]({% post_url 2026-07-07-signal-vs-power %}).

{% include component-carousel.html %}

<br>

The connectors too, listed here to get familiar with them and keep them in mind.

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

These cards are done, they will serve throughout the experiments, to make and check the wiring. Now, what flows between the components, and how they talk to each other.

<br>

[^1]: [Teensy 4.0 — pjrc.com](https://www.pjrc.com/store/teensy40.html), the 40 pins pinout
[^2]: [AS5600 — ams-osram](https://ams-osram.com/products/sensor-solutions/position-sensors/ams-as5600-position-sensor), datasheet of the magnetic angle sensor
[^3]: [MPU-6050 module — components101](https://components101.com/sensors/mpu6050-module), pinout & registers
[^4]: [TB6612FNG Hookup Guide — SparkFun](https://learn.sparkfun.com/tutorials/tb6612fng-hookup-guide/all), how to drive the 2 motors
[^5]: [MP1584 — Monolithic Power Systems](https://www.monolithicpower.com/en/mp1584.html), datasheet of the buck converter
[^6]: [Pololu — Quadrature encoders](https://www.pololu.com/docs/0J83/5.4), CPR at the wheel = gearbox ratio x counts at the motor shaft

---
layout: post
title: Signal vs Power
date: 2026-07-07 10:00:00 +0200
tags: [electronics]
---

A confusion I had: in the chain Teensy → driver → motor, what does the **battery** send? A PWM too?

## No, the battery sends power, not a signal

The battery gives **constant DC power**: 7.4 V, all the time. It is raw energy, not information, it never "blinks" and never "says" anything.

Two different things reach the driver:

```
POWER :  Battery 7.4 V (constant) ─────────────┐
                                                ▼
                                        [ DRIVER TB6612FNG ] ──► Motor
                                                ▲
SIGNAL : Teensy ── PWM 3.3 V (on/off) ──────────┘
```

- **Battery → power**: 7.4 V constant, big current available (the energy).
- **Teensy → signal**: PWM at 3.3 V, tiny current (the order).
- **Driver**: takes the battery's 7.4 V and chops it on/off, following the Teensy's PWM order. The motor gets chopped 7.4 V.

## Water analogy 💧

- **Battery** = the pressurised tank (always full, constant).
- **Teensy (PWM)** = the hand on the tap, saying "open / close" very fast.
- **Driver** = the tap that opens and closes, following the hand.

The tank doesn't decide anything, it just supplies water. The hand commands, the tap executes.

## Why two different voltages

- **3.3 V = the signal** (the order). The Teensy speaks at 3.3 V (its logic level). A signal only needs to be "high" or "low", so 3.3 V is enough to mean "on". It carries almost no energy (tiny current).
- **7.4 V = the power** (the energy). The motors need force to turn, so they need the battery's real power (7.4 V, big current).

The driver bridges the two. The TB6612FNG has two separate inputs on purpose:
- **VCC** = logic side, powered at 3.3 V (it listens to the Teensy's order).
- **VM** = motor side, powered at 7.4 V (the battery's power).

It reads the small 3.3 V signal and uses it to switch the big 7.4 V to the motor. The 3.3 V never "becomes" 7.4 V: they are two different circuits that the driver links.

#### The switch analogy 💡
Think of a wall switch: your finger (weak, safe) presses the switch = the 3.3 V signal. The lamp is powered by the mains 230 V = the 7.4 V power. Your finger doesn't carry the 230 V, it just commands. The switch (= the driver) links your weak command to the powerful circuit.

→ A signal doesn't need the same voltage as the power it commands.

## How it flows in the robot

**Power path** (energy, thick wires), with the current in each wire:
```
LiPo 2S 7.4 V ─► Fuse ─► Switch ─┬─► TB6612FNG (VM) ─► Motors ×2      (~3.0 A peak)
  (~3.1 A peak total)            │
                                 └─► MP1584 ─ 5 V ─► Teensy (VIN)     (~0.10 A @ 5 V)
                                                      └─► LDO 3.3 V ─► AS5600 ×2 + MPU-6050 + encoders   (~54 mA)
```
The current in a wire = what the downstream asks for, not what the part itself uses. So the fuse + switch + top wires carry the full ~3 A motor peak, while the Teensy/sensor branch is tiny.

**Signal path** (orders, tiny current):
```
Teensy ─ PWM + direction (3.3 V) ─► TB6612FNG      (how fast the motors go)
AS5600 ×2 ─ analog OUT (0–3.3 V) ─► Teensy ADC     (whisker angle)
MPU-6050 ─ I2C (3.3 V) ──────────► Teensy          (heading)
Encoders ─ quadrature (3.3 V) ───► Teensy          (distance)
```

## Takeaway

The battery sends power (constant 7.4 V), not a signal. The only signal (PWM) comes from the Teensy. The driver marries the two, and a signal never needs the same voltage as the power it commands.

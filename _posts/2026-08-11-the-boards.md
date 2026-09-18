---
layout: post
title: The boards
date: 2026-08-11 10:00:00 +0200
tags: [electronics, hardware]
---

* TOC
{:toc}


In this post I want to explain the different ways to connect everything together, the boards, and how to make the paths.


## 1. Breadboard 🐭

Nothing to cut, nothing to solder: you push the wires in. The holes of a column are already connected, in groups of 5, and the two long rails on the sides run the whole length: <br>
●───●───●───●───●  (a column of 5)

To connect 2 columns, a jumper wire. It is for trying things out: the contacts are springs, they loosen with time and vibrations, so it stays on the bench and does not go on the robot. All the experiments were done on it, e.g. [Clean whisker signal]({% post_url 2026-07-28-clean-signal-under-pwm %}).


## 2. Stripboard

You cut instead of drawing. Before, the holes of a strip are already connected to each other:
●───●───●───●───●───●───●

After cutting between 2 holes, with a small drill bit or a cutter, it makes 2 separate groups: <br>
●───●───●  ✂  ●───●───●───●

To connect 2 different strips (perpendicular), there is no automatic connection between strips: you add a jumper wire soldered between the two points.


## 3. Perfboard 🐭

Everything is isolated, you wire it one wire at a time, each hole is independent: <br>
●   ●   ●   ●   ●   ●   ●

To connect, you solder a wire between the 2 pins. If the two holes are next to each other, you can make a drop of solder — watch out for the short circuit if there is too much of it.

For the GND, a trunk wire that you add yourself, along a chosen row of holes.

⚠️ run a whole line, lengthwise or widthwise, for the GND. Where that line goes matters, see [Signal vs power]({% post_url 2026-07-07-signal-vs-power %}).


## 4. Photosensitive board

You draw instead of cutting. The board is copper covered with a light-sensitive layer: you print the paths on a transparent sheet, expose the board to UV through it, then a chemical bath removes the copper everywhere except under the paths.

It gives a real board with your own tracks, but it takes a UV lamp, two baths and a drill for the holes. One mistake = a new board.


## 5. PCB

Same result, made by a factory. You draw the paths in a software (KiCad), send the file, and receive the boards a week later: tracks, holes, solder mask, text.

The cleanest and the most compact, but every change means a new order and a new wait. This is the goal for the v2, once the wiring stops moving.


## 6. The paths

. **Insulated hookup wire**: ok for high current, 🐭 the motor wires, 1-3 A <br>
. **Wire-wrap wire**: no high current or it melts, ok for the information/signal paths, 🐭 sensors and buttons, a few mA, fragile so it must be taped or glued so it does not move
<br>


# References
- [Wikipedia — Stripboard](https://en.wikipedia.org/wiki/Stripboard)
- [Wikipedia — Printed circuit board](https://en.wikipedia.org/wiki/Printed_circuit_board)
- [KiCad](https://www.kicad.org/), the open source software to draw a PCB



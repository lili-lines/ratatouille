---
layout: post
title: The boards
date: 2026-08-11 10:00:00 +0200
tags: [electronics, hardware]
---

* TOC
{:toc}


In this post I want to explain the different ways to connect everything together, the boards, and how to make the paths.

## 1. Stripboard

You cut instead of drawing. Before, the holes of a strip are already connected to each other:
●───●───●───●───●───●───●

After cutting between 2 holes, with a small drill bit or a cutter, it makes 2 separate groups: <br>
●───●───●  ✂  ●───●───●───●

To connect 2 different strips (perpendicular), there is no automatic connection between strips: you add a jumper wire soldered between the two points.

## 2. Perfboard

Everything is isolated, you wire it one wire at a time, each hole is independent: <br>
●   ●   ●   ●   ●   ●   ●

To connect, you solder a wire between the 2 pins. If the two holes are next to each other, you can make a drop of solder — watch out for the short circuit if there is too much of it.

For the GND, a trunk wire that you add yourself, along a chosen row of holes.

⚠️ run a whole line, lengthwise or widthwise, for the GND. Where that line goes matters, see [Signal vs power]({% post_url 2026-07-07-signal-vs-power %}).

## 3. Breadboard, photosensitive, PCB

🚧 todo

## 4. The paths

. **Insulated hookup wire**: ok for high current <br>
. **Wire-wrap wire**: no high current or it melts, ok for the information/signal paths, fragile so it must be taped or glued so it does not move

# References

- [Breadboard vs Protoboard: Which One Should You Use — JLCPCB](https://jlcpcb.com/blog/breadboard-vs-protoboard-guide)



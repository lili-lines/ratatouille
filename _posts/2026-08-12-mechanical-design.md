---
layout: post
title: Mechanical Design
date: 2026-08-12 10:00:00 +0200
tags: [mechanics, hardware]
---

* TOC
{:toc}

<figure style="margin:0;">
  <img src="{{ '/assets/img/mouse_v0.png' | relative_url }}" alt="Initial design of the mouse" style="max-width:100%; border-radius:6px;">
  <figcaption style="text-align:center; font-size:0.85rem;">Assembly design of the components, V0. In brown the laser-cut acrylic plates.</figcaption>
</figure>
<br>

This post covers how the body was made and how the components were assembled on the chassis. Above, the initial design. For a first build I use laser-cut acrylic plates as supports.

At first the chassis was planned in 3D print (see [BOM]({% post_url 2026-06-28-bom %})), but for a first assembly laser cutting is simpler and the iterations are faster. It is easier to re-drill or widen a hole by hand to adjust a position. 3D printing stays relevant for a v2, once the geometry is settled.

For **the body**, rectangular plates to validate the position of the components. This is where the size of the mouse is set: length, width, height. The wall is 50 mm high, so the whiskers must sit below that to detect it.

For **the whiskers**, a sandwich structure that keeps the whisker pivot stable and straight on both sides. The whisker material must be stiff for precision. And a mechanism must bring the whisker back to a defined neutral position.

## 1. The constraints

#### 1.1 Mechanical

The mouse must be small enough to move around the maze with some margin in the turns. And, if we get there, to run the diagonals in a straight line. So let's compute the free space in a diagonal: <br>

<div style="display:flex; flex-wrap:nowrap; gap:16px; align-items:center;" markdown="1">
<figure style="margin:0; flex-shrink:0;">
  <img src="{{ '/assets/img/diag_mesure_transp.png' | relative_url }}" alt="Free space in a diagonal" style="width:500px; border-radius:6px;">
  <figcaption style="text-align:center; font-size:0.85rem;">Maze diagram for the free space computation.</figcaption>
</figure>
<div style="flex:1; min-width:0; font-size:0.85em;" markdown="1">
**Pythagoras:**

$$\begin{aligned} \text{diag}^2 &= c^2 + c^2 \\ \text{diag} &= \sqrt{2} \times c \end{aligned}$$

Our known side, 180 mm, becomes the hypotenuse, so we look for the side perpendicular to the path:

$$\begin{aligned} c &= \frac{\text{diag}}{\sqrt{2}} = \frac{180}{\sqrt{2}} \approx 127.28\text{mm} \end{aligned}$$

The diagonal of the 12 mm post:

$$\text{diag}_{\text{post}} = \sqrt{2} \times 12 \approx 16.97\text{mm}$$

Subtract it from our side:

$$\begin{aligned} \text{Free space} &= c - \text{diag}_{\text{post}} \\ &= 127.28 - 16.97 \\ & \approx 110.31\text{mm} \end{aligned}$$
</div>
</div>
<br>

Let's keep a margin and set the width at 70-80 mm. The smaller the mouse, the more it forgives small position errors. And the more speed we can get in straight lines and diagonals, but that is not the priority for the v1.

In a straight corridor, the free width is the cell minus the post, $180 - 12 = 168$ mm. An 80 mm mouse in the middle leaves on each side:

$$\frac{168 - 80}{2} = 44\ \text{mm}$$

44 mm per side, that is the free space. This is the number that sets the test tolerances in [Flash the critter]({% post_url 2026-08-20-flash-the-critter %}).

Lengthwise, the reference point is the middle of the axle: it is the position the odometry computes, and the point the robot pivots around. 
The motors must be aligned on the same axis, or the robot does not run straight.
The whisker pivot must be perfectly stable and aligned with the AS5600 chip that reads the magnet position. The whiskers must sit at a height < 50 mm.

For the screws, the distance from the centre of the hole to the edge of the plate must be ≥ 2 × the hole diameter: <br>
M2 = 2 × Ø2.2 mm → 5 mm from the edge <br>
M3 = 2 × Ø3.2 mm → 7 mm from the edge

#### 1.2 Signal

For a clean signal, watch the noisy components. So physically separate the noise-sensitive ones, the gyro and the AS5600 sensors, from the noise-making ones, the motors, the buck and the driver.

<div style="display:flex; flex-wrap:nowrap; gap:25px; align-items:center;" markdown="1">
<img src="{{ '/assets/img/taxis_bouton.gif' | relative_url }}" alt="Control buttons" style="width:350px; flex-shrink:0; display:block; margin-top:35px; border-radius:10px;">
<div style="flex:1; min-width:0;" markdown="1">
#### 1.3 Driving: explore, retry, reset

<style>
.led-dot { display:inline-block; width:14px; height:14px; border-radius:50%; background:#3F00FF; margin:0 4px; vertical-align:middle; }
.led-slow { animation: led-blink 2s steps(1) infinite; }
.led-fast { animation: led-blink 0.4s steps(1) infinite; }
.led-triple { animation: led-blink 0.3s steps(1) 6; }
.led-fixed { animation: none; opacity: 1; }
.led-orange { background:#ff8c00; }
@keyframes led-blink { 0%, 49% { opacity: 1; } 50%, 100% { opacity: 0.15; } }
</style>

Add buttons: <br>
<span class="led-dot led-slow"></span> GO, exploration run <br>
<span class="led-dot led-fast"></span> RETRY, restart the current step <br>
<span class="led-dot led-triple led-orange"></span> RESET, erase everything <br>
<span class="led-dot led-fixed"></span> Exploration done ✅ the return starts
</div>
</div>


## 2. Chassis and whiskers

Draw the parts in a vector software = Inkscape <br>
. material = transparent acrylic, light and strong <br>
. thickness = 2 mm <br>
. SVG file = [body_v1.svg]({{ site.repo }}/mecha/body_v1.svg) <br>

<div class="two-col">
<div class="col">
<figure style="margin:0;">
  <img src="{{ '/assets/img/body_lazer_cut.jpg' | relative_url }}" alt="The acrylic plates after laser cutting" style="width:100%; display:block; border-radius:6px;">
  <figcaption style="text-align:center; font-size:0.85rem;">Result of the laser cut</figcaption>
</figure>
</div>
<div class="col">
<figure style="margin:0;">
  <img src="{{ '/assets/img/whisker_type.png' | relative_url }}" alt="The whisker materials tested: aluminium, plastic, piano wire, copper" style="width:100%; display:block; border-radius:6px;">
  <figcaption style="text-align:center; font-size:0.85rem;">The whisker materials to test</figcaption>
</figure>
</div>
</div>

For the v1 we go with the Ø0.5 mm aluminium rod. It is the best compromise among the materials tested: stiff enough to pass the contact to the pivot without bending any which way, but soft enough not to push the robot away from the wall. It cuts and shapes easily, which matters for a v1 where we adjust often.


## 3. Validation

<figure style="margin:0;">
  <img src="{{ '/assets/img/monstre_cie.gif' | relative_url }}" alt="Monsters, Inc." style="width:400px; max-width:100%; display:block; margin:0 auto; border-radius:6px;">
  <figcaption style="text-align:center; font-size:0.85rem;">Monsters, Inc., 2001</figcaption>
</figure>

Validation is iterative, like Boo with Randall Boggs in Monsters, Inc.: we change things until we are happy. 

**Criteria** <br>
. whisker height < 50 mm ✅ <br>
. the whole thing is stable ✅ <br>
. centre of gravity and weight are ok ✅ <br>
. the Teensy port and the buttons are reachable ✅<br>

**Notes** <br>
First problem, the whiskers are too high, so the AS5600 go down to level 0 and then it is ok.
Getting the holes to line up is hard, because some components are badly cut, like the AS5600 module. So plan a margin, or position by hand and use a drill. Then measure and adjust the vector drawing for a v2. Apart from the whiskers, the other criteria are met. <br>
This is the IKEA stage: the parts that end up better somewhere else, the ones we add and remove. Assemble, disassemble... plan supports for the tests, drill several holes to try different screw positions. Iterate until the result meets the criteria, then make a clean version for the v1. <br>

<figure style="margin:0;">
  <img src="{{ '/assets/img/floor_mouse.png' | relative_url }}" alt="v1 layout on the maze plan" style="max-width:100%; border-radius:6px;">
  <figcaption style="text-align:center; font-size:0.85rem;">Layout validated for the v1</figcaption>
</figure>


## 4. Thoughts

<div style="display:flex; flex-wrap:wrap; gap:16px; justify-content:center; align-items:flex-start;">
<figure style="margin:0; flex:0 1 auto;">
  <img src="{{ '/assets/img/IMG_9714.JPG' | relative_url }}" alt="" style="height:320px; max-width:100%; display:block; border-radius:6px;">
  <figcaption style="text-align:center; font-size:0.85rem;">pivot, option 1</figcaption>
</figure>
<figure style="margin:0; flex:0 1 auto;">
  <img src="{{ '/assets/img/IMG_9726.jpg' | relative_url }}" alt="" style="height:320px; max-width:100%; display:block; border-radius:6px;">
  <figcaption style="text-align:center; font-size:0.85rem;">pivot, option 2</figcaption>
</figure>
</div>

The whisker pivot should be improved, it is a bit wobbly for now, but that will be for the v2.
A rectangular shape catches the corners more easily when the heading drifts. So for a v2, try a round or octagonal shape.

# References

- [Micromouse Online — Chassis Layout](https://micromouseonline.com/micromouse-book/the-chassis/chassis-layout/)
- [Demo Micromouse Robot — Sam Dale](https://www.spdale.com/projects/demo-micromouse)

---
layout: post
title: Flash the critter
date: 2026-08-20 10:00:00 +0200
tags: [electronics, firmware, experiment]
---

<div class="learned">
<img class="learned-icon" src="{{ '/assets/img/line.png' | relative_url }}" alt="">
<div class="learned-text" markdown="1">
**What I learned** <br>
Checking the components and the circuit with a multimeter really matters, it catches errors like pins that are not connected. When working with magnetic fields, keep an eye on how close they are to each other. The noise measured on the breadboard came from the setup, not from the electronics. On the robot it is 7× lower. Setting tolerances before testing is essential.
</div>
</div>

* TOC
{:toc}

First power on. Everything is checked one step at a time, then the mouse goes on the floor.
Now check that everything is connected, that current flows, and that the information is sent and interpreted correctly. First, define the acceptable error margin to validate each test.

**The error margin** <br>
Between the wall and the mouse there is 44 mm on each side, see [Mechanical design]({% post_url 2026-08-12-mechanical-design %}). This value sets our (→ the) error margins. For the heading, the error accumulates along the path.

<div style="display:flex; flex-wrap:wrap; gap:24px; align-items:flex-start;">
<div style="flex:0 0 auto;" markdown="1">
| heading<br>error | drift per<br>cell | cells before<br>💥 the wall |
|---|---|---|
| ±5° | 15.7 mm | 2.8 |
| ±3° | 9.4 mm | 4.7 |
| ±2° | 6.3 mm | 7.0 |

Table 1: Heading error, lateral drift per cell travelled
{: .table-caption}
</div>
<div style="flex:1; min-width:240px;" markdown="1">
(1) 2 tolerances for the angle error: the gyro reading compared to the real angle, ±2°, see the measurements in [Odometry]({% post_url 2026-08-01-odometry %}). And the turn error, ±3°, an error that accumulates along the path: from table 1, after 4 cells it exceeds the 44 mm.
</div>
</div>

<div style="display:flex; flex-wrap:wrap; gap:24px; align-items:flex-start;">
<div style="flex:0 0 auto;" markdown="1">
| distance<br>error | drift per<br>cell | cells before<br> > 44 mm |
|---|---|---|
| ±1 % | 1.8 mm | 24 |
| ±2 % | 3.6 mm | 12 |
| ±3 % | 5.4 mm | 8 |
| ±4 % | 7.2 mm | 6 |

Table 2: Distance error,<br>becomes a lateral drift after a 90° turn
{: .table-caption}
</div>
<div style="flex:1; min-width:240px;" markdown="1">
(2) The distance error is proportional to the distance travelled. It costs at the turn: turn 20 mm too late and the nose can touch the wall. At ±2 % we hold (→ the mouse holds) 12 cells before touching.
</div>
</div>

(3) The error between runs ≤ 10 mm (that is 1 % over 1 m) at different speeds.

(4) Signal error: the base noise of the AS5600 signal is 70 counts peak-to-peak on the breadboard, see [Clean whisker signal]({% post_url 2026-07-28-clean-signal-under-pwm %}), but once the robot is assembled the noise drops to 10.

ℹ️ Note: a constant error is easier to correct than an unstable one.


## 1. USB test

#### 1.1 Test gyro, counters and whiskers response

<div class="two-col">
<div class="col" markdown="1">

**Goal** <br>
Observation: the idea is to check that the sensors work and are read correctly.

**Material** <br>
. the mouse without battery, USB cable

</div>
<div class="col" markdown="1">

**Protocol** <br>
The motors are stopped, `STBY LOW`. We turn the wheels by hand. (→ I turn the wheels by hand.)
Watch the sensor values in the Arduino Serial Monitor: the values are read continuously.

Upload on the Teensy: <br>
[check1_xp_system.cpp]({{ site.repo }}/algo/check1_xp_system.cpp) <br>

</div>
</div>

|---|---|---|
| Observable | Success criterion | Result |
| gyro | after calibration[^1], the value follows the movement of the mouse, plus a drift over time | ✅ |
| 2 wheel encoders | react when the wheels are turned by hand, one way (+) and the other (−) | no reaction from the right encoder; testing the signal path with the multimeter showed a missing solder joint; after fixing it the signal is ✅ |
| 3 AS5600 | the chips react to the movement of their magnet over the 0-4095 range | initially the 3 were placed side by side, 2 of them were unstable: the AS5600 chips being that close probably disturb each other's magnetic field. The middle module was moved to level 1 with the chip facing down, to keep its pivot position without changing the structure. After that the signals are ✅ |

Table 3: Serial Monitor observations
{: .table-caption}


#### 1.2 Test motors, buttons, trajectory, turn

Tolerance: <br>
. whisker at rest: ~10 counts peak-to-peak (12 bits)

**Goal** <br>
The driver is in `STBY LOW` sleep mode at startup. The buttons: GO wakes the driver up `STBY HIGH`, RESET zeroes the sensors. <br>
Check that the position and contact sensors return the right information, and that the Teensy interprets them correctly.

**Material** <br>
. mouse without battery, USB <br>
. Arduino Serial Monitor <br>
. Python <br>

#### 1.2.1 By hand, motors OFF

**Protocol** <br>
<div class="two-col">
<div class="col" style="flex:1;" markdown="1">
Straight line of 30 cm: push the robot by hand. <br>
Read the last line of `encCountR` and `encCountL` in the Serial Monitor, in raw ticks, and compute the distance. <br>
ℹ️ the distance is measured at the middle of the axle
</div>
<div class="col" style="flex:1.1;" markdown="1">
$\text{dist}_{mm} = \dfrac{encCountR + encCountL}{2} \times 0.087$

$0.087$ = mm travelled per encoder tick, see the resolution computed in [Odometry]({% post_url 2026-08-01-odometry %})
</div>
</div>

<div class="two-col">
<div class="col" style="flex:1;" markdown="1">
Combined path, 30 cm + 90°: push the robot in an L. Read the last line of `headingDeg` in the Serial Monitor. <br>

Upload on the Teensy: <br>
. [check1_xp_system.cpp]({{ site.repo }}/algo/check1_xp_system.cpp) <br>
. [check1_xp_system_log.py]({{ site.repo }}/algo/check1_xp_system_log.py) <br>

</div>
<div class="col" style="flex:1.1;" markdown="1">
```cpp
float gyroZ_dps = (gyroZraw / 131.0) - gyroZ_bias;   // rotation speed, °/s
headingDeg += gyroZ_dps * dt;                        // accumulated in degrees
```
ℹ️ the gyro measures a rotation speed in °/s, the angle in ° is computed in the script, it is `headingDeg`
</div>
</div>

**Result** <br>

| Criteria | Measure | Tolerance | Validation | 
|---|---|---|---|
| straight line 30 cm | 255 mm (−45 mm, −15 %) | ±2 %, ±6 mm | ❌ |
| heading 90° | 89.6° | ±2° | ✅ |

Table 4: Results of the hand-push experiment, motors OFF
{: .table-caption}

Data: [xp_30cm_log_1789541716.csv]({{ '/assets/data_xp/xp_30cm_log_1789541716.csv' | relative_url }}), [xp_L_log_1789542386.csv]({{ '/assets/data_xp/xp_L_log_1789542386.csv' | relative_url }})

The mouse underestimates the distance by 15 %. The cause is not identified, to investigate for the v2. The good news is that the 2 encoders measure the same distance, R 254 mm, L 256 mm. <br>

ℹ️ The peak-to-peak value of the AS5600 signal on the assembled mouse is ~10, in the end 7 times lower than on the breadboard where the noise was ~70.


#### 1.2.2 Autonomous on USB, motors ON

**Protocol** <br>
. with the motors ON, the robot must travel on its own, in a straight line, the requested distance of 30 cm <br>
. it must do a 360° <br>
. touch a whisker: the robot turns 90° towards the side of the whisker that moved, and for the front whisker the robot does a 360.


Upload on the Teensy: <br>
[> check2_xp_straight.cpp]({{ site.repo }}/algo/check2_xp_straight.cpp) <br>
[> check3_xp_turn.cpp]({{ site.repo }}/algo/check3_xp_turn.cpp) - setting `TARGET_ANGLE = 360` <br>
[> check4_xp_whisker_turn.cpp]({{ site.repo }}/algo/check4_xp_whisker_turn.cpp)


| Criteria | Measure | Tolerance | Validation | 
|---|---|---|---|
| straight line 30 cm | 340 mm | ±2 %, ±6 mm | ❌ |
| 360° turn | - | observation | ✅ |
| left AS5600, 90° reaction | - | observation | ✅ |
| right AS5600, 90° reaction | - | observation  | ✅ |
| front AS5600, 180° reaction | - | observation | ✅ |

Table 5: Results with motors ON
{: .table-caption}

<div style="display:flex; flex-wrap:wrap; gap:16px; align-items:flex-start;">
  <figure style="flex:1; min-width:260px; margin:0;">
    <div style="position:relative; width:270px; max-width:100%; margin:0 auto; aspect-ratio:9/16; border-radius:3px; overflow:hidden;">
      <iframe src="https://x.com/i/videos/tweet/2100386987957993638" style="position:absolute; inset:0; width:100%; height:100%; border:0;" allowfullscreen allow="autoplay; fullscreen; picture-in-picture"></iframe>
    </div>
    <figcaption style="text-align:center; font-size:0.85rem;">Straight line 30 cm — <a href="https://x.com/lili_lines/status/2100386987957993638">see on X</a></figcaption>
  </figure>
  <figure style="flex:1; min-width:260px; margin:0;">
    <div style="position:relative; width:270px; max-width:100%; margin:0 auto; aspect-ratio:9/16; border-radius:3px; overflow:hidden;">
      <iframe src="https://x.com/i/videos/tweet/2100395652039504211" style="position:absolute; inset:0; width:100%; height:100%; border:0;" allowfullscreen allow="autoplay; fullscreen; picture-in-picture"></iframe>
    </div>
    <figcaption style="text-align:center; font-size:0.85rem;">360° pivot — <a href="https://x.com/lili_lines/status/2100395652039504211">see on X</a></figcaption>
  </figure>
</div>

<figure style="margin:0;">
  <div style="position:relative; width:480px; max-width:100%; margin:0 auto; aspect-ratio:16/9; border-radius:3px; overflow:hidden;">
    <iframe src="https://x.com/i/videos/tweet/2100474516245823643" style="position:absolute; inset:0; width:100%; height:100%; border:0;" allowfullscreen allow="autoplay; fullscreen; picture-in-picture"></iframe>
  </div>
  <figcaption style="text-align:center; font-size:0.85rem;">Whisker sensitivity — <a href="https://x.com/lili_lines/status/2100474516245823643">see on X</a></figcaption>
</figure>


## 2. Battery test

<div class="two-col">
<div class="col" markdown="1">
**Goal** <br>
Final check of the assembly and of the sensor sensitivity on battery, before testing in the maze.
</div>
<div class="col" markdown="1">
**Material** <br>
. 🐭 complete mouse with battery
</div>
</div>

**Protocol** <br>
Mark the requested distance on the floor. For the circle, the robot must come back to its starting square.

Upload on the Teensy: <br>
[> check2_xp_straight.cpp]({{ site.repo }}/algo/check2_xp_straight.cpp) - setting `DIST_MM = 900.0`, that is 10 359 ticks for 900 mm <br>
[> check6_xp_circle.cpp]({{ site.repo }}/algo/check6_xp_circle.cpp)

| Criteria | Measure | Tolerance | Validation | 
|---|---|---|---|
| straight line 900 mm, cruise | 1035 mm | ±2 %, ±20 mm | ❌ |
| straight line 900 mm, fast | 840 mm | ±2 %, ±20 mm | ❌ |
| max gap between runs | 195 mm | ≤ 10 mm | ❌ |
| circle, cruise | 25 mm | ±20 mm | ❌ |
| circle, fast | 25 mm | ±20 mm | ❌ |
| max gap between runs | 10 mm | ≤ 10 mm | ✅ |
| follow the circle | - | observation | ✅ |

Table 6: Results on battery
{: .table-caption}

The error is not the same at each speed: the distance error changes sign when the speed goes up. A problem to solve for the v2.

<div style="display:flex; flex-wrap:wrap; gap:16px; align-items:flex-start;">
  <figure style="flex:1; min-width:260px; margin:0;">
    <div style="position:relative; width:270px; max-width:100%; margin:0 auto; aspect-ratio:9/16; border-radius:3px; overflow:hidden;">
      <iframe src="https://x.com/i/videos/tweet/2100527968938299767" style="position:absolute; inset:0; width:100%; height:100%; border:0;" allowfullscreen allow="autoplay; fullscreen; picture-in-picture"></iframe>
    </div>
    <figcaption style="text-align:center; font-size:0.85rem;">Straight line 900 mm, cruise — <a href="https://x.com/lili_lines/status/2100527968938299767">see on X</a></figcaption>
  </figure>
  <figure style="flex:1; min-width:260px; margin:0;">
    <div style="position:relative; width:270px; max-width:100%; margin:0 auto; aspect-ratio:9/16; border-radius:3px; overflow:hidden;">
      <iframe src="https://x.com/i/videos/tweet/2100528381322268845" style="position:absolute; inset:0; width:100%; height:100%; border:0;" allowfullscreen allow="autoplay; fullscreen; picture-in-picture"></iframe>
    </div>
    <figcaption style="text-align:center; font-size:0.85rem;">Straight line 900 mm, fast — <a href="https://x.com/lili_lines/status/2100528381322268845">see on X</a></figcaption>
  </figure>
</div>
<br>

<div style="display:flex; flex-wrap:wrap; gap:16px; align-items:flex-start;">
  <figure style="flex:1; min-width:260px; margin:0;">
    <div style="position:relative; width:100%; aspect-ratio:16/9; border-radius:3px; overflow:hidden;">
      <iframe src="https://x.com/i/videos/tweet/2100529941485957495" style="position:absolute; inset:0; width:100%; height:100%; border:0;" allowfullscreen allow="autoplay; fullscreen; picture-in-picture"></iframe>
    </div>
    <figcaption style="text-align:center; font-size:0.85rem;">1 circle, cruise — <a href="https://x.com/lili_lines/status/2100529941485957495">see on X</a></figcaption>
  </figure>
  <figure style="flex:1; min-width:260px; margin:0;">
    <div style="position:relative; width:100%; aspect-ratio:16/9; border-radius:3px; overflow:hidden;">
      <iframe src="https://x.com/i/videos/tweet/2100530212068880484" style="position:absolute; inset:0; width:100%; height:100%; border:0;" allowfullscreen allow="autoplay; fullscreen; picture-in-picture"></iframe>
    </div>
    <figcaption style="text-align:center; font-size:0.85rem;">1 circle, fast — <a href="https://x.com/lili_lines/status/2100530212068880484">see on X</a></figcaption>
  </figure>
</div>

<br>

That is the end of the v1. The mouse runs straight, turns correctly, and reacts to its whiskers. It still under-reads distance by 15 %, and the cause is not found yet.

This first step taught me the basics of electricity and electronics, a bit of mechanics, and which components make a small autonomous robot that explores its space. I also started 3D drawing and 3D printing. And above all a way of working, one experiment at a time, a protocol, a success criterion set before the measurement, and the result written down even when it is a failure. That is where most of the learning came from. In 10 weeks, from a blank BOM to a robot on the floor.

Now on to the v2, a stiffer whisker pivot, an oval body, a smaller battery, and the wheel encoder mystery to solve. The adventure goes on.

<br>

[^1]: [joshuaccl/Micromouse (GitHub)](https://github.com/joshuaccl/Micromouse), STM32 micromouse firmware with dedicated branches for gyro/encoder calibration and wall tracking

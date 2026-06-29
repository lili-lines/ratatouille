---
layout: page
title: Timeline
permalink: /timeline/
---

## 🐭 Timeline Micromouse : June 12 → August 22, 2026


### 🗓️ Week 1 — June 12 to 18

**Target**
- **General :** define the key concepts
- **Elec :** BOM + diagram
- **Mecha :** Design the modules : base plate of the whisker module + whisker arm + chassis + motor brackets ×2. <br>
*Lang&Tools : Fusion 360*
- **Algo :** maze-solving algo, clone simu [**mms**](https://github.com/mackorone/mms)<br>
*Lang&Tools : Python, mms*
- **Order :** order all parts (electronic + mechanical) — account for shipping lead time

**Progress files**
- post = takeaways.md
- post = bom.md
- 3D files.stl
- clone mms
- post = all parts ordered 🚧.md


### 🗓️ Week 2 — June 19 to 25

**Target**
- **Elec/Mecha :** USB + teensy + whisker module (pivot + magnet + AS5600) and read the angle <br>
*Lang&Tools : C++*
- **Algo :** full flood-fill in mms : exploration, Breadth First Search (BFS) from the center, re-flood. Plan for the separation : exploration & speed runs <br>
*Lang&Tools : C++*

**Progress files**
- post : flood-Fill (C++) + explanation.md 🚧
- xp : 1 whisker module : outputs a specific analog value corr to the bending (C++ Arduino / PlatformIO)
- angle → distance, curve plotted (py)
- post : whisker 🚧


### 🗓️ Week 3 — June 26 to July 2

**Target**
- **Elec :** test the Teensy on a bench power supply <br>
-> boot it, read the 2× AS5600 (analog), read the IMU (Inertial Measurement Unit) over I2C (Inter-Integrated Circuit), read the encoders, spin the motors
- **Test :** combine encoders + IMU (sensor fusion), and detect the whisker spring-back with a threshold on the signal slope <br>
*Lang&Tools : Python*

**Progress files**
- Teensy : all sensors + motors working, clean whisker signal under PWM (Pulse Width Modulation) (C++)
- proto : sensor fusion (position) + spring-back threshold working (whisker) (py)
- post : xp plan & result 🚧


### 🗓️ Week 4 — July 3 to 9

**Target**
- **Algo :** once the maze is mapped, build the path (shortest route, straight lines). Split the run : (1) exploration (build the map) & (2) speed run (follow the path). Whiskers = safety + re-centering in the corridor <br>
*Lang&Tools : C++*

**Progress files**
- path planning + exploration / speed-run split (C++)

### 🗓️ Week 5 — July 10 to 16

### 🗓️ Week 6 — July 17 to 23

### 🗓️ Week 7 — July 24 to 30

### 🗓️ Week 8 — July 31 to August 6

### 🗓️ Week 9 — August 7 to 13

### 🗓️ Week 10 — August 14 to 21

## Bill of Materials (BOM) — Analog-Whisker Micromouse

### 🗺️ Mind map

```
                          FRONT
   Whisker L  \       ___________      /  Whisker R
   AS5600 ──────●──[ pivot+magnet ]──●────── AS5600   (analog OUT)
                 |    3D CHASSIS     |
   Wheel L ◖━ N20+encoder ━━━━ N20+encoder ━◗ Wheel R
                 |  Teensy 4.0       |
                 |  MPU-6050 (mid)   |
                 |  LiPo 2S (bottom) |
                          REAR
                       (PTFE skid)
```

## 1. 🧠 Brain & power
- MCU: **Teensy 4.0**
- Battery: **LiPo 2S** (7.4 V)
- 5 V regulator: **MP1584** (buck)
- 3.3 V: Teensy onboard regulator
- Switch: slide switch
- Safety: fuse holder + **2 A** fuse

## 2. 🌊 Movement
- Motors ×2: **N20 6 V (~50–100:1) with integrated hall encoder**
- Driver: **TB6612FNG**
- Wheels ×2: **N20 Ø32 mm silicone**
- Rear support: **PTFE skid / ball caster**

## 3. 🧲 Analog whiskers (the core)
- Whisker sensor ×2: **AS5600 (module), analog OUT**
- Magnets ×2: **diametric Ø6 mm**
- Whisker rod: **carbon fiber Ø0.5 mm** (or piano wire)
- Pivot ×2: **MR63 bearing**
- Damped return: micro-spring + rubber
- Low-friction tip: **PTFE sleeve / micro-ball**

## 4. 🧭 Inertial sensing
- Wheel encoders: **integrated in the N20 motors**
- IMU: **MPU-6050 (GY-521)**

## 5. 🫧 Clean signal chain (motor-noise)
- Reservoir capacitor: **≥ 470 µF** electrolytic
- RC filter ×2: **R 1 kΩ + C 100 nF** per whisker
- Decoupling: **100 nF** per chip
- Star ground (wiring)

## 6. 🏠 Chassis & mechanics
- Chassis: **3D-printed PLA** (oval) + perfboard/Dupont
- Motor brackets ×2: 3D-printed
- Screws: **M2** + nylon nuts
- Whisker module ×2: 3D-printed (base plate + arm)
- Wiring: Dupont + perfboard

---

## 🛠️ Tools / software / languages

**Software**
- Fusion 360 — CAD (3D parts)
- mms — maze algorithm simulator
- Arduino / PlatformIO — firmware (Teensy)
- Python — signal analysis & telemetry (also 2D sim for control logic)

**Languages**
- C++ — firmware + mms
- Python — signal modeling / analysis

**Hardware tools**
- 3D printer
- soldering iron
- multimeter
- bench power supply
- (oscilloscope / logic analyzer — ideal)
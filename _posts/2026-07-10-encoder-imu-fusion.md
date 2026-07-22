---
layout: post
title: Where is Ratatouille ?
date: 2026-07-10 10:00:00 +0200
tags: [sensors, navigation]
---

[SCHEMA FUSION]

Fusing the **wheel encoders** with the **IMU** (Inertial Measurement Unit) gives
two things: the robot's **position** `(x, y)` and its **orientation** (heading).

## What each sensor gives

- **Encoders** (on the wheels) → **distance** each wheel travelled → position `(x, y)`.
- **IMU gyro** → the **heading** (which way the robot faces).

## Why fuse them

Each one covers the other's weakness:

- Encoders **drift when a wheel slips** (spin without moving).
- The gyro **drifts slowly over time** (its heading value creeps).

Combined, you get a **stable position + heading**, precise to the millimetre.

## TODO

- [ ] the actual fusion method (simple: gyro for heading, encoders for distance)
- [ ] test result on the bench


Fusion encodeurs (sur les roues) + IMU (gyro) = Position + orientation du robot : où il est (x, y) et dans quel sens il regarde (le cap). précis au millimètre (mm). combiner 2 capteurs pour composé les défauts l'un de l'autre

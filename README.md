# Space Invader (C++ & SFML)

A simple **Space Invader** game built using **C++ and SFML**. This project demonstrates **collision detection, paddle/ship movement, scoring, and resource handling** for a retro-style arcade game.

---

## Features

- **Player-controlled spaceship**
  - Move left/right using **A/D keys** (or arrow keys)
  - Fire missiles to destroy enemies

- **Enemies/aliens**
  - Multiple enemy sprites moving across the screen
  - Collision detection with missiles

- **Scoring system**
  - Tracks player score in real-time
  - High score saved in `HighScore.txt`

- **Assets**
  - Includes images, sound effects, and fonts
  - Smooth animations and sound effects for shooting and explosions

- **Cross-platform**
  - Runs on Linux (and other platforms with SFML)

---

## Controls

| Action            | Key             |
|------------------|----------------|
| Move Left         | A or Left Arrow|
| Move Right        | D or Right Arrow|
| Shoot Missile     | Spacebar       |
| Quit Game         | Esc            |

---

## Setup Instructions

### **1. Install SFML**
On Ubuntu/Linux:

```bash
sudo apt-get update
sudo apt-get install libsfml-dev
```
---
# Clone the Repository
```bash
git clone git@github.com:Hemeshwer0001/Space-Invader.git
cd Space-Invader
```
---
# Compile
```bash
g++ SpaceInvader.cpp -o spaceinvader -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
```
---
# Run
```bash
./spaceinvader
```
---



# Space Invader (C++ & SFML)

A simple **Space Invader** game built using **C++ and SFML**. This project demonstrates **collision detection, Space Ship movement, scoring(along with HighScore), and resource handling** for a retro-style arcade game. It utilizes **File Handling** to keep track of **HighScore**

---

## Features

- **Player-controlled spaceship and missile**
  - Move left/right using **Arrow Keys**
  - Fire missiles to destroy enemies using **Space Bar**

- **Enemies/aliens**
  - Multiple enemy sprites moving across the screen
  - Collision detection with missiles

- **Scoring system**
  - Tracks player score in **real-time**
  - High score saved in `HighScore.txt`

- **Assets**
  - Includes **images**,  **fonts** and **sound effects** for **shooting and explosions**

- **Cross-platform**
  - Runs on Linux (and other platforms with SFML)

---

## Controls

| Action            | Key             |
|------------------|----------------|
| Move Left         | Left Arrow|
| Move Right        | Right Arrow|
| Shoot Missile     | Spacebar       |

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



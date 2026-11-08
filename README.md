<h1 align="center">🤖 2-Wheel Autonomous Line Follower Robot</h1>

<p align="center">
  An Arduino Uno robot car that follows a black line on the floor completely on its own.<br>
  No remote, no external control. Built as a final lab project (A4 topic).
</p>

<p align="center">
  <img src="https://img.shields.io/badge/Arduino-Uno%20R3-00979D?logo=arduino&logoColor=white" alt="Arduino Uno R3">
  <img src="https://img.shields.io/badge/Language-C%2B%2B-00599C?logo=cplusplus&logoColor=white" alt="C++">
  <img src="https://img.shields.io/badge/Driver-L298N-orange" alt="L298N">
  <img src="https://img.shields.io/badge/License-MIT-green" alt="MIT License">
</p>

<p align="center">
  <img src="media/demo.gif" alt="Robot following a closed black-tape track" width="280">
</p>

<p align="center">
  🎬 <a href="media/demo.mp4">Full demo video</a> &nbsp;·&nbsp;
  📄 <a href="docs/Line_Follower_Robot_Report.pdf">Project report (Bangla, 10 pages)</a>
</p>

---

## 📑 Contents

- [Features](#-features)
- [Hardware](#-hardware)
- [Wiring](#-wiring)
- [How it works](#-how-it-works)
- [Getting started](#-getting-started)
- [Tuning and troubleshooting](#-tuning-and-troubleshooting)
- [Repository layout](#-repository-layout)
- [Future improvements](#-future-improvements)
- [Author](#-author)

## ✨ Features

- 🛣️ **Fully autonomous**: follows straight lines, gentle curves and sharp turns
- ⚡ **PWM speed control** for both motors through an L298N driver
- 🧠 **Line memory**: if the line is lost, the robot turns toward the side where it last saw it
- 🔋 **Stable power**: a 7.4 V battery pack feeds the motors, and an LM2596 buck converter gives a clean 5 V to the Arduino
- 🔀 **Two firmware versions**:
  - **v1**: simple rule-based logic with a 5-channel digital IR array
  - **v2**: smooth proportional (P) control with an 8-channel QTR array

## 🔧 Hardware

<p align="center">
  <img src="media/robot_top_view_1.jpg" width="340" alt="Robot top view 1">
  &nbsp;
  <img src="media/robot_top_view_2.jpg" width="340" alt="Robot top view 2">
</p>

### Components

| Component | Qty | Role |
| --- | :-: | --- |
| Arduino Uno R3 | 1 | Main controller |
| IR line sensor array (5-ch for v1, 8-ch QTR for v2) | 1 | Detects black line vs. white floor |
| L298N motor driver | 1 | Motor direction + PWM speed control |
| N20 1000 RPM gear motor + wheel | 2 | Locomotion |
| 18650 Li-ion cell (3.7 V), in series = 7.4 V | 2 | Power source |
| LM2596 buck converter | 1 | 7.4 V → 5 V for the Arduino |
| ON/OFF switch | 1 | Main power switch |
| Brackets, jumper wires, tape | – | Mounting and wiring |

### Block diagram

```
 2×18650 (7.4 V) ──► Switch ─┬──► L298N ──────────► 2× N20 motors
                             │      ▲
                             │      │ direction + PWM
                             └──► LM2596 ──5 V──► Arduino Uno ◄── IR sensor array
```

## 🔌 Wiring

### v1: 5-channel digital IR array → [`line_follower_5ir.ino`](firmware/line_follower_5ir/line_follower_5ir.ino)

| Signal | Arduino pin |
| --- | :-: |
| Sensor 1 (far left) | D2 |
| Sensor 2 (near left) | D3 |
| Sensor 3 (center) | D4 |
| Sensor 4 (near right) | D5 |
| Sensor 5 (far right) | D6 |
| L298N ENA (left speed, PWM) | D9 |
| L298N IN1 / IN2 (left direction) | D7 / D8 |
| L298N IN3 / IN4 (right direction) | D11 / D12 |
| L298N ENB (right speed, PWM) | D10 |

### v2: 8-channel QTR (RC) array → [`line_follower_qtr8.ino`](firmware/line_follower_qtr8/line_follower_qtr8.ino)

| Signal | Arduino pin |
| --- | :-: |
| QTR sensors 1–8 (left → right) | A0, A1, A2, A3, A4, A5, D2, D3 |
| L298N ENA / ENB (PWM) | D5 / D6 |
| L298N IN1 / IN2 / IN3 / IN4 | D7 / D8 / D9 / D10 |

### Power

- Battery 1 (+) → switch → L298N **12 V** terminal **and** LM2596 **IN+**
- Battery 1 (−) → Battery 2 (+) (series = 7.4 V); Battery 2 (−) → common GND
- LM2596 **OUT** → Arduino **5 V** / **GND**

> [!IMPORTANT]
> - Set the LM2596 output to **5 V with a multimeter before** connecting the Arduino.
> - The Arduino, sensors and L298N **must share a common GND**.
> - Remove the **ENA / ENB jumpers** on the L298N, otherwise PWM speed control will not work.

## 🧠 How it works

### v1: rule-based (5 digital sensors)

Every loop the robot reads all 5 sensors and picks one rule:

| Situation | Left wheel | Right wheel |
| --- | :-: | :-: |
| Center sensor on line | 70 | 70 |
| Near-left sensor on line (gentle left) | 25 | 70 |
| Far-left sensor on line (sharp left) | 0 | 85 |
| Near-right sensor on line (gentle right) | 70 | 25 |
| Far-right sensor on line (sharp right) | 85 | 0 |
| Line lost | turn toward the last seen side, or stop if there is no memory | |

### v2: proportional control (8 QTR sensors)

`readLineBlack()` returns the line position from **0** (far left) to **7000** (far right), with **3500** meaning centered. The distance from the center is the error. It is multiplied by `Kp` and used to speed up one wheel and slow the other, so the robot steers smoothly instead of jumping between fixed rules.

```
error      = position - 3500
correction = Kp * error
left       = baseSpeed + correction
right      = baseSpeed - correction
```

If the line is lost, `readLineBlack()` keeps reporting the edge where it was last seen, so the robot naturally turns back toward it.

## 🚀 Getting started

1. **Build the circuit** using the wiring tables above.
2. **Open the sketch** for your version in the Arduino IDE:
   - v1 → `firmware/line_follower_5ir/line_follower_5ir.ino`
   - v2 → `firmware/line_follower_qtr8/line_follower_qtr8.ino`
     (needs the **QTRSensors** library by Pololu: *Sketch → Include Library → Manage Libraries*)
3. **Upload**: select **Arduino Uno** and the correct port, then click Upload.
4. **Run** on a black-tape track (a white floor works best):
   - **v1** waits **3 s** after power-on so you can place it on the line.
   - **v2** calibrates first (about 10 s, on-board LED is **on**). During this time, slide the robot by hand left and right across the line so every sensor sees both black and white. When the LED turns off, it starts driving.

## 🎛️ Tuning and troubleshooting

| Problem | Fix |
| --- | --- |
| Robot wobbles / zig-zags | Lower `Kp` (v2) or lower the speeds |
| Robot leaves the track on curves | Raise `Kp`, or lower `baseSpeed` / `BASE_SPEED` |
| Robot runs away from the line | Sensor output is inverted: swap `HIGH`/`LOW` (v1) or use `readLineWhite()` (v2) |
| One wheel spins backwards | Swap that motor's two wires on the L298N |
| Motors don't change speed | ENA / ENB jumpers are still on the L298N, so remove them |
| Arduino resets when motors start | Check the common GND and the battery charge |

## 📁 Repository layout

```
.
├── firmware/
│   ├── line_follower_5ir/
│   │   └── line_follower_5ir.ino    # v1: 5 digital IR sensors, rule-based
│   └── line_follower_qtr8/
│       └── line_follower_qtr8.ino   # v2: 8-sensor QTR, proportional control
├── docs/
│   └── Line_Follower_Robot_Report.pdf
├── media/
│   ├── demo.gif / demo.mp4
│   └── robot_top_view_1.jpg / robot_top_view_2.jpg
├── LICENSE
└── README.md
```

## 🔮 Future improvements

- [ ] Full PID control (add `Ki`, `Kd`) for higher speed
- [ ] Wheel encoders for speed feedback
- [ ] Intersection / junction handling
- [ ] Custom PCB instead of jumper wires

## 👤 Author

**Mahamud Hassan** · [@mahamud894](https://github.com/mahamud894)

## 📄 License

This project is licensed under the [MIT License](LICENSE).

---

### 🇧🇩 সংক্ষেপে (বাংলায়)

এটি আর্ডুইনো উনো দিয়ে তৈরি একটি স্বয়ংক্রিয় লাইন ফলোয়ার রোবট কার। IR সেন্সর কালো লাইন শনাক্ত করে, আর আর্ডুইনো L298N মোটর ড্রাইভারের মাধ্যমে দুটি N20 মোটরের গতি PWM দিয়ে নিয়ন্ত্রণ করে। লাইন হারিয়ে গেলে শেষবার যেদিকে লাইন দেখা গেছে, রোবট সেদিকে ঘুরে লাইন খুঁজে নেয়। বিস্তারিত ব্যাখ্যা [`docs/`](docs/Line_Follower_Robot_Report.pdf) ফোল্ডারের রিপোর্টে আছে।

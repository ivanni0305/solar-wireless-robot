# Wireless Robot with Autonomous Collision Avoidance and Solar Charging

A self-directed hardware and firmware project built to demonstrate hands-on skills in power systems, embedded programming, and wireless communication. The robot is remotely controlled via a custom joystick controller and automatically stops when it detects an obstacle in its path.

## Demo

https://github.com/user-attachments/assets/82d3c578-20b4-4a7f-a8ce-f67f73e317a3

---

## Features

- **Solar rechargeable** — 6V/3W solar panel charges the 2S Li-ion battery pack via a CC/CV boost charging module when the robot is parked
- **Wireless control** — ESP-NOW protocol between two ESP32 microcontrollers for low-latency, WiFi-independent communication
- **Autonomous collision avoidance** — HC-SR04 ultrasonic sensor overrides forward movement when an obstacle is detected within 20cm
- **Dual power rails** — motor power and logic power are separated to prevent electrical noise from affecting the ESP32
- **Custom joystick controller** — handheld unit with its own ESP32, joystick module, and 18650 battery

---

## Hardware

### Robot
| Component | Purpose |
|---|---|
| ESP32 (ESP-WROOM-32) | Main microcontroller, runs wireless receiver and motor control |
| TB6612FNG Motor Driver | Controls both DC gear motors — chosen over L298N for lower heat and higher efficiency |
| LM2596 Buck Converter | Steps 7.4V battery down to 5V for ESP32 and logic |
| 2S Li-ion Battery Pack (7.4V) | Main power source |
| Multi-cell 2S CC/CV Charger Module | Charges battery from solar panel at correct 8.4V cutoff |
| 6V 3W Solar Panel (145×145mm) | External solar charger — connects to CC/CV module to recharge battery |
| HC-SR04 Ultrasonic Sensor | Measures distance for collision avoidance |
| 2WD Acrylic Chassis | Robot platform with 2 DC gear motors and caster wheel |

### Controller
| Component | Purpose |
|---|---|
| ESP32 (ESP-WROOM-32) | Reads joystick and transmits commands via ESP-NOW |
| Joystick Module | Analog X/Y input for directional control |
| 18650 Li-ion Cell | Powers the controller |
| TP4056 Charging Module | Charges 18650 via USB-C |

---

## Power System Architecture

```
Solar Panel (6V)
      ↓
CC/CV 2S Charger Module
      ↓
2S Li-ion Battery Pack (7.4V nominal, 8.4V full)
      ├── TB6612 VM pin (raw motor power)
      └── LM2596 Buck Converter → 5V
                                    ├── ESP32 (logic)
                                    └── HC-SR04 (sensor)
```

The solar panel is used to recharge the battery when the robot is not in use — the CC/CV charger module boosts the panel's 6V output to the correct 8.4V charging voltage for the 2S pack.

Key design decisions:
- **Buck converter over linear regulator** — significantly more efficient, less heat, better for battery life
- **Separated motor and logic power rails** — prevents voltage spikes from motors corrupting ESP32 operation
- **CC/CV charging** — correct constant-current/constant-voltage profile for safe Li-ion charging
- **TB6612FNG over L298N** — lower voltage drop, no heatsink needed, better PWM response

---

## Wireless Communication

Both ESP32s use **ESP-NOW**, a Espressif proprietary protocol that allows direct peer-to-peer communication without a WiFi router. This gives:
- Sub-10ms latency
- No network infrastructure needed
- Simple pairing via MAC address

The controller sends joystick X/Y values every 20ms. The robot has a 300ms timeout — if no packet is received, motors stop automatically.

---

## Collision Avoidance Logic

```
Joystick sends forward command
         ↓
Robot ESP32 reads HC-SR04 distance
         ↓
Distance < 20cm?
   YES → Stop motors (override)
   NO  → Execute forward command
```

Backward, left, and right commands are not overridden — only forward movement is blocked to prevent driving into obstacles.

---

## Wiring

### Robot ESP32 Pin Connections

| Pin | Connection |
|---|---|
| 5V | Buck converter OUT+ |
| GND | Buck converter OUT− |
| P25 (GPIO25) | TB6612 PWMA |
| P26 (GPIO26) | TB6612 AIN2 |
| P27 (GPIO27) | TB6612 AIN1 |
| P13 (GPIO13) | TB6612 STBY |
| P32 (GPIO32) | TB6612 BIN2 |
| P33 (GPIO33) | TB6612 BIN1 |
| P14 (GPIO14) | TB6612 PWMB |
| 3V3 | TB6612 VCC |
| P5 (GPIO5) | HC-SR04 TRIG |
| P18 (GPIO18) | HC-SR04 ECHO |

### Controller ESP32 Pin Connections

| Pin | Connection |
|---|---|
| 3V3 | Joystick VCC |
| GND | Joystick GND |
| P34 (GPIO34) | Joystick VRX |
| P35 (GPIO35) | Joystick VRY |
| P32 (GPIO32) | Joystick SW |

---

## Software

- **Language:** C++ (Arduino framework)
- **Wireless:** ESP-NOW (no WiFi router required)
- **Board package:** Espressif ESP32 v2.0.11
- **Key libraries:** `esp_now.h`, `WiFi.h`

See `robot_code/robot_code.ino` and `controller_code/controller_code.ino` for full source.

---

## Build Notes

- All wiring uses 22 AWG silicone wire
- JST-PH connector used for battery connection
- Jumper wires used for ESP32 ↔ TB6612 and ESP32 ↔ HC-SR04 connections
- HC-SR04 mounted at front of chassis on included bracket
- ESP32 is powered via a USB power bank during operation — this avoids a voltage conflict between the buck converter 5V rail and the ESP32's onboard USB 5V rail, which caused instability when both were connected simultaneously

---

## Author

Ivan Ni — Electrical and Computer Engineering, NYU Tandon School of Engineering  
[LinkedIn](https://www.linkedin.com/in/ivan-ni-4a5bb3293/) | [GitHub](https://github.com/ivanni0305)

# Bio-Inspired Robotic Flower (Heliotropism Simulator)

A modular robotic system built on the **Arduino** platform that emulates the biological behavior of plants—specifically **heliotropism** (orienting towards light) and interactive "blooming" through proximity sensing.

## Overview
This project serves as a practical demonstration of bio-inspired robotics. It integrates sensorics, actuation, and embedded processing to recreate the evolutionary adaptations of plants that optimize solar energy capture.

## Key Features
* **Automatic Light Tracking:** Implements phototropism using optical sensors to rotate the base toward light sources.
* **User Interaction:** Detects proximity to trigger a "blooming" response (opening petals).
* **Coordinated Actuation:** Synchronized control of multiple servo motors to simulate natural movements.
* **Modular Hardware:** An extensible architecture based on the Arduino platform.

---

## Hardware Architecture

### Components:
* **Microcontroller:** Arduino (Uno/Nano)
* **Sensors:** * 1x **HC-SR04** (Ultrasonic sensor for proximity)
    * 2x **LDRs** (Light Dependent Resistors for light tracking)
* **Actuators:**
    * 4x **180° Servos** (Petal movement)
    * 1x **360° Servo** (Base rotation)



---

## How It Works

### 1. Petal Control (Proximity Mode)
Utilizing the **HC-SR04** ultrasonic sensor, the system measures the distance to nearby objects. 
* **Action:** When an object (e.g., a hand) enters the predefined range, the microcontroller commands the four 180° servos to open the petals.
* **Reset:** When the object is removed, the servos reverse the movement, closing the flower.

### 2. Base Orientation (Phototropism Mode)
The system uses two LDRs to balance light intensity. 
* **Logic:** The algorithm calculates the difference between the two light sensors. If a significant imbalance is detected (e.g., a flashlight shining on one side), the **360° servo** at the base rotates the entire structure until the light levels are equalized.



---

## Technical Challenges Solved
* **Signal Processing:** Filtering and interpreting sensor data to avoid erratic movements or oscillations.
* **Mechanical Coordination:** Managing multiple actuators simultaneously for fluid, lifelike motion.
* **Bio-Mimicry:** Translating biological heliotropism into a functional algorithmic control loop.

---

## Project Structure
* `/src` - Arduino (.ino) source code.
* `/schematics` - Circuit diagrams and wiring layouts.
* `/docs` - Detailed technical documentation.

---

### Future Improvements
* Integration of a moisture sensor for "thirst" simulations.
* Implementation of a sleep mode (closing petals at night).
* Battery optimization for standalone operation.

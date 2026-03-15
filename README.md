# 01_SchrittMot

![AVR](https://img.shields.io/badge/Platform-AVR-blue)
![MCU](https://img.shields.io/badge/MCU-ATmega2560-green)
![Language](https://img.shields.io/badge/Language-C-orange)
![Build](https://img.shields.io/badge/Build-Microchip%20Studio-red)
![Status](https://img.shields.io/badge/Status-Stable-success)

A simple stepper motor controller firmware for the ATmega2560.

The program allows runtime control of:

* Motor direction
* Motor speed

The implementation uses a finite state machine (FSM) to keep the control logic simple, predictable, and easy to extend.

The firmware was developed and tested using a: 
* PICado ATmega2560 development board
* Breakout board
* Stepper motor driver module.

---

# Table of Contents

* [Overview](#overview)
* [Requirements](#requirements)
* [Project Setup](#project-setup)
* [Building and Flashing](#building-and-flashing)
* [System Architecture](#system-architecture)
* [Hardware Setup](#hardware-setup)
* [Stepper Motor Operation](#stepper-motor-operation)
* [Design Decisions](#design-decisions)
* [Testing](#testing)
* [Sources](#sources)

---

# Overview

This project is a minimal implementation of stepper motor control firmware.

Features include:

* Clockwise rotation
* Anticlockwise rotation
* Adjustable motor speed
* Deterministic control using an FSM

The project focuses on clarity and maintainability, making it suitable for educational use or small embedded projects.

---

# Requirements

## Development Tools

* Microchip Studio

  * AVR toolchain
  * XC8 compiler support

### Documentation Tools (Optional)

For editing diagrams:

* VS Code
* Draw.io plugin
* C and C++ plugin

## Hardware

* ATmega2560 development board
* Stepper motor with driver module

---

# Project Setup

Before building the project, a small configuration step is required.

## 1. Install AVRdude

Download the AVRdude tool from:

[https://github.com/ml3rc/01_SchrittMot/tree/main/readme/AVRdude](https://github.com/ml3rc/01_SchrittMot/tree/main/readme/AVRdude)

Place it in:

```
C:\Program Files (x86)\Atmel\Studio\7.0\tools\
```

Adjust the folder if your Microchip Studio version differs.

---

## 2. Configure the Programming Tool

Open the project properties in Microchip Studio:

```
Project → 01_SchrittMot Properties
```

Navigate to:

```
Tool → Custom programming tool → Command
```

Update the following things in the command:

* Repository path
* COM port of the development board

---

# Building and Flashing

Once the setup is complete, the firmware can be built and flashed directly from Microchip Studio.

Use:

```
Start Without Debugging (Green start sign with a hole in the middle)
```

This will:

1. Compile the firmware
2. Upload it to the ATmega2560
3. Start execution immediately

---

# System Architecture

The control logic is implemented using a finite state machine (FSM).

Advantages of this approach:

* Clear system behavior
* Predictable execution flow
* Easy to extend with new states

For example, adding a new state such as `IDLE` would require only minimal changes.

## State Event Diagram

![State Event Diagram](./readme/State-Event.svg)

---

# Hardware Setup

The firmware was tested using the following hardware:

* PICado ATmega2560 development board
* Breakout board
* Stepper motor with driver module

## Hardware Connection Diagram

![Hardware Connection](./readme/HW-Connection.svg)

The development board provides 5 V supply voltage, and the driver circuitry required for the motor is integrated on the board.

---

# Stepper Motor Operation

The stepper motor used in this project follows the schematic below.

![Stepper Motor](https://plc247.com/wp-content/uploads/2020/07/variable-reluctance-stepper.jpg)

## Operating Parameters

| Parameter         | Value         |
| ----------------- | ------------- |
| Default step rate | 20 steps/sec  |
| Maximum step rate | 300 steps/sec |
| Supply voltage    | 5 V           |

---

## Task Step Sequence

The original task specification defined the following stepping pattern:

```c
const uint8_t STEPS[] = {
    0b00001001,
    0b00000011,
    0b00000110,
    0b00001100
};
```

This sequence uses only half of the available motor positions.

---

## Implemented Step Sequence

To improve motion smoothness and positioning accuracy, the following half-step sequence was implemented:

```c
const uint8_t STEPS[] = {
    0b00001001,
    0b00000001,
    0b00000011,
    0b00000010,
    0b00000110,
    0b00000100,
    0b00001100,
    0b00001000
};
```

This allows intermediate states where two coils are active, resulting in:

* smoother motion
* finer positioning
* better mechanical behavior

---

## Stepper Motor Visualization

The animation below illustrates the internal working principle of a stepper motor.

Note that intermediate states where two coils are active are not shown.

![Stepper Motor Animation](https://electricdiylab.com/wp-content/uploads/2019/12/STEPPER-2.gif)

---

# Design Decisions

## Finite State Machine

An FSM was chosen because it provides:

* clear separation of states
* predictable system behavior
* simple expansion for future features

For example, adding an `IDLE` or `PAUSE` state would require only minimal code changes.

---

## No Interrupts

Interrupts were intentionally not used.

The control logic is simple and deterministic, so polling inside the main loop keeps the implementation easier to understand.

---

## Adjustable Speed Control

Speed adjustment was added because:

* it requires very little additional code
* it greatly improves usability and control of the motor

---

# Testing

All functionality was tested on the hardware setup described above.

| Step |              Action             |             Expected Result             |                  Result                 | Pass/Fail |
| :--: | :-----------------------------: | :-------------------------------------: | :-------------------------------------: | :-------: |
|   0  |             Power on            | Motor rotates clockwise at 20 steps/sec | Motor rotates clockwise at 20 steps/sec |    Pass   |
|   1  | Press B_DIRECTION_ANTICLOCKWISE |       Motor rotates anticlockwise       |       Motor rotates anticlockwise       |    Pass   |
|   2  |   Press B_DIRECTION_CLOCKWISE   |         Motor rotates clockwise         |         Motor rotates clockwise         |    Pass   |
|   3  |         Press B_SPEEDUP         |          Motor speed increases          |          Motor speed increases          |    Pass   |
|   4  |     Press B_SPEEDDOWN twice     |          Motor speed decreases          |          Motor speed decreases          |    Pass   |

---

# Sources

All images used in this document are referenced via their original URLs inside the README.

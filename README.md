# 01_SchrittMot

Simple Stepmotor controller with speed and direction change.

## Requirements

- Microchip Studio with support for AVR and XC8
- ATmega 2560 with 
controller for Stepmotor

## Initial Setup

1. Put the [AVRdude](https://github.com/ml3rc/01_SchrittMot) tool into `C:\Program Files (x86)\Atmel\Studio\7.0\tools\` (correct the version number)

2. Correct the Paths and COM-port in `Microchip Studio > Project > 01_SchrittMot Properties... > Tool > Custom programming tool > Command` to match your repo location.

## Building

After the Initial Setup you can build the project with `Start without debugging` (green start sign with a hole in the middle)

## State Event

![Diagram-State-Event.drawio.svg](readme\State-Event.drawio.svg)

## Connection Diagramm

The following connection diagramm is for a PICado ATmega 2560 board with a corresponding break-out-board and a stepper motor module containing a driver.

![Diagram-HW-Connection.drawio.svg](readme\HW-Connection.drawio.svg)


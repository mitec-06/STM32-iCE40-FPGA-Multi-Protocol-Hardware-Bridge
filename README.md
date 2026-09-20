# STM32/FPGA Embedded Projects

## Overview

These series of projects is a digital control system bridging an STM32 (ARM Cortex-M4) host controller with an Lattice iCE40UP5K FPGA (iCESugar). The system processes incoming incoming commands over various serial peripherals (UART/I2C/SPI) and drives hardware peripherals.

### Project Milestones

## MILESTONE 1.2 

The primary goal of this milestone was to create an interface between the STM32 and the FPGA, such that any input on the STM32 would be noticed on the FPGA. An implementation of this involves the STM32 toggling a GPIO pin, and the FPGA seeing this toggling, and lighting up an LED as a result.

## MILESTONE 1.3 

Building upon the previous milestone, Milestone 1.3 essentially helped map various commands from the STM32, with the FPGA mapping those translations to exhibit behavior. Basically, the STM32 would send opcodes to the FPGA, and the FPGA would would decode this opcode and light a specific color (red, green, cyan, etc).

## MILESTONE 1.4

This milestone builds upon the previous by having the user type in commands on an serial console called PuTTY, which via UART would send the command to the STM32, and via the work of the previous milestone, send that sequence to the FPGA which would decode the sequence and light a specific color.


More Milestones + Explanations soon...
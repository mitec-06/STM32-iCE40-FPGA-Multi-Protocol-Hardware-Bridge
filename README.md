# STM32/FPGA Embedded Projects

## Overview

These series of projects is a digital control system bridging an STM32 (ARM Cortex-M4) host controller with an Lattice iCE40UP5K FPGA (iCESugar). The system processes incoming incoming commands over various serial peripherals (UART/I2C/SPI) and drives hardware peripherals.

### Project Milestones

## Milestone 1.2 

The primary goal of this milestone was to create an interface between the STM32 and the FPGA, such that any input on the STM32 would be noticed on the FPGA. An implementation of this involves the STM32 toggling a GPIO pin, and the FPGA seeing this toggling, and lighting up an LED as a result.

## Milestone 1.3 

Building upon the previous milestone, Milestone 1.3 essentially helped map various commands from the STM32, with the FPGA mapping those translations to exhibit behavior. Basically, the STM32 would send opcodes to the FPGA, and the FPGA would would decode this opcode and light a specific color (red, green, cyan, etc).

## Milestone 1.4

This milestone builds upon the previous by having the user type in commands on an serial console called PuTTY, which via UART would send the command to the STM32, and via the work of the previous milestone, send that sequence to the FPGA which would decode the sequence and light a specific color.

## Milestone 2.1

This milestone covers the basics of I2C communication, and ensures that using a Waveforms AD3, the behavior of how an I2C protocol works is fully captured, including the START condition, differentiating between ACK/NACK, along with the device address being sent correctly.

## Milestone 2.2

This milestone builds upon the previous by being able to read a byte from the device using I2C, along with using a Logic Analyzer from the AD3 to be able to properly decode the information coming from the device.

## Milestone 2.3 

This milestone also builds upon the previous by being able to write registers to the device, along with being able to read those written registers from the device, ultimately still using a Logic Analyzer to fully decode the I2C data stream. 

## Milestone 3

This milestone implements an FPGA connected to an STM32. The FPGA essentially holds the SDA line low, ruining the I2C transmit, and causing a NACK. After the connection to the FGPA is broken, the STM32 runs a function in order to release the broken bus, ultimately restarting the I2C transmission by sending a START condition. It resets this broken bus by disconnecting the SCLK and SDA pins, and manually running 9 clock pulses to clear the broken bus, reconnecting the SDA and SCLK pins via software, and then sending a START condition, with the ultimate goal of receiving a successful transmission (ACK).

## Milestone 4.1

The goal of this milestone is to show that SPI transmission on the STM32 works, ensuring that the correct bits are sent via MOSI.

## Milestone 4.2

This milestone builds upon the previous by having an FPGA detect the rising SCLK edges, and turning on its LED whenever SCLK shows a rising edge. 

## Milestone 4.3

Using a shift register, allowing the MOSI stream to eventually populate the shift register, the FGPA is able to also differentiate between different addresses, flashing different colors depending on the address sent.

## Milestone 4.4

This milestone essentially builds upon the previous to fully implement op-codes. In this milestone, the FPGA identifies two addresses it can be sent, the first being address 0x01, which controls LED color, and address 0x02 which controls LED Pulsing Frequency.

In addition, using a verilog counter, the FPGA counts how many bits are sent, and depending on whether the user wants to read the information or write it, if the user wants to read the data currently on the register, the output gets sent via UART on PuTTY, while if the user wants to write the data, it simply gets written.

Because at most two addresses can be sent via MOSI, if the user wants to write data, the first 8 bits are recorded with the read and write bit being saved along with the rest of the address, with the next 8 bits being recorded as the new address to be written.

If the user chooses to read the information, only the first 8 bits are processed via MOSI, afterwhich the address the user wants to read is sent back to the STM32 via MISO on the negedge of SCLK.

## Milestone 5.1

This milestone tests the interrupt feature on the STM32, printing an error message on PuTTY using UART if a pin driven high at 3.3V is connected to GND.


More Milestones + Explanations + Pictures soon...
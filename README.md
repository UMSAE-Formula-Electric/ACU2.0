# Auxiliary Control Unit (ACU)

## Introduction
This repository contains the code for the Auxiliary Control Unit (ACU) of the Formula Electric SAE Student Team UMSAE Electric.
The ACU is responsible for the control of the auxiliary systems of the vehicle, such as battery protection, BMS connectivity, 
cooling system, safety loop, and lights (TSAL, brakes, fault lights). 
The ACU is also responsible for the communication with the other ECUs in the vehicle, such as the Vehicle Control Unit (VCU) and the Sensor Control Unit (SCU).
This is achieved through the use of the CAN bus, with the ACU able to both receive and transmit messages.
The ACU uses the FreeRTOS operating system for task scheduling and management. The software is structured into tasks, each responsible for a specific function such as handling CAN bus messages or controlling a specific auxiliary system.

## Cloning the repository
### Option 1: Using `git clone --recurse-submodules` (for newer Git versions)
```bash
git clone --recurse-submodules git@github.com:UMSAE-Formula-Electric/ACU2.0.git
```

### Option 2: Using `git clone` and `git submodule update --init --recursive` (for older Git versions)
```bash
git clone git@github.com:UMSAE-Formula-Electric/ACU2.0.git
cd ACU2.0
git submodule update --init --recursive
```

### Option 3: If you have already cloned the repository without submodules
```bash
cd ACU2.0
git submodule update --init --recursive
```

## Building the code
The code is built using STM32CubeIDE.

### Prerequisites
- [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html) 
- [STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html)(optional) 

# IoT Smart Battery Charger

## Overview
This project presents the design and implementation of an IoT-enabled smart battery charging system using ESP32 microcontroller technology. The system was developed to provide safe, intelligent, and efficient charging of 12V lead-acid batteries in unstable power environments.

The charger integrates real-time voltage and current sensing, automatic charging cut-off, LCD display monitoring, and cloud-based remote monitoring through the Blynk IoT platform.

---

## Features
- ESP32-based smart control
- Automatic battery full detection
- Relay-controlled charging cut-off
- Real-time current and voltage monitoring
- LCD display interface
- Blynk IoT remote monitoring
- PWM-based charging control
- Buzzer alarm system
- Wi-Fi communication

---

## Technologies Used
- ESP32
- Arduino IDE
- Blynk IoT Platform
- LiquidCrystal_I2C
- ACS712 Current Sensor
- Voltage Divider Circuit
- PWM Control
- Buck Converter

---

## System Components
- ESP32 Microcontroller
- ACS712 Current Sensor
- Voltage Sensor
- Relay Module
- LCD I2C Display
- Buck Converter
- Buzzer
- 12V Lead-Acid Battery

---

## System Architecture
The system uses ESP32 as the main controller for monitoring battery voltage and charging current. Sensor data is processed and transmitted to the Blynk IoT platform via Wi-Fi for remote monitoring.

The relay module disconnects charging automatically when the battery reaches full charge voltage (14.4V).

---

## Results
The implemented system successfully:
- monitored battery voltage and current in real time
- performed automatic charging cut-off
- displayed battery percentage
- transmitted live data through Blynk IoT
- improved charging safety and efficiency

---

## Applications
- Smart battery management systems
- Solar energy storage systems
- Backup power systems
- IoT energy monitoring
- Embedded energy systems

---

## Future Improvements
- Lithium-ion battery support
- Solar charging integration
- Cloud database logging
- AI-based battery health prediction
- Mobile application enhancement

---

## Author
Angel Ibe  
Department of Electrical Engineering  
University of Nigeria, Nsukka

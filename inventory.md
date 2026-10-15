# Inventory

All components available for projects.

## Boards and power

| Component | Model / part no. | Qty | Notes |
|-----------|------------------|-----|-------|
| Controller board | Arduino Uno R3 (compatible) | 1 | |
| USB cable | USB-A to USB-B | 1 | Power and upload |
| 9V battery connector | 9V clip to DC barrel | 1 | Powers the board without USB |

## Displays

| Component | Model / part no. | Qty | Notes |
|-----------|------------------|-----|-------|
| LCD 16x2 | 1602 with I2C backpack | 1 | I2C |
| OLED 2.42" 128x64 | SSD1309, green | 1 | I2C |
| 7-segment display, 1 digit | | 1 | |
| 7-segment display, 4 digits | | 1 | Needs multiplexing |
| LED matrix 8x8 | | 1 | No driver chip; use with 74HC595 |

## Sensors

| Component | Model / part no. | Qty | Notes |
|-----------|------------------|-----|-------|
| Temperature and humidity module | DHT11 | 1 | Digital, 1-wire |
| Temperature sensor | LM35DZ | 1 | Analog, 10 mV/°C |
| Photoresistor | | 3 | Analog, needs 10K divider |
| Sound sensor module | | 1 | Analog + digital out |
| Water level sensor module | | 1 | Analog |
| Flame sensor | IR diode | 1 | |
| Tilt ball switch | | 2 | Digital |
| IR receiver | | 1 | Use with IR remote |
| RFID module | RC522 | 1 | SPI, **3.3V only** |
| Real-time clock module | DS1302 | 1 | 3-wire serial |

## Input

| Component | Model / part no. | Qty | Notes |
|-----------|------------------|-----|-------|
| Push button | | 5 | |
| Potentiometer | 10K | 1 | Analog |
| Joystick module | | 1 | 2 analog axes + button |
| Keypad 4x4 | Matrix membrane | 1 | 8 digital pins |
| IR remote | | 1 | |

## Output and actuators

| Component | Model / part no. | Qty | Notes |
|-----------|------------------|-----|-------|
| LED, red | 5 mm | 5 | Needs 220Ω resistor |
| LED, yellow | 5 mm | 5 | Needs 220Ω resistor |
| LED, green | 5 mm | 5 | Needs 220Ω resistor |
| RGB LED module | | 1 | 3 PWM pins |
| Active buzzer | | 1 | Fixed tone, digital on/off |
| Passive buzzer | | 1 | Any tone, use `tone()` |
| Servo motor | SG90 | 1 | PWM |
| Stepper motor | 28BYJ-48 | 1 | Use with ULN2003 driver |
| Stepper motor driver | ULN2003 | 1 | |
| Relay module | 5V, 1 channel | 1 | |

## ICs

| Component | Model / part no. | Qty | Notes |
|-----------|------------------|-----|-------|
| Shift register | 74HC595 | 1 | 3 pins → 8 outputs |

## Passive components

| Component | Model / part no. | Qty | Notes |
|-----------|------------------|-----|-------|
| Resistors | 220Ω / 1K / 10K | 30 | Total of all values |

## Prototyping

| Component | Model / part no. | Qty | Notes |
|-----------|------------------|-----|-------|
| Breadboard | 830 tie points | 1 | |
| Jumper wire, male-male | | 65 | |
| Dupont wire, female-male | | 20 | |

# Beer pour tracker

Prototype that tracks how much beer is poured, and how much CO2 is in it. The LCD shows the totals. A button starts and stops a simulated pour.

## How it works

- Press the button to start pouring. Press again to stop.
- Flow rate: 0.059 L/s (2 oz/s, a common tap design rate).
- CO2: 1.6 g per 0.33 L of lager (≈ 4.85 g/L).

## Hardware

- Board: Arduino Uno R3 (compatible)
- Components:
  - LCD 16x2 (1602) with I2C backpack
  - Push button
  - Breadboard and jumper wires

## Wiring

| Component pin | Arduino pin |
|---------------|-------------|
| LCD GND       | GND         |
| LCD VCC       | 5V          |
| LCD SDA       | SDA (or A4) |
| LCD SCL       | SCL (or A5) |
| Button leg 1  | D2          |
| Button leg 2  | GND         |

The button uses the internal pull-up resistor, so it needs no external resistor. Use two diagonal legs of the button.

[Wokwi diagram](diagram.json)

## Libraries

- [LiquidCrystal I2C](https://github.com/johnrickman/LiquidCrystal_I2C) by Frank de Brabander (install from the Library Manager)

## Code

[beer-pour-tracker.ino](beer-pour-tracker.ino ':include :type=code')

## Troubleshooting

- **Backlight is on, but no text:** turn the blue contrast screw on the back of the LCD.
- **Screen stays blank:** change the I2C address from `0x27` to `0x3F`. Run an I2C scanner sketch to find it.

## Earlier versions

- [OLED version](https://github.com/JorgenJJ/arduino-projects/tree/beer-pour-tracker-oled/projects/beer-pour-tracker) (2.42" SSD1309, tag `beer-pour-tracker-oled`)

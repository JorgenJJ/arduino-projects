# Beer pour tracker

Prototype that tracks how much beer is poured, and how much CO2 is in it. The OLED shows the totals. A button starts and stops a simulated pour.

## How it works

- Press the button to start pouring. Press again to stop.
- Flow rate: 0.059 L/s (2 oz/s, a common tap design rate).
- CO2: 1.6 g per 0.33 L of lager (≈ 4.85 g/L).

## Hardware

- Board: Arduino Uno R3 (compatible)
- Components:
  - OLED 2.42" 128x64, SSD1309, I2C
  - Push button
  - Breadboard and jumper wires

## Wiring

| Component pin | Arduino pin |
|---------------|-------------|
| OLED GND      | GND         |
| OLED VCC      | 5V          |
| OLED SCL      | A5          |
| OLED SDA      | A4          |
| Button leg 1  | D2          |
| Button leg 2  | GND         |

The button uses the internal pull-up resistor, so it needs no external resistor. Use two diagonal legs of the button.

[Wokwi diagram](diagram.json)

![Wiring](wiring.jpg)

## Libraries

- [U8g2](https://github.com/olikraus/u8g2) (install from the Library Manager)

## Code

[beer-pour-tracker.ino](beer-pour-tracker.ino ':include :type=code')

## Troubleshooting

- **Image is shifted by 2 pixels:** change `NONAME0` to `NONAME2` in the display constructor.
- **Screen stays black:** check that the I2C address is 0x3C. Run an I2C scanner sketch to find it.

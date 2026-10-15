# Button toggle LED

First project: press a button to turn a light on. Press again to turn it off, like a light switch.

## How it works

```
 +---------+  press   +----------+
 |   OFF   | -------> |    ON    |
 | LED LOW | <------- | LED HIGH |
 +---------+  press   +----------+
```

- The Arduino reads the button many thousand times per second.
- It acts only when the button **changes** from up to down. Holding the button does nothing more.
- Each press flips (toggles) the light.

## Hardware

- Board: Arduino Uno R3 (compatible)
- All parts are in the ACEBOTT Complete Starter Kit.
- Components:
  - LED, red, 5 mm
  - 220Ω resistor (red-red-brown)
  - Push button
  - Breadboard and 4 jumper wires
  - USB-A to USB-B cable

## Step 1: Set up the Arduino IDE

1. Download and install the [Arduino IDE](https://www.arduino.cc/en/software).
2. Connect the Uno to the computer with the USB cable. The green **ON** LED comes on.
3. In the IDE, open the board menu at the top. Select **Arduino Uno** and its COM port.
4. Test the board: open **File → Examples → 01.Basics → Blink** and click **Upload** (→).
5. The small **L** LED on the board blinks once per second. Your setup works.

> **No Arduino port?** Compatible boards often use a CH340 USB chip, and Windows has no driver for it. "Unknown" ports are often Bluetooth. In **Device Manager**, the board shows as **USB Serial** with a warning icon. Install the [CH340 driver](https://www.wch-ic.com/downloads/CH341SER_EXE.html) ([guide](https://learn.sparkfun.com/tutorials/how-to-install-ch340-drivers/all)), then connect the board again. It then shows as **USB-SERIAL CH340 (COMx)**.

## Step 2: Learn the parts

```
    LED              Resistor         Push button

   .---.                              1 +-----+ 1
   |   |           --[|||||]--          | (O) |
   '---'           no polarity        2 +-----+ 2
    | |                               legs with the same
    | +-- long leg  = anode   (+)     number are connected
    |
    +---- short leg = cathode (-), flat edge
```

- **LED:** current flows only one way. The long leg goes toward the Arduino pin. The short leg goes toward GND.
- **Resistor:** limits the current, so the LED and the pin do not burn out. Without it, the LED gets too much current.
- **Button:** when you push it, the two sides connect. Use two **diagonal** legs, then you always get one leg from each side.

## Step 3: Wire the circuit

Disconnect the USB cable before you wire.

| Component pin             | Arduino pin |
|---------------------------|-------------|
| Resistor leg 1            | D13         |
| Resistor leg 2            | LED anode (long leg) |
| LED cathode (short leg)   | GND         |
| Button leg 1              | D2          |
| Button leg 2 (diagonal)   | GND         |

[Wokwi diagram](diagram.json)

```
 D13 ---[220 ohm]---|>|--- GND     LED circuit
                    LED

 D2 ----o  o---- GND              Button circuit
       button
```

The button uses the internal pull-up resistor, so it needs no external resistor:

- Button **up**: the pin reads `HIGH` (5V through the internal resistor).
- Button **down**: the pin connects to GND and reads `LOW`.

> **Breadboard tip:** the 5 holes in a short row (a–e or f–j) are connected. The long rails at the edges are connected along their length. Put the button across the center gap.

## Step 4: Upload the code

1. In the IDE, click **File → New Sketch** and paste the code below.
2. Click **Upload** (→).
3. Press the button. The LED comes on. Press again. The LED goes off.

The **L** LED on the board is also on pin 13, so it toggles too.

[button-toggle-led.ino](button-toggle-led.ino ':include :type=code')

## Step 5: Understand the code

**`setup()`** runs one time at start.

```cpp
pinMode(BUTTON_PIN, INPUT_PULLUP);  // read the button, use internal pull-up
pinMode(LED_PIN, OUTPUT);           // drive the LED
```

**`loop()`** runs again and again. When `buttonPressed()` is true, it flips `lightOn` and writes it to the LED.

```cpp
lightOn = !lightOn;              // true → false, false → true
digitalWrite(LED_PIN, lightOn);  // true = HIGH = 5V = LED on
```

**`buttonPressed()`** does two things:

1. **Edge detection:** it returns `true` only when the state changes from `HIGH` to `LOW`. This is one press, not "is held down".
2. **Debounce:** the metal contacts in a button bounce for some milliseconds. The pin then reads many fast presses. The function ignores changes closer than `DEBOUNCE_MS` (50 ms).

```
Pin level when you press, without debounce:

HIGH ----+ +-+ +-+
         | | | | |
LOW      +-+ +-+ +--------   <- bounce: 3 "presses"
         |<- 50 ms ->|       <- debounce ignores this
```

## Troubleshooting

- **LED never comes on:** turn the LED around. The long leg goes to the resistor.
- **LED toggles at random, or two times for one press:** use two diagonal button legs. Increase `DEBOUNCE_MS` to 100.
- **Upload fails:** select the correct board and COM port. Close other programs that use the port (for example Serial Monitor).

## Next steps

- Print the state: add `Serial.begin(9600);` to `setup()` and `Serial.println(lightOn);` after the toggle. Open **Tools → Serial Monitor**.
- Switch a real lamp with the 5V relay module on pin 13. Use only low-voltage loads (for example a 12V LED strip). Do **not** connect mains voltage (230V) without training.

## Sources

- [Debounce](https://docs.arduino.cc/built-in-examples/digital/Debounce/) and [State Change Detection](https://docs.arduino.cc/built-in-examples/digital/StateChangeDetection/), Arduino docs
- [Input Pullup Serial](https://docs.arduino.cc/built-in-examples/digital/InputPullupSerial/), Arduino docs
- [A Guide to Debouncing](https://www.ganssle.com/debouncing.htm), Jack Ganssle

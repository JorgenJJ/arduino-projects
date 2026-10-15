#include <U8g2lib.h>

const uint8_t BUTTON_PIN = 2;
const unsigned long DEBOUNCE_MS = 50;
const float FLOW_L_PER_S = 0.059;      // 2 oz/s, common tap design rate
const float CO2_G_PER_L = 1.6 / 0.33;  // lager

// Page buffer (_2_) saves RAM on the Uno. Try NONAME2 if the image is shifted.
U8G2_SSD1309_128X64_NONAME0_2_HW_I2C display(U8G2_R0);

bool pouring = false;
float pouredL = 0;
unsigned long lastUpdateMs = 0;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  display.setBusClock(400000);
  display.begin();
}

void loop() {
  if (buttonPressed()) pouring = !pouring;

  unsigned long now = millis();
  if (pouring) pouredL += FLOW_L_PER_S * (now - lastUpdateMs) / 1000.0;
  lastUpdateMs = now;

  draw();
}

bool buttonPressed() {
  static bool lastState = HIGH;
  static unsigned long lastChangeMs = 0;

  bool state = digitalRead(BUTTON_PIN);
  if (state == lastState || millis() - lastChangeMs < DEBOUNCE_MS) return false;

  lastState = state;
  lastChangeMs = millis();
  return state == LOW;
}

void draw() {
  display.firstPage();
  do {
    display.setFont(u8g2_font_6x10_tr);
    display.drawStr(0, 10, pouring ? "POURING" : "STOPPED");
    display.drawStr(0, 34, "Beer");
    display.drawStr(0, 58, "CO2");

    display.setFont(u8g2_font_helvB14_tr);
    display.setCursor(36, 36);
    display.print(pouredL, 2);
    display.print(" L");
    display.setCursor(36, 60);
    display.print(pouredL * CO2_G_PER_L, 1);
    display.print(" g");
  } while (display.nextPage());
}

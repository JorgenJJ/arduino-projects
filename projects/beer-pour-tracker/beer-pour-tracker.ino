#include <LiquidCrystal_I2C.h>

const uint8_t BUTTON_PIN = 2;
const unsigned long DEBOUNCE_MS = 50;
const float FLOW_L_PER_S = 0.059;      // 2 oz/s, common tap design rate
const float CO2_G_PER_L = 1.6 / 0.33;  // lager

// Try 0x3F if the screen stays blank
LiquidCrystal_I2C lcd(0x27, 16, 2);

bool pouring = false;
float pouredL = 0;
unsigned long lastUpdateMs = 0;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  lcd.init();
  lcd.backlight();
  lcd.print("Beer");
  lcd.setCursor(0, 1);
  lcd.print("CO2");
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

// Fixed-width fields overwrite old values, so the screen never needs clear()
void draw() {
  char value[8];
  lcd.setCursor(5, 0);
  lcd.print(dtostrf(pouredL, 5, 2, value));
  lcd.print(" L ");
  lcd.print(pouring ? "POUR" : "STOP");
  lcd.setCursor(5, 1);
  lcd.print(dtostrf(pouredL * CO2_G_PER_L, 5, 1, value));
  lcd.print(" g");
}

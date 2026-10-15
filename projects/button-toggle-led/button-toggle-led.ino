const uint8_t BUTTON_PIN = 2;
const uint8_t LED_PIN = 13;
const unsigned long DEBOUNCE_MS = 50;

bool lightOn = false;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  if (buttonPressed()) {
    lightOn = !lightOn;
    digitalWrite(LED_PIN, lightOn);
  }
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

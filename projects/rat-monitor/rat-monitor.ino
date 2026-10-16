#ifndef ZIGBEE_MODE_ED
#error "Select Tools -> Zigbee mode -> Zigbee ED (end device)"
#endif

#include "Zigbee.h"

const uint8_t PIR_PIN = D1;  // GPIO1, a low-power pin that can wake deep sleep
const uint8_t ENDPOINT = 10;

ZigbeeOccupancySensor occupancy(ENDPOINT);

void setup() {
  pinMode(PIR_PIN, INPUT);

  occupancy.setManufacturerAndModel("DIY", "RatMonitor");
  occupancy.setPowerSource(ZB_POWER_SOURCE_BATTERY, 100);  // no battery measurement yet
  Zigbee.addEndpoint(&occupancy);

  esp_zb_cfg_t config = ZIGBEE_DEFAULT_ED_CONFIG();
  config.nwk_cfg.zed_cfg.keep_alive = 10000;
  Zigbee.setTimeout(10000);
  if (!Zigbee.begin(&config, false)) sleepUntilMotion();
  while (!Zigbee.connected()) delay(100);

  // First boot only joins the network; a PIR wake is a visit
  if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT1) reportVisit();
  sleepUntilMotion();
}

void loop() {}

void reportVisit() {
  occupancy.setOccupancy(true);
  occupancy.report();
  while (digitalRead(PIR_PIN) == HIGH) delay(100);
  occupancy.setOccupancy(false);
  occupancy.report();
  delay(100);
}

void sleepUntilMotion() {
  esp_sleep_enable_ext1_wakeup(BIT(PIR_PIN), ESP_EXT1_WAKEUP_ANY_HIGH);
  esp_deep_sleep_start();
}

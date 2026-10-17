// Option B node: PIR and/or SHT40, sends to the hub over ESP-NOW, sleeps between events
#include <WiFi.h>
#include <Wire.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <RatMonitor.h>

const bool HAS_PIR = true;               // false for a climate-only node
const uint8_t PIR_PIN = D1;              // GPIO1, a low-power pin that can wake deep sleep
const uint32_t CLIMATE_INTERVAL = 900;   // seconds between climate readings
const uint32_t REPLY_TIMEOUT = 50;       // ms to wait for the hub on each try

const uint8_t BROADCAST[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Kept in deep sleep, lost on power loss
RTC_DATA_ATTR uint8_t channel = 1;       // Wi-Fi channel where the hub was last found
RTC_DATA_ATTR bool deterrentOn = false;  // from the hub; used by the phase 2 deterrent

volatile bool replied = false;
bool radioOn = false;
bool hasClimate = false;
float temperature, humidity;

void setup() {
  if (HAS_PIR) pinMode(PIR_PIN, INPUT);
  Wire.begin();
  hasClimate = RatMonitor::readSht40(temperature, humidity);

  if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT1) {
    send(RatMonitor::MOTION | RatMonitor::OCCUPIED);
    while (digitalRead(PIR_PIN) == HIGH) delay(100);
    send(RatMonitor::MOTION);
  } else if (hasClimate) {
    send(0);  // timer wake or first boot
  }
  sleep();
}

void loop() {}

void startRadio() {
  WiFi.mode(WIFI_STA);
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
  esp_now_init();
  esp_now_register_recv_cb(onReply);
  esp_now_peer_info_t peer = {};
  memcpy(peer.peer_addr, BROADCAST, 6);
  peer.channel = 0;  // follow the current channel
  peer.ifidx = WIFI_IF_STA;
  esp_now_add_peer(&peer);
  radioOn = true;
}

void onReply(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  RatMonitor::HubReply reply;
  if (len != sizeof(reply)) return;
  memcpy(&reply, data, len);
  if (reply.version != RatMonitor::VERSION) return;
  deterrentOn = reply.deterrent;
  replied = true;
}

// Adds the climate reading if the node has an SHT40
void send(uint8_t flags) {
  if (!radioOn) startRadio();
  RatMonitor::NodeMessage msg = {RatMonitor::VERSION, flags, 0, 0};
  if (hasClimate) {
    msg.flags |= RatMonitor::CLIMATE;
    msg.temperature = lroundf(temperature * 100);
    msg.humidity = lroundf(humidity * 100);
  }
  for (int i = 0; i < 3; i++) {
    if (trySend(msg, channel)) return;
  }
  // The hub follows the router's channel, so look for it on all channels
  for (uint8_t ch = 1; ch <= 13; ch++) {
    if (ch != channel && trySend(msg, ch)) {
      channel = ch;
      return;
    }
  }
}

bool trySend(const RatMonitor::NodeMessage &msg, uint8_t ch) {
  esp_wifi_set_channel(ch, WIFI_SECOND_CHAN_NONE);
  replied = false;
  esp_now_send(BROADCAST, (const uint8_t *)&msg, sizeof(msg));
  uint32_t start = millis();
  while (!replied && millis() - start < REPLY_TIMEOUT) delay(1);
  return replied;
}

// The timer is always on, so a missed SHT40 reading is retried next time
void sleep() {
  if (HAS_PIR) esp_sleep_enable_ext1_wakeup(BIT(PIR_PIN), ESP_EXT1_WAKEUP_ANY_HIGH);
  esp_sleep_enable_timer_wakeup(CLIMATE_INTERVAL * 1000000ULL);
  esp_deep_sleep_start();
}

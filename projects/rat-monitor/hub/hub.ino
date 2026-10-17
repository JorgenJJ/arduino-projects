// Option B hub: receives ESP-NOW from the basement nodes and forwards to Home Assistant over MQTT
#include <WiFi.h>
#include <Wire.h>
#include <esp_now.h>
#include <PubSubClient.h>
#include <RatMonitor.h>
#include "secrets.h"  // copy secrets.example.h to secrets.h and fill it in

const uint32_t HUB_CLIMATE_INTERVAL = 60000;  // ms between readings of the hub's own SHT40 (optional)
const int MAX_NODES = 20;                      // ESP-NOW peer limit

struct Received {
  uint8_t mac[6];
  RatMonitor::NodeMessage msg;
};

struct Node {
  uint8_t mac[6];
  uint8_t announced;  // NodeMessage flags already sent as HA discovery
};

QueueHandle_t queue;
Node nodes[MAX_NODES];
int nodeCount = 0;
volatile bool deterrentOn = false;
bool hubHasClimate = false;
uint8_t hubAnnounced = 0;
uint32_t lastHubClimate = 0, lastConnect = 0;

WiFiClient wifi;
PubSubClient mqtt(wifi);

void setup() {
  Serial.begin(115200);
  Wire.begin();
  float t, h;
  hubHasClimate = RatMonitor::readSht40(t, h);

  queue = xQueueCreate(20, sizeof(Received));
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);  // stay awake to hear the nodes
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) delay(100);
  Serial.printf("Wi-Fi channel %d\n", WiFi.channel());

  esp_now_init();
  esp_now_register_recv_cb(onReceive);

  mqtt.setServer(MQTT_HOST, MQTT_PORT);
  mqtt.setBufferSize(768);  // discovery messages are long
  mqtt.setCallback(onMqtt);
}

void loop() {
  if (!mqtt.connected()) {
    if (millis() - lastConnect > 5000) connectMqtt();
    return;
  }
  mqtt.loop();

  Received r;
  while (xQueueReceive(queue, &r, 0)) publish(r.mac, r.msg);

  if (hubHasClimate && millis() - lastHubClimate > HUB_CLIMATE_INTERVAL) {
    lastHubClimate = millis();
    float t, h;
    if (RatMonitor::readSht40(t, h)) {
      RatMonitor::NodeMessage msg = {RatMonitor::VERSION, RatMonitor::CLIMATE, (int16_t)lroundf(t * 100), (uint16_t)lroundf(h * 100)};
      publishClimate("hub", msg, hubAnnounced);
    }
  }
}

// Runs in the Wi-Fi task: reply at once, publish later from loop()
void onReceive(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  Received r;
  if (len != sizeof(r.msg)) return;
  memcpy(&r.msg, data, len);
  if (r.msg.version != RatMonitor::VERSION) return;
  memcpy(r.mac, info->src_addr, 6);

  if (!esp_now_is_peer_exist(r.mac)) {
    esp_now_peer_info_t peer = {};
    memcpy(peer.peer_addr, r.mac, 6);
    peer.channel = 0;
    peer.ifidx = WIFI_IF_STA;
    esp_now_add_peer(&peer);
  }
  RatMonitor::HubReply reply = {RatMonitor::VERSION, deterrentOn};
  esp_now_send(r.mac, (const uint8_t *)&reply, sizeof(reply));
  xQueueSend(queue, &r, 0);
}

void connectMqtt() {
  lastConnect = millis();
  if (!mqtt.connect("rat-monitor-hub", MQTT_USER, MQTT_PASSWORD)) return;
  for (int i = 0; i < nodeCount; i++) nodes[i].announced = 0;  // the broker may have lost discovery
  hubAnnounced = 0;

  const char *device = "\"device\":{\"identifiers\":[\"ratmon_hub\"],\"name\":\"Rat monitor hub\",\"manufacturer\":\"DIY\"}";
  char config[512];
  snprintf(config, sizeof(config),
    "{\"name\":\"Deterrent\",\"unique_id\":\"ratmon_hub_deterrent\",\"icon\":\"mdi:rodent\","
    "\"command_topic\":\"ratmonitor/hub/deterrent/set\",\"state_topic\":\"ratmonitor/hub/deterrent\",\"retain\":true,%s}",
    device);
  mqtt.publish("homeassistant/switch/ratmon_hub/deterrent/config", config, true);
  mqtt.subscribe("ratmonitor/hub/deterrent/set");
}

// HA switch: the deterrent state goes to each node in its next reply
void onMqtt(char *topic, byte *payload, unsigned int len) {
  deterrentOn = len == 2 && memcmp(payload, "ON", 2) == 0;
  mqtt.publish("ratmonitor/hub/deterrent", deterrentOn ? "ON" : "OFF", true);
}

void publish(const uint8_t *mac, const RatMonitor::NodeMessage &msg) {
  Node *node = findNode(mac);
  if (!node) return;
  char id[7];
  snprintf(id, sizeof(id), "%02x%02x%02x", mac[3], mac[4], mac[5]);

  if (msg.flags & RatMonitor::MOTION) {
    if (!(node->announced & RatMonitor::MOTION)) {
      announce(id, "binary_sensor", "occupancy", "Motion", "occupancy", "", "", 0);
      node->announced |= RatMonitor::MOTION;
    }
    publishState(id, "occupancy", msg.flags & RatMonitor::OCCUPIED ? "ON" : "OFF");
  }
  if (msg.flags & RatMonitor::CLIMATE) publishClimate(id, msg, node->announced);
}

void publishClimate(const char *id, const RatMonitor::NodeMessage &msg, uint8_t &announced) {
  if (!(announced & RatMonitor::CLIMATE)) {
    // Shown as unavailable if a node misses about 3 readings
    announce(id, "sensor", "temperature", "Temperature", "temperature", "°C", "{{ value_json.temperature }}", 3000);
    announce(id, "sensor", "humidity", "Humidity", "humidity", "%", "{{ value_json.humidity }}", 3000);
    announced |= RatMonitor::CLIMATE;
  }
  char value[48];
  snprintf(value, sizeof(value), "{\"temperature\":%.2f,\"humidity\":%.2f}", msg.temperature / 100.0, msg.humidity / 100.0);
  publishState(id, "climate", value);
}

void publishState(const char *id, const char *key, const char *value) {
  char topic[48];
  snprintf(topic, sizeof(topic), "ratmonitor/%s/%s", id, key);
  mqtt.publish(topic, value);
}

// Home Assistant MQTT discovery: the entity shows up by itself
void announce(const char *id, const char *component, const char *key, const char *name,
              const char *deviceClass, const char *unit, const char *valueTemplate, int expireAfter) {
  bool isClimate = valueTemplate[0];
  char config[640], topic[96];
  snprintf(topic, sizeof(topic), "homeassistant/%s/ratmon_%s/%s/config", component, id, key);
  int n = snprintf(config, sizeof(config),
    "{\"name\":\"%s\",\"unique_id\":\"ratmon_%s_%s\",\"device_class\":\"%s\",\"state_topic\":\"ratmonitor/%s/%s\"",
    name, id, key, deviceClass, id, isClimate ? "climate" : key);
  if (isClimate) {
    n += snprintf(config + n, sizeof(config) - n,
      ",\"unit_of_measurement\":\"%s\",\"value_template\":\"%s\",\"state_class\":\"measurement\",\"expire_after\":%d",
      unit, valueTemplate, expireAfter);
  }
  snprintf(config + n, sizeof(config) - n,
    ",\"device\":{\"identifiers\":[\"ratmon_%s\"],\"name\":\"Rat monitor %s\",\"manufacturer\":\"DIY\"}}", id, id);
  mqtt.publish(topic, config, true);
}

Node *findNode(const uint8_t *mac) {
  for (int i = 0; i < nodeCount; i++) {
    if (memcmp(nodes[i].mac, mac, 6) == 0) return &nodes[i];
  }
  if (nodeCount == MAX_NODES) return nullptr;
  Node *node = &nodes[nodeCount++];
  memcpy(node->mac, mac, 6);
  node->announced = 0;
  return node;
}

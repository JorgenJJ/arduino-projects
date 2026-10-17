// Shared by the rat monitor node and hub (option B): ESP-NOW messages and the SHT40 reader
#pragma once

#include <Arduino.h>
#include <Wire.h>

namespace RatMonitor {

const uint8_t VERSION = 1;

// NodeMessage.flags
const uint8_t MOTION = 1 << 0;    // the message has a PIR state
const uint8_t OCCUPIED = 1 << 1;  // PIR state: on
const uint8_t CLIMATE = 1 << 2;   // the message has temperature and humidity

// Node -> hub
struct __attribute__((packed)) NodeMessage {
  uint8_t version;
  uint8_t flags;
  int16_t temperature;  // 0.01 °C
  uint16_t humidity;    // 0.01 %RH
};

// Hub -> node, sent after each NodeMessage
struct __attribute__((packed)) HubReply {
  uint8_t version;
  uint8_t deterrent;  // 1 = deterrent period is ON (phase 2)
};

// Sensirion SHT40 temperature and humidity sensor (I2C)
const uint8_t SHT40_ADDRESS = 0x44;

inline uint8_t sht40Crc(const uint8_t *data) {
  uint8_t crc = 0xFF;
  for (int i = 0; i < 2; i++) {
    crc ^= data[i];
    for (int bit = 0; bit < 8; bit++) crc = crc & 0x80 ? (crc << 1) ^ 0x31 : crc << 1;
  }
  return crc;
}

// Returns false if no SHT40 answers, so it also detects the sensor
inline bool readSht40(float &temperature, float &humidity) {
  Wire.beginTransmission(SHT40_ADDRESS);
  Wire.write(0xFD);  // measure, high precision
  if (Wire.endTransmission() != 0) return false;
  delay(10);
  uint8_t b[6];
  if (Wire.requestFrom(SHT40_ADDRESS, (uint8_t)6) != 6) return false;
  for (uint8_t &x : b) x = Wire.read();
  if (sht40Crc(b) != b[2] || sht40Crc(b + 3) != b[5]) return false;
  temperature = -45 + 175.0f * (b[0] << 8 | b[1]) / 65535;
  humidity = constrain(-6 + 125.0f * (b[3] << 8 | b[4]) / 65535, 0.0f, 100.0f);
  return true;
}

}  // namespace RatMonitor

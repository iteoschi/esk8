#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <EEPROM.h>
#include <stdio.h>
#include <string.h>

#include "../common/config.h"
#include "../common/radio_protocol.h"

struct RemoteSettings {
  uint16_t version;
  int16_t hall_min;
  int16_t hall_center;
  int16_t hall_max;
  uint8_t deadzone;
  uint8_t throttle_max_percent;
  uint8_t brake_max_percent;
  uint8_t accel_time_s;
  uint8_t brake_time_s;
  uint8_t cruise_time_s;
  uint8_t range_km;
  uint8_t display_rotate;
  uint8_t crc;
} __attribute__((packed));

struct BindingStorage {
  uint8_t valid;
  uint8_t address[5];
  uint8_t crc;
} __attribute__((packed));

enum SettingId : uint8_t {
  SET_THROTTLE_MAX = 0,
  SET_BRAKE_MAX,
  SET_DEADZONE,
  SET_ACCEL_TIME,
  SET_BRAKE_TIME,
  SET_RANGE_KM,
  SET_ROTATE,
  SET_CAL_MIN,
  SET_CAL_CENTER,
  SET_CAL_MAX,
  SET_TRIP_RESET,
  SET_BIND,
  SET_RESET_DEFAULTS,
  SETTING_COUNT
};

const uint8_t PAGE_RIDE = 0;
const uint8_t PAGE_STATS = 1;
const uint8_t PAGE_ENERGY = 2;
const uint8_t PAGE_CONFIG = 3;
const uint8_t PAGE_COUNT = 4;

const int EEPROM_SETTINGS_ADDR = 0;
const int EEPROM_BIND_ADDR = EEPROM_SETTINGS_ADDR + sizeof(RemoteSettings);

extern RF24 radio;
#if ENABLE_REMOTE_OLED
extern U8G2_SSD1306_128X32_UNIVISION_1_HW_I2C u8g2;
#endif

extern RadioCommand command;
extern RadioTelemetry telemetry;

extern RemoteSettings settings;
extern BindingStorage binding;

extern uint8_t radioAddress[5];
extern uint8_t pendingBindAddress[5];

extern uint32_t lastTxMs;
extern uint32_t lastAckMs;
extern uint32_t lastUiMs;
extern uint32_t lastBindMs;
extern uint32_t lastThrottleMs;
extern uint32_t settingsDirtyMs;

extern bool connected;
extern bool bindingMode;
extern bool settingsMode;
extern bool settingsEdit;
extern bool settingsNavLatch;
extern bool settingsDirty;
extern bool tripResetRequested;

extern uint8_t currentPage;
extern uint8_t configSelection;
extern bool lightsEnabled;
extern bool cruiseEnabled;

extern bool triggerPrev;
extern uint32_t triggerDownMs;
extern int lastHallRaw;
extern int16_t outputThrottle;
extern uint8_t settingsIndex;

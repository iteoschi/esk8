#include "remote_storage.h"

uint8_t crc8(const uint8_t *data, size_t len) {
  uint8_t crc = 0x00;
  for (size_t i = 0; i < len; i++) {
    crc ^= data[i];
    for (uint8_t b = 0; b < 8; b++) {
      if (crc & 0x80) {
        crc = (uint8_t)((crc << 1) ^ 0x07);
      } else {
        crc <<= 1;
      }
    }
  }
  return crc;
}

void setDefaultSettings() {
  settings.version = REMOTE_SETTINGS_VERSION;
  settings.hall_min = REMOTE_HALL_MIN;
  settings.hall_center = REMOTE_HALL_CENTER;
  settings.hall_max = REMOTE_HALL_MAX;
  settings.deadzone = REMOTE_HALL_DEADZONE;
  settings.throttle_max_percent = REMOTE_THROTTLE_MAX_PERCENT;
  settings.brake_max_percent = REMOTE_BRAKE_MAX_PERCENT;
  settings.accel_time_s = REMOTE_ACCEL_TIME_S;
  settings.brake_time_s = REMOTE_BRAKE_TIME_S;
  settings.cruise_time_s = REMOTE_CRUISE_TIME_S;
  settings.range_km = REMOTE_RANGE_KM_DEFAULT;
  settings.display_rotate = 0;
}

void normalizeHallSettings() {
  if (settings.hall_min >= settings.hall_center || settings.hall_center >= settings.hall_max) {
    settings.hall_min = REMOTE_HALL_MIN;
    settings.hall_center = REMOTE_HALL_CENTER;
    settings.hall_max = REMOTE_HALL_MAX;
  }
}

void saveSettings() {
#if ENABLE_REMOTE_PERSISTENCE
  settings.version = REMOTE_SETTINGS_VERSION;
  settings.crc = crc8(reinterpret_cast<const uint8_t *>(&settings), sizeof(RemoteSettings) - 1);
  EEPROM.put(EEPROM_SETTINGS_ADDR, settings);
#endif
  settingsDirty = false;
}

void markSettingsDirty() {
  settingsDirty = true;
  settingsDirtyMs = millis();
}

void maybeSaveSettings() {
  if (!settingsDirty) {
    return;
  }
  if (millis() - settingsDirtyMs >= REMOTE_SETTINGS_SAVE_MS) {
    saveSettings();
  }
}

void loadSettings() {
#if ENABLE_REMOTE_PERSISTENCE
  EEPROM.get(EEPROM_SETTINGS_ADDR, settings);
  uint8_t crc = crc8(reinterpret_cast<const uint8_t *>(&settings), sizeof(RemoteSettings) - 1);
  if (settings.version != REMOTE_SETTINGS_VERSION || crc != settings.crc) {
    setDefaultSettings();
    saveSettings();
  }
#else
  setDefaultSettings();
#endif
  normalizeHallSettings();
}

void saveBinding() {
#if ENABLE_REMOTE_PERSISTENCE
  binding.valid = 1;
  binding.crc = crc8(reinterpret_cast<const uint8_t *>(&binding), sizeof(BindingStorage) - 1);
  EEPROM.put(EEPROM_BIND_ADDR, binding);
#endif
}

void loadBinding() {
#if ENABLE_REMOTE_PERSISTENCE
  EEPROM.get(EEPROM_BIND_ADDR, binding);
  uint8_t crc = crc8(reinterpret_cast<const uint8_t *>(&binding), sizeof(BindingStorage) - 1);
  if (binding.valid != 1 || crc != binding.crc) {
    binding.valid = 0;
    memcpy(radioAddress, RADIO_ADDRESS, sizeof(radioAddress));
    return;
  }
  memcpy(radioAddress, binding.address, sizeof(radioAddress));
#else
  memcpy(radioAddress, RADIO_ADDRESS, sizeof(radioAddress));
#endif
}

void applyDisplayRotation() {
#if ENABLE_REMOTE_OLED
  if (settings.display_rotate) {
    u8g2.setDisplayRotation(U8G2_R2);
  } else {
    u8g2.setDisplayRotation(U8G2_R0);
  }
#endif
}

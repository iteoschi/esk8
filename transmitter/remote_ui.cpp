#include "remote_ui.h"
#include "remote_storage.h"
#include "remote_radio.h"
#include "remote_input.h"

static bool settingIsAction(uint8_t index) {
  return index >= SET_CAL_MIN;
}

static void adjustSetting(int delta) {
  switch (settingsIndex) {
    case SET_THROTTLE_MAX:
      settings.throttle_max_percent = constrain((int)settings.throttle_max_percent + delta, 20, 100);
      markSettingsDirty();
      break;
    case SET_BRAKE_MAX:
      settings.brake_max_percent = constrain((int)settings.brake_max_percent + delta, 20, 100);
      markSettingsDirty();
      break;
    case SET_DEADZONE:
      settings.deadzone = constrain((int)settings.deadzone + delta, 0, 120);
      markSettingsDirty();
      break;
    case SET_ACCEL_TIME:
      settings.accel_time_s = constrain((int)settings.accel_time_s + delta, 0, 5);
      markSettingsDirty();
      break;
    case SET_BRAKE_TIME:
      settings.brake_time_s = constrain((int)settings.brake_time_s + delta, 0, 5);
      markSettingsDirty();
      break;
    case SET_RANGE_KM:
      settings.range_km = constrain((int)settings.range_km + delta, 1, REMOTE_RANGE_KM_MAX);
      markSettingsDirty();
      break;
    case SET_ROTATE:
      if (delta != 0) {
        settings.display_rotate = settings.display_rotate ? 0 : 1;
        applyDisplayRotation();
        markSettingsDirty();
      }
      break;
    default:
      break;
  }
}

static void executeSettingAction(int hallRaw) {
  switch (settingsIndex) {
    case SET_CAL_MIN:
      settings.hall_min = hallRaw;
      normalizeHallSettings();
      markSettingsDirty();
      break;
    case SET_CAL_CENTER:
      settings.hall_center = hallRaw;
      normalizeHallSettings();
      markSettingsDirty();
      break;
    case SET_CAL_MAX:
      settings.hall_max = hallRaw;
      normalizeHallSettings();
      markSettingsDirty();
      break;
    case SET_TRIP_RESET:
      tripResetRequested = true;
      break;
    case SET_BIND:
      startBinding();
      break;
    case SET_RESET_DEFAULTS:
      setDefaultSettings();
      applyDisplayRotation();
      markSettingsDirty();
      break;
    default:
      break;
  }
}

void handleSettingsInput(int hallRaw) {
  bool triggerNow = triggerActive();
  bool neutral = abs(hallRaw - settings.hall_center) <= REMOTE_UI_NEUTRAL_MARGIN;

  if (triggerNow && !triggerPrev) {
    triggerDownMs = millis();
  }

  if (!triggerNow && triggerPrev) {
    uint32_t pressMs = millis() - triggerDownMs;
    if (neutral && pressMs >= REMOTE_UI_LONG_PRESS_MS) {
      settingsMode = false;
      settingsEdit = false;
      markSettingsDirty();
      triggerPrev = triggerNow;
      return;
    }

    if (neutral && pressMs <= REMOTE_UI_SHORT_PRESS_MS) {
      if (settingIsAction(settingsIndex)) {
        executeSettingAction(hallRaw);
      } else {
        settingsEdit = !settingsEdit;
      }
    }
  }

  if (!settingsEdit) {
    if (hallRaw > settings.hall_center + REMOTE_UI_NAV_MARGIN && !settingsNavLatch) {
      if (settingsIndex > 0) settingsIndex--;
      settingsNavLatch = true;
    } else if (hallRaw < settings.hall_center - REMOTE_UI_NAV_MARGIN && !settingsNavLatch) {
      if (settingsIndex + 1 < SETTING_COUNT) settingsIndex++;
      settingsNavLatch = true;
    } else if (neutral) {
      settingsNavLatch = false;
    }
  } else {
    if (hallRaw > settings.hall_center + REMOTE_UI_NAV_MARGIN && !settingsNavLatch) {
      adjustSetting(1);
      settingsNavLatch = true;
    } else if (hallRaw < settings.hall_center - REMOTE_UI_NAV_MARGIN && !settingsNavLatch) {
      adjustSetting(-1);
      settingsNavLatch = true;
    } else if (neutral) {
      settingsNavLatch = false;
    }
  }

  triggerPrev = triggerNow;
}

void handleUiInput(int hallRaw) {
  bool triggerNow = triggerActive();
  bool neutral = abs(hallRaw - settings.hall_center) <= REMOTE_UI_NEUTRAL_MARGIN;

  if (triggerNow && !triggerPrev) {
    triggerDownMs = millis();
  }

  if (!triggerNow && triggerPrev) {
    uint32_t pressMs = millis() - triggerDownMs;
    if (neutral && pressMs <= REMOTE_UI_SHORT_PRESS_MS) {
      if (currentPage == PAGE_CONFIG) {
        if (configSelection == 0) {
          lightsEnabled = !lightsEnabled;
        } else {
          cruiseEnabled = !cruiseEnabled;
        }
      } else {
        currentPage = (currentPage + 1) % PAGE_COUNT;
      }
    } else if (neutral && pressMs >= REMOTE_UI_LONG_PRESS_MS) {
      settingsMode = true;
      settingsEdit = false;
      settingsNavLatch = false;
      settingsIndex = 0;
    }
  }

  if (currentPage == PAGE_CONFIG) {
    if (hallRaw > settings.hall_center + REMOTE_UI_NAV_MARGIN) {
      configSelection = 0;
    } else if (hallRaw < settings.hall_center - REMOTE_UI_NAV_MARGIN) {
      configSelection = 1;
    }
  }

  triggerPrev = triggerNow;
}

#if ENABLE_REMOTE_OLED
static void formatFixed1(char *buffer, size_t size, int16_t valueX10) {
  int16_t whole = valueX10 / 10;
  int16_t frac = abs(valueX10 % 10);
  snprintf(buffer, size, "%d.%d", whole, frac);
}

static void drawRidePage() {
  char line[22];

  u8g2.setFont(u8g2_font_5x8_tf);

  formatFixed1(line, sizeof(line), telemetry.speed_kph_x10);
  u8g2.drawStr(0, 10, "SPD");
  u8g2.drawStr(24, 10, line);
  u8g2.drawStr(58, 10, "KMH");

  snprintf(line, sizeof(line), "B %u%%", telemetry.board_batt_percent);
  u8g2.drawStr(0, 22, line);

  snprintf(line, sizeof(line), "R %u%%", command.remote_batt_percent);
  u8g2.drawStr(52, 22, line);

  snprintf(line, sizeof(line), "RF %s L:%c C:%c", connected ? "OK" : "--", lightsEnabled ? '1' : '0', cruiseEnabled ? '1' : '0');
  u8g2.drawStr(0, 32, line);
}

static void drawStatsPage() {
  char line[22];

  u8g2.setFont(u8g2_font_5x8_tf);

  uint32_t meters = telemetry.trip_m;
  uint32_t km = meters / 1000;
  uint32_t dec = (meters % 1000) / 10;
  snprintf(line, sizeof(line), "TRIP %lu.%02lukm", (unsigned long)km, (unsigned long)dec);
  u8g2.drawStr(0, 10, line);

  snprintf(line, sizeof(line), "WH/KM %u.%u", telemetry.wh_per_km_x10 / 10, telemetry.wh_per_km_x10 % 10);
  u8g2.drawStr(0, 22, line);

  formatFixed1(line, sizeof(line), (int16_t)telemetry.max_speed_kph_x10);
  u8g2.drawStr(0, 32, "MAX");
  u8g2.drawStr(24, 32, line);
  u8g2.drawStr(58, 32, "KMH");
}

static void drawEnergyPage() {
  char line[22];
  u8g2.setFont(u8g2_font_5x8_tf);

  snprintf(line, sizeof(line), "IN %d.%dA",
           telemetry.current_x10 / 10, abs(telemetry.current_x10 % 10));
  u8g2.drawStr(0, 10, line);

  snprintf(line, sizeof(line), "MOT %d.%dA",
           telemetry.motor_current_x10 / 10, abs(telemetry.motor_current_x10 % 10));
  u8g2.drawStr(64, 10, line);

  snprintf(line, sizeof(line), "MOS %dC MOT %dC",
           telemetry.temp_mos_c_x10 / 10, telemetry.temp_motor_c_x10 / 10);
  u8g2.drawStr(0, 22, line);

  uint16_t rangeKm = (uint16_t)((settings.range_km * telemetry.board_batt_percent) / 100);
  snprintf(line, sizeof(line), "RNG %ukm AH %u.%u",
           rangeKm,
           telemetry.amp_hours_x100 / 100,
           (telemetry.amp_hours_x100 % 100) / 10);
  u8g2.drawStr(0, 32, line);
}

static void drawConfigPage() {
  char line[22];

  u8g2.setFont(u8g2_font_5x8_tf);
  u8g2.drawStr(0, 10, "CONFIG");

  snprintf(line, sizeof(line), "%sLIGHTS:%s", (configSelection == 0) ? ">" : " ", lightsEnabled ? "ON" : "OFF");
  u8g2.drawStr(0, 22, line);

  snprintf(line, sizeof(line), "%sCRUISE:%s", (configSelection == 1) ? ">" : " ", cruiseEnabled ? "ON" : "OFF");
  u8g2.drawStr(0, 32, line);
}

static void drawSettingsPage() {
  char line[22];
  char value[12];

  u8g2.setFont(u8g2_font_5x8_tf);
  snprintf(line, sizeof(line), "SET %u/%u", (unsigned int)(settingsIndex + 1), (unsigned int)SETTING_COUNT);
  u8g2.drawStr(0, 8, line);

  switch (settingsIndex) {
    case SET_THROTTLE_MAX:
      snprintf(line, sizeof(line), "THR MAX");
      snprintf(value, sizeof(value), "%u%%", settings.throttle_max_percent);
      break;
    case SET_BRAKE_MAX:
      snprintf(line, sizeof(line), "BRK MAX");
      snprintf(value, sizeof(value), "%u%%", settings.brake_max_percent);
      break;
    case SET_DEADZONE:
      snprintf(line, sizeof(line), "DEADZONE");
      snprintf(value, sizeof(value), "%u", settings.deadzone);
      break;
    case SET_ACCEL_TIME:
      snprintf(line, sizeof(line), "ACCEL S");
      snprintf(value, sizeof(value), "%u", settings.accel_time_s);
      break;
    case SET_BRAKE_TIME:
      snprintf(line, sizeof(line), "BRAKE S");
      snprintf(value, sizeof(value), "%u", settings.brake_time_s);
      break;
    case SET_RANGE_KM:
      snprintf(line, sizeof(line), "RANGE KM");
      snprintf(value, sizeof(value), "%u", settings.range_km);
      break;
    case SET_ROTATE:
      snprintf(line, sizeof(line), "ROTATE");
      snprintf(value, sizeof(value), "%s", settings.display_rotate ? "ON" : "OFF");
      break;
    case SET_CAL_MIN:
      snprintf(line, sizeof(line), "CAL MIN");
      snprintf(value, sizeof(value), "%d", lastHallRaw);
      break;
    case SET_CAL_CENTER:
      snprintf(line, sizeof(line), "CAL CTR");
      snprintf(value, sizeof(value), "%d", lastHallRaw);
      break;
    case SET_CAL_MAX:
      snprintf(line, sizeof(line), "CAL MAX");
      snprintf(value, sizeof(value), "%d", lastHallRaw);
      break;
    case SET_TRIP_RESET:
      snprintf(line, sizeof(line), "RESET TRIP");
      snprintf(value, sizeof(value), "PRESS");
      break;
    case SET_BIND:
      snprintf(line, sizeof(line), "BIND");
      snprintf(value, sizeof(value), "PRESS");
      break;
    case SET_RESET_DEFAULTS:
      snprintf(line, sizeof(line), "RESET ALL");
      snprintf(value, sizeof(value), "PRESS");
      break;
    default:
      snprintf(line, sizeof(line), "UNKNOWN");
      snprintf(value, sizeof(value), "-");
      break;
  }

  u8g2.drawStr(0, 18, line);
  u8g2.drawStr(0, 28, value);
  if (!settingIsAction(settingsIndex)) {
    u8g2.drawStr(80, 28, settingsEdit ? "EDIT" : "SEL");
  }
}
#endif

void updateDisplay() {
#if ENABLE_REMOTE_OLED
  uint32_t now = millis();
  if (now - lastUiMs < REMOTE_UI_REFRESH_MS) {
    return;
  }

  lastUiMs = now;

  u8g2.firstPage();
  do {
    if (settingsMode) {
      drawSettingsPage();
    } else if (currentPage == PAGE_RIDE) {
      drawRidePage();
    } else if (currentPage == PAGE_STATS) {
      drawStatsPage();
    } else if (currentPage == PAGE_ENERGY) {
      drawEnergyPage();
    } else {
      drawConfigPage();
    }
  } while (u8g2.nextPage());
#endif
}

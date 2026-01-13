#include "remote_input.h"

bool triggerActive() {
#if REMOTE_TRIGGER_ACTIVE_LOW
  return digitalRead(REMOTE_TRIGGER_PIN) == LOW;
#else
  return digitalRead(REMOTE_TRIGGER_PIN) == HIGH;
#endif
}

int readHallRaw() {
  long total = 0;
  for (int i = 0; i < REMOTE_HALL_SAMPLES; i++) {
    total += analogRead(REMOTE_HALL_PIN);
  }
  return (int)(total / REMOTE_HALL_SAMPLES);
}

int16_t mapThrottle(int raw) {
  int16_t center = settings.hall_center;
  int16_t minimum = settings.hall_min;
  int16_t maximum = settings.hall_max;
  uint8_t deadzone = settings.deadzone;

  if (abs(raw - center) <= deadzone) {
    return 0;
  }

  int16_t value = 0;
  if (raw >= center) {
    value = (int16_t)constrain(map(raw, center, maximum, 0, 1000), 0, 1000);
  } else {
    value = (int16_t)constrain(map(raw, minimum, center, -1000, 0), -1000, 0);
  }

  int16_t maxForward = (int16_t)(1000L * settings.throttle_max_percent / 100L);
  int16_t maxBrake = (int16_t)(1000L * settings.brake_max_percent / 100L);
  return (int16_t)constrain(value, -maxBrake, maxForward);
}

int16_t applyThrottleRamp(int16_t target) {
  uint32_t now = millis();
  if (lastThrottleMs == 0) {
    lastThrottleMs = now;
    outputThrottle = target;
    return outputThrottle;
  }

  uint32_t dtMs = now - lastThrottleMs;
  lastThrottleMs = now;

  if (dtMs == 0) {
    return outputThrottle;
  }

  uint8_t accelTime = (target >= outputThrottle) ? settings.accel_time_s : settings.brake_time_s;
  if (accelTime == 0) {
    outputThrottle = target;
    return outputThrottle;
  }

  float maxDelta = 1000.0f * (float)dtMs / (accelTime * 1000.0f);
  int16_t delta = target - outputThrottle;

  if (abs(delta) <= maxDelta) {
    outputThrottle = target;
  } else {
    outputThrottle += (delta > 0) ? (int16_t)maxDelta : (int16_t)-maxDelta;
  }

  return outputThrottle;
}

float readRemoteBatteryVoltage() {
  long total = 0;
  for (int i = 0; i < 8; i++) {
    total += analogRead(REMOTE_BATTERY_PIN);
  }

  float avg = total / 8.0f;
  return (avg / 1023.0f) * REMOTE_ADC_REF_V * REMOTE_BATTERY_DIVIDER;
}

uint8_t remoteBatteryPercent() {
  float voltage = readRemoteBatteryVoltage();
  float cellVoltage = voltage / REMOTE_BATTERY_CELL_COUNT;
  float percent = (cellVoltage - REMOTE_CELL_MIN_V) * 100.0f / (REMOTE_CELL_MAX_V - REMOTE_CELL_MIN_V);
  if (percent < 0.0f) percent = 0.0f;
  if (percent > 100.0f) percent = 100.0f;
  return (uint8_t)(percent + 0.5f);
}

#include "board_control.h"
#include "board_utils.h"

static int16_t computeTargetThrottle() {
  uint32_t now = millis();
  bool radioTimeout = (now - lastRadioMs) > RADIO_FAILSAFE_MS;

  if (radioTimeout) {
    cruiseActive = false;
    return 0;
  }

#if ENABLE_CRUISE
  bool cruiseRequested = (lastCommand.flags & RADIO_FLAG_CRUISE) != 0;
  if (!cruiseRequested) {
    cruiseActive = false;
  }

  if (cruiseRequested && !cruiseActive) {
    if (abs(lastCommand.throttle) < BOARD_THROTTLE_DEADZONE && wheelSpeedKph >= BOARD_CRUISE_MIN_KPH) {
      cruiseTargetKph = wheelSpeedKph;
      cruiseActive = true;
    }
  }

  if (cruiseActive && lastCommand.throttle < -BOARD_THROTTLE_DEADZONE) {
    cruiseActive = false;
  }

  if (cruiseActive) {
    float error = cruiseTargetKph - wheelSpeedKph;
    float output = error * BOARD_CRUISE_KP;
    output = clampFloat(output, 0.0f, BOARD_CRUISE_MAX_THROTTLE);
    return (int16_t)(output * 1000.0f);
  }
#endif

  return lastCommand.throttle;
}

static void sendToVesc(int16_t throttle) {
#if ENABLE_VESC_UART
  if (abs(throttle) < BOARD_THROTTLE_DEADZONE) {
    throttle = 0;
  }

  float norm = clampFloat(throttle / 1000.0f, -1.0f, 1.0f);
  float current = (norm >= 0.0f) ? norm * BOARD_MAX_CURRENT_A : norm * BOARD_MAX_BRAKE_CURRENT_A;
  vesc.setCurrent(current);
#endif
}

void updateControl() {
  uint32_t now = millis();
  if (now - lastControlMs < BOARD_CONTROL_INTERVAL_MS) {
    return;
  }

  lastControlMs = now;
  int16_t target = computeTargetThrottle();
  target = clampInt16(target, -1000, 1000);

  appliedThrottle = rampToward(appliedThrottle, target, BOARD_FAILSAFE_RAMP_STEP);
  sendToVesc(appliedThrottle);
}

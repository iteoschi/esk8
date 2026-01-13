#include "board_lights.h"

void setupLights() {
#if ENABLE_LIGHTS
  pinMode(BOARD_LIGHT_HEAD_PIN, OUTPUT);
  pinMode(BOARD_LIGHT_BRAKE_PIN, OUTPUT);
  digitalWrite(BOARD_LIGHT_HEAD_PIN, LOW);
  digitalWrite(BOARD_LIGHT_BRAKE_PIN, LOW);
#endif
}

void updateLights() {
#if ENABLE_LIGHTS
  digitalWrite(BOARD_LIGHT_HEAD_PIN, lightsEnabled ? HIGH : LOW);

  bool brakeOn = (appliedThrottle < -BOARD_THROTTLE_DEADZONE);
  if (vescOk && vescData.avgInputCurrent < BOARD_BRAKE_CURRENT_ON_A) {
    brakeOn = true;
  }

  digitalWrite(BOARD_LIGHT_BRAKE_PIN, brakeOn ? HIGH : LOW);
#endif
}

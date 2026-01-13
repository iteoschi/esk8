#include "remote_state.h"
#include "remote_storage.h"
#include "remote_input.h"
#include "remote_radio.h"
#include "remote_ui.h"

void setup() {
  pinMode(REMOTE_TRIGGER_PIN, INPUT_PULLUP);
  pinMode(REMOTE_HALL_PIN, INPUT);
  pinMode(REMOTE_BATTERY_PIN, INPUT);

  loadSettings();
  loadBinding();
  lastHallRaw = settings.hall_center;

#if ENABLE_REMOTE_OLED
  u8g2.begin();
  applyDisplayRotation();
#endif

  setupRadio();
}

void loop() {
  lastHallRaw = readHallRaw();
  if (settingsMode) {
    handleSettingsInput(lastHallRaw);
  } else {
    handleUiInput(lastHallRaw);
  }

  int16_t throttle = mapThrottle(lastHallRaw);
  if (!triggerActive() && throttle > 0) {
    throttle = 0;
  }
  if (settingsMode || bindingMode) {
    throttle = 0;
  }

  throttle = applyThrottleRamp(throttle);

  if (bindingMode) {
    handleBinding();
  } else {
    sendCommand(throttle);
  }

  maybeSaveSettings();
  updateDisplay();
}

#include "board_wheel.h"
#include "board_utils.h"

#if ENABLE_WHEEL_HALL
static void IRAM_ATTR wheelHallISR() {
  wheelPulses++;
}
#endif

void setupWheelHall() {
#if ENABLE_WHEEL_HALL
  if (BOARD_WHEEL_HALL_PULLUP) {
    pinMode(BOARD_WHEEL_HALL_PIN, INPUT_PULLUP);
  } else {
    pinMode(BOARD_WHEEL_HALL_PIN, INPUT);
  }
  attachInterrupt(digitalPinToInterrupt(BOARD_WHEEL_HALL_PIN), wheelHallISR, RISING);
#endif
}

void updateWheelSpeed() {
#if ENABLE_WHEEL_HALL
  uint32_t now = millis();
  if (now - lastWheelCalcMs < BOARD_WHEEL_SPEED_INTERVAL_MS) {
    return;
  }

  lastWheelCalcMs = now;

  uint32_t pulses = 0;
  noInterrupts();
  pulses = wheelPulses;
  wheelPulses = 0;
  interrupts();

  float metersPerPulse = wheelCircumferenceM() / BOARD_MAGNETS_PER_REV;
  float distanceM = pulses * metersPerPulse;
  float intervalS = BOARD_WHEEL_SPEED_INTERVAL_MS / 1000.0f;

  odometerM += distanceM;
  tripM += distanceM;

  if (intervalS > 0.0f) {
    float speedMps = distanceM / intervalS;
    wheelSpeedKph = speedMps * 3.6f;
  } else {
    wheelSpeedKph = 0.0f;
  }

  if (wheelSpeedKph > maxSpeedKph) {
    maxSpeedKph = wheelSpeedKph;
  }
#endif
}

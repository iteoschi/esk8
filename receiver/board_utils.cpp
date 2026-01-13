#include "board_utils.h"

float clampFloat(float value, float minVal, float maxVal) {
  if (value < minVal) return minVal;
  if (value > maxVal) return maxVal;
  return value;
}

int16_t clampInt16(int16_t value, int16_t minVal, int16_t maxVal) {
  if (value < minVal) return minVal;
  if (value > maxVal) return maxVal;
  return value;
}

int16_t rampToward(int16_t current, int16_t target, int16_t step) {
  if (current < target) {
    return (target - current > step) ? current + step : target;
  }
  if (current > target) {
    return (current - target > step) ? current - step : target;
  }
  return current;
}

float wheelCircumferenceM() {
  return (BOARD_WHEEL_DIAMETER_MM / 1000.0f) * 3.1415926f;
}

float erpmToKph(float erpm) {
  float motorRpm = erpm / (BOARD_MOTOR_POLES / 2.0f);
  float wheelRpm = motorRpm / BOARD_GEAR_RATIO;
  float speedMps = (wheelRpm * wheelCircumferenceM()) / 60.0f;
  return speedMps * 3.6f;
}

uint8_t batteryPercent(float packVoltage) {
  float cellVoltage = packVoltage / BOARD_BATTERY_CELL_COUNT;
  float percent = (cellVoltage - BOARD_CELL_MIN_V) * 100.0f / (BOARD_CELL_MAX_V - BOARD_CELL_MIN_V);
  percent = clampFloat(percent, 0.0f, 100.0f);
  return (uint8_t)(percent + 0.5f);
}

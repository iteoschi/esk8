#pragma once

#include "board_state.h"

float clampFloat(float value, float minVal, float maxVal);
int16_t clampInt16(int16_t value, int16_t minVal, int16_t maxVal);
int16_t rampToward(int16_t current, int16_t target, int16_t step);
float wheelCircumferenceM();
float erpmToKph(float erpm);
uint8_t batteryPercent(float packVoltage);

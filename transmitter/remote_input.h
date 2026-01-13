#pragma once

#include "remote_state.h"

bool triggerActive();
int readHallRaw();
int16_t mapThrottle(int raw);
int16_t applyThrottleRamp(int16_t target);
float readRemoteBatteryVoltage();
uint8_t remoteBatteryPercent();

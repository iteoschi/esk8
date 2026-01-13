#pragma once

#include "remote_state.h"

void applyRadioAddress(const uint8_t *address);
void setupRadio();
void sendCommand(int16_t throttle);
void startBinding();
void handleBinding();

#pragma once

#include "remote_state.h"

uint8_t crc8(const uint8_t *data, size_t len);
void setDefaultSettings();
void normalizeHallSettings();
void loadSettings();
void saveSettings();
void markSettingsDirty();
void maybeSaveSettings();
void loadBinding();
void saveBinding();
void applyDisplayRotation();

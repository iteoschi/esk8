#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>
#include <VescUart.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <Preferences.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "../common/config.h"
#include "../common/radio_protocol.h"

extern RF24 radio;
extern VescUart vesc;
extern Preferences prefs;

#if ENABLE_BOARD_OLED
extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;
#endif

extern portMUX_TYPE telemetryMux;

extern RadioCommand lastCommand;
extern RadioTelemetry telemetry;

extern uint8_t radioAddress[5];
extern bool radioAddressValid;
extern uint32_t boardId;

extern volatile uint32_t wheelPulses;

extern float wheelSpeedKph;
extern float vescSpeedKph;
extern float maxSpeedKph;
extern float odometerM;
extern float tripM;
extern float energyWh;
extern float whPerKm;

extern uint32_t lastRadioMs;
extern uint32_t lastControlMs;
extern uint32_t lastWheelCalcMs;
extern uint32_t lastVescPollMs;
extern uint32_t lastEnergyMs;
extern uint32_t lastPersistMs;
extern float lastPersistOdo;
extern float lastPersistEnergy;
extern uint16_t lastTripResetSeq;

extern int16_t appliedThrottle;
extern bool cruiseActive;
extern float cruiseTargetKph;
extern bool lightsEnabled;
extern bool vescOk;

extern struct bldcMeasure vescData;

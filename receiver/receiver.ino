#include "board_state.h"
#include "board_persist.h"
#include "board_radio.h"
#include "board_vesc.h"
#include "board_wheel.h"
#include "board_control.h"
#include "board_lights.h"
#include "board_display.h"

void setup() {
  boardId = (uint32_t)(ESP.getEfuseMac() & 0xFFFFFFFF);
  loadBoardStorage();

  setupRadio();
  setupVesc();
  setupWheelHall();
  setupLights();
  setupDisplay();
}

void loop() {
  updateVesc();
  updateWheelSpeed();
  updateTelemetry();
  updateRadio();
  updateControl();
  updateLights();
  persistBoardStorage(false);
}

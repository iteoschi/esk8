#include "board_persist.h"
#include "board_utils.h"

static void setDefaultRadioAddress() {
  memcpy(radioAddress, RADIO_ADDRESS, sizeof(radioAddress));
  radioAddressValid = true;
}

void loadBoardStorage() {
#if ENABLE_BOARD_PERSISTENCE || ENABLE_BINDING
  prefs.begin("esk8", false);
#endif

  setDefaultRadioAddress();

#if ENABLE_BINDING
  size_t addrLen = prefs.getBytesLength("addr");
  if (addrLen == sizeof(radioAddress)) {
    prefs.getBytes("addr", radioAddress, sizeof(radioAddress));
    radioAddressValid = true;
  }
#endif

#if ENABLE_BOARD_PERSISTENCE
  odometerM = prefs.getUInt("odo_m", 0);
  tripM = prefs.getUInt("trip_m", 0);
  maxSpeedKph = prefs.getUShort("max_spd", 0) / 10.0f;
  energyWh = prefs.getUInt("wh_x10", 0) / 10.0f;
#endif

#if ENABLE_BOARD_PERSISTENCE || ENABLE_BINDING
  prefs.end();
#endif

  lastPersistOdo = odometerM;
  lastPersistEnergy = energyWh;
}

void persistBoardStorage(bool force) {
#if ENABLE_BOARD_PERSISTENCE || ENABLE_BINDING
  uint32_t now = millis();
  bool timeElapsed = (now - lastPersistMs) >= BOARD_PERSIST_INTERVAL_MS;
  bool distElapsed = fabsf(odometerM - lastPersistOdo) >= BOARD_PERSIST_DISTANCE_M;
  bool energyElapsed = fabsf(energyWh - lastPersistEnergy) >= BOARD_PERSIST_ENERGY_WH;

  if (!force && !(timeElapsed && (distElapsed || energyElapsed))) {
    return;
  }

  prefs.begin("esk8", false);
#if ENABLE_BOARD_PERSISTENCE
  prefs.putUInt("odo_m", (uint32_t)(odometerM + 0.5f));
  prefs.putUInt("trip_m", (uint32_t)(tripM + 0.5f));
  prefs.putUShort("max_spd", (uint16_t)(maxSpeedKph * 10.0f + 0.5f));
  prefs.putUInt("wh_x10", (uint32_t)(energyWh * 10.0f + 0.5f));
#endif
#if ENABLE_BINDING
  if (radioAddressValid) {
    prefs.putBytes("addr", radioAddress, sizeof(radioAddress));
  }
#endif
  prefs.end();

  lastPersistMs = now;
  lastPersistOdo = odometerM;
  lastPersistEnergy = energyWh;
#endif
}

void resetTripStats() {
  tripM = 0.0f;
  energyWh = 0.0f;
  whPerKm = 0.0f;
  maxSpeedKph = 0.0f;
  persistBoardStorage(true);
}

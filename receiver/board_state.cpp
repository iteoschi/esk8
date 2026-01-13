#include "board_state.h"

RF24 radio(BOARD_RF24_CE_PIN, BOARD_RF24_CSN_PIN);
VescUart vesc;
Preferences prefs;

#if ENABLE_BOARD_OLED
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
#endif

portMUX_TYPE telemetryMux = portMUX_INITIALIZER_UNLOCKED;

RadioCommand lastCommand = {};
RadioTelemetry telemetry = {};

uint8_t radioAddress[5] = {};
bool radioAddressValid = false;
uint32_t boardId = 0;

volatile uint32_t wheelPulses = 0;

float wheelSpeedKph = 0.0f;
float vescSpeedKph = 0.0f;
float maxSpeedKph = 0.0f;
float odometerM = 0.0f;
float tripM = 0.0f;
float energyWh = 0.0f;
float whPerKm = 0.0f;

uint32_t lastRadioMs = 0;
uint32_t lastControlMs = 0;
uint32_t lastWheelCalcMs = 0;
uint32_t lastVescPollMs = 0;
uint32_t lastEnergyMs = 0;
uint32_t lastPersistMs = 0;
float lastPersistOdo = 0.0f;
float lastPersistEnergy = 0.0f;
uint16_t lastTripResetSeq = 0;

int16_t appliedThrottle = 0;
bool cruiseActive = false;
float cruiseTargetKph = 0.0f;
bool lightsEnabled = false;
bool vescOk = false;

struct bldcMeasure vescData = {};

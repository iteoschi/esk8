#include "remote_state.h"

RF24 radio(REMOTE_RF24_CE_PIN, REMOTE_RF24_CSN_PIN);

#if ENABLE_REMOTE_OLED
U8G2_SSD1306_128X32_UNIVISION_1_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
#endif

RadioCommand command = {};
RadioTelemetry telemetry = {};

RemoteSettings settings = {};
BindingStorage binding = {};

uint8_t radioAddress[5] = {};
uint8_t pendingBindAddress[5] = {};

uint32_t lastTxMs = 0;
uint32_t lastAckMs = 0;
uint32_t lastUiMs = 0;
uint32_t lastBindMs = 0;
uint32_t lastThrottleMs = 0;
uint32_t settingsDirtyMs = 0;

bool connected = false;
bool bindingMode = false;
bool settingsMode = false;
bool settingsEdit = false;
bool settingsNavLatch = false;
bool settingsDirty = false;
bool tripResetRequested = false;

uint8_t currentPage = 0;
uint8_t configSelection = 0;
bool lightsEnabled = false;
bool cruiseEnabled = false;

bool triggerPrev = false;
uint32_t triggerDownMs = 0;
int lastHallRaw = REMOTE_HALL_CENTER;
int16_t outputThrottle = 0;
uint8_t settingsIndex = 0;

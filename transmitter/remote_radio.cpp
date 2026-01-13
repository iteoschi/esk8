#include "remote_radio.h"
#include "remote_storage.h"
#include "remote_input.h"

void applyRadioAddress(const uint8_t *address) {
  memcpy(radioAddress, address, sizeof(radioAddress));
  radio.setChannel(RADIO_CHANNEL);
  radio.openWritingPipe(radioAddress);
}

static void generateBindAddress() {
  uint32_t seed = analogRead(REMOTE_HALL_PIN) ^ (analogRead(REMOTE_BATTERY_PIN) << 10) ^ micros();
  randomSeed(seed);
  for (uint8_t i = 0; i < sizeof(pendingBindAddress); i++) {
    pendingBindAddress[i] = (uint8_t)random(0x20, 0x7F);
  }
  if (memcmp(pendingBindAddress, RADIO_BIND_ADDRESS, sizeof(pendingBindAddress)) == 0) {
    pendingBindAddress[0] ^= 0x5A;
  }
}

void startBinding() {
#if ENABLE_BINDING
  generateBindAddress();
  bindingMode = true;
  lastBindMs = 0;
#endif
}

void handleBinding() {
#if ENABLE_BINDING
  uint32_t now = millis();
  if (now - lastBindMs < RADIO_BIND_RETRY_MS) {
    return;
  }
  lastBindMs = now;

  RadioBindRequest request = {};
  request.magic = RADIO_BIND_MAGIC;
  request.version = RADIO_BIND_VERSION;
  memcpy(request.address, pendingBindAddress, sizeof(request.address));
  request.token = now;

  radio.setChannel(RADIO_BIND_CHANNEL);
  radio.openWritingPipe(RADIO_BIND_ADDRESS);

  bool ok = radio.write(&request, sizeof(request));
  if (ok && radio.isAckPayloadAvailable()) {
    uint8_t len = radio.getDynamicPayloadSize();
    if (len == sizeof(RadioBindAck)) {
      RadioBindAck ack = {};
      radio.read(&ack, sizeof(ack));
      if (ack.magic == RADIO_BIND_MAGIC && ack.version == RADIO_BIND_VERSION && ack.accepted) {
        memcpy(binding.address, ack.address, sizeof(binding.address));
        saveBinding();
        applyRadioAddress(binding.address);
        bindingMode = false;
        lastAckMs = now;
      }
    } else {
      uint8_t discard[32];
      radio.read(discard, len);
    }
  }
#endif
}

void setupRadio() {
  radio.begin();
  radio.setChannel(RADIO_CHANNEL);
  radio.setDataRate(RADIO_DATA_RATE);
  radio.setPALevel(RADIO_PA_LEVEL);
  radio.setRetries(RADIO_RETRY_DELAY, RADIO_RETRY_COUNT);
  radio.enableAckPayload();
  radio.enableDynamicPayloads();
  applyRadioAddress(radioAddress);
  radio.stopListening();
}

void sendCommand(int16_t throttle) {
  uint32_t now = millis();
  if (now - lastTxMs < RADIO_SEND_INTERVAL_MS) {
    return;
  }

  lastTxMs = now;
  command.seq++;
  command.throttle = throttle;
  command.flags = 0;
  if (lightsEnabled) command.flags |= RADIO_FLAG_LIGHTS;
  if (cruiseEnabled) command.flags |= RADIO_FLAG_CRUISE;
  if (tripResetRequested) command.flags |= RADIO_FLAG_TRIP_RESET;
  command.remote_batt_percent = remoteBatteryPercent();

  bool ok = radio.write(&command, sizeof(command));
  if (ok && radio.isAckPayloadAvailable()) {
    uint8_t len = radio.getDynamicPayloadSize();
    if (len == sizeof(telemetry)) {
      radio.read(&telemetry, sizeof(telemetry));
    } else {
      uint8_t discard[32];
      radio.read(discard, len);
    }
    lastAckMs = now;
  }

  connected = (now - lastAckMs) < (RADIO_FAILSAFE_MS * 2);
  tripResetRequested = false;
}

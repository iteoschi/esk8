#include "board_radio.h"
#include "board_utils.h"
#include "board_persist.h"

static bool bindingSafe() {
  return fabsf(wheelSpeedKph) <= BOARD_BINDING_SAFE_SPEED_KPH &&
         abs(appliedThrottle) <= BOARD_THROTTLE_DEADZONE;
}

static void applyRadioAddress(const uint8_t *address) {
  memcpy(radioAddress, address, sizeof(radioAddress));
  radioAddressValid = true;
  radio.stopListening();
#if ENABLE_BINDING
  radio.openReadingPipe(1, RADIO_BIND_ADDRESS);
#endif
  radio.openReadingPipe(2, radioAddress);
  radio.startListening();
}

static void handleBindRequest(const RadioBindRequest &request) {
  RadioBindAck ack = {};
  ack.magic = RADIO_BIND_MAGIC;
  ack.version = RADIO_BIND_VERSION;
  ack.board_id = boardId;
  ack.accepted = 0;
  memcpy(ack.address, radioAddress, sizeof(ack.address));

  if (request.magic == RADIO_BIND_MAGIC && request.version == RADIO_BIND_VERSION && bindingSafe()) {
    applyRadioAddress(request.address);
    memcpy(ack.address, radioAddress, sizeof(ack.address));
    ack.accepted = 1;
    persistBoardStorage(true);
  }

  radio.writeAckPayload(1, &ack, sizeof(ack));
}

void setupRadio() {
  SPI.begin(BOARD_SPI_SCK, BOARD_SPI_MISO, BOARD_SPI_MOSI);
  radio.begin();
  radio.setChannel(RADIO_CHANNEL);
  radio.setDataRate(RADIO_DATA_RATE);
  radio.setPALevel(RADIO_PA_LEVEL);
  radio.setRetries(RADIO_RETRY_DELAY, RADIO_RETRY_COUNT);
  radio.enableAckPayload();
  radio.enableDynamicPayloads();
#if ENABLE_BINDING
  radio.openReadingPipe(1, RADIO_BIND_ADDRESS);
#endif
  radio.openReadingPipe(2, radioAddress);
  radio.startListening();
}

void updateTelemetry() {
  RadioTelemetry local = {};

  local.seq = lastCommand.seq;
  local.speed_kph_x10 = (int16_t)(wheelSpeedKph * 10.0f);
  local.speed_vesc_kph_x10 = (int16_t)(vescSpeedKph * 10.0f);
  local.input_voltage_x10 = (uint16_t)(vescData.inpVoltage * 10.0f);
  local.current_x10 = (int16_t)(vescData.avgInputCurrent * 10.0f);
  local.motor_current_x10 = (int16_t)(vescData.avgMotorCurrent * 10.0f);
  local.temp_mos_c_x10 = (int16_t)(vescData.tempMos * 10.0f);
  local.temp_motor_c_x10 = (int16_t)(vescData.tempMotor * 10.0f);

  local.board_batt_percent = batteryPercent(vescData.inpVoltage);

  local.odometer_m = (uint32_t)(odometerM + 0.5f);
  local.trip_m = (uint32_t)(tripM + 0.5f);

  float tripKm = tripM / 1000.0f;
  if (tripKm > 0.05f) {
    whPerKm = energyWh / tripKm;
  } else {
    whPerKm = 0.0f;
  }

  if (whPerKm < 0.0f) {
    whPerKm = 0.0f;
  }

  local.wh_per_km_x10 = (uint16_t)(whPerKm * 10.0f);
  local.max_speed_kph_x10 = (uint16_t)(maxSpeedKph * 10.0f);
  local.amp_hours_x100 = (uint16_t)(vescData.ampHours * 100.0f);

  uint8_t flags = 0;
  if (lightsEnabled) flags |= TELEMETRY_FLAG_LIGHTS_ON;
  if (cruiseActive) flags |= TELEMETRY_FLAG_CRUISE_ACTIVE;
  if (vescOk) flags |= TELEMETRY_FLAG_VESC_OK;
#if ENABLE_WHEEL_HALL
  flags |= TELEMETRY_FLAG_HALL_OK;
#endif
  local.flags = flags;

  portENTER_CRITICAL(&telemetryMux);
  telemetry = local;
  portEXIT_CRITICAL(&telemetryMux);

  radio.writeAckPayload(2, &telemetry, sizeof(telemetry));
}

void updateRadio() {
  uint8_t pipeNum = 0;

  while (radio.available(&pipeNum)) {
    uint8_t payloadSize = radio.getDynamicPayloadSize();
    if (payloadSize == 0 || payloadSize > 32) {
      radio.flush_rx();
      continue;
    }

#if ENABLE_BINDING
    if (pipeNum == 1 && payloadSize == sizeof(RadioBindRequest)) {
      RadioBindRequest request = {};
      radio.read(&request, sizeof(request));
      handleBindRequest(request);
      continue;
    }
#endif

    if (payloadSize != sizeof(RadioCommand)) {
      uint8_t discard[32];
      radio.read(discard, payloadSize);
      continue;
    }

    RadioCommand incoming = {};
    radio.read(&incoming, sizeof(incoming));

    lastCommand = incoming;
    lastRadioMs = millis();

    lightsEnabled = (lastCommand.flags & RADIO_FLAG_LIGHTS) != 0;
    if (!(lastCommand.flags & RADIO_FLAG_CRUISE)) {
      cruiseActive = false;
    }

    if ((lastCommand.flags & RADIO_FLAG_TRIP_RESET) != 0 && lastCommand.seq != lastTripResetSeq) {
      lastTripResetSeq = lastCommand.seq;
      resetTripStats();
    }
  }
}

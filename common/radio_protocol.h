#pragma once

#include <Arduino.h>

#define RADIO_PROTOCOL_VERSION 1
#define RADIO_BIND_MAGIC 0xB5
#define RADIO_BIND_VERSION 1

enum RadioFlags : uint8_t {
  RADIO_FLAG_LIGHTS = 1 << 0,
  RADIO_FLAG_CRUISE = 1 << 1,
  RADIO_FLAG_TRIP_RESET = 1 << 2
};

enum TelemetryFlags : uint8_t {
  TELEMETRY_FLAG_LIGHTS_ON = 1 << 0,
  TELEMETRY_FLAG_CRUISE_ACTIVE = 1 << 1,
  TELEMETRY_FLAG_VESC_OK = 1 << 2,
  TELEMETRY_FLAG_HALL_OK = 1 << 3
};

struct RadioCommand {
  uint16_t seq;
  int16_t throttle;
  uint8_t flags;
  uint8_t remote_batt_percent;
} __attribute__((packed));

struct RadioTelemetry {
  uint16_t seq;
  int16_t speed_kph_x10;
  int16_t speed_vesc_kph_x10;
  uint8_t board_batt_percent;
  uint8_t flags;
  uint16_t input_voltage_x10;
  int16_t current_x10;
  int16_t motor_current_x10;
  int16_t temp_mos_c_x10;
  int16_t temp_motor_c_x10;
  uint32_t odometer_m;
  uint32_t trip_m;
  uint16_t wh_per_km_x10;
  uint16_t max_speed_kph_x10;
  uint16_t amp_hours_x100;
} __attribute__((packed));

static_assert(sizeof(RadioCommand) <= 32, "RadioCommand too large");
static_assert(sizeof(RadioTelemetry) <= 32, "RadioTelemetry too large");

struct RadioBindRequest {
  uint8_t magic;
  uint8_t version;
  uint8_t address[5];
  uint32_t token;
} __attribute__((packed));

struct RadioBindAck {
  uint8_t magic;
  uint8_t version;
  uint8_t accepted;
  uint8_t address[5];
  uint32_t board_id;
} __attribute__((packed));

static_assert(sizeof(RadioBindRequest) <= 32, "RadioBindRequest too large");
static_assert(sizeof(RadioBindAck) <= 32, "RadioBindAck too large");

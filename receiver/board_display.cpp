#include "board_display.h"

#if ENABLE_BOARD_OLED
static void drawBoardDisplay(const RadioTelemetry &localTelemetry) {
  char line[24];

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tf);

  int16_t speed = localTelemetry.speed_kph_x10;
  snprintf(line, sizeof(line), "SPD %d.%dkm", speed / 10, abs(speed % 10));
  u8g2.drawStr(0, 12, line);

  float packVoltage = localTelemetry.input_voltage_x10 / 10.0f;
  float cellVoltage = packVoltage / BOARD_BATTERY_CELL_COUNT;
  snprintf(line, sizeof(line), "CELL %.2fV", cellVoltage);
  u8g2.drawStr(0, 24, line);

  snprintf(line, sizeof(line), "PACK %.1fV", packVoltage);
  u8g2.drawStr(70, 24, line);

  int16_t mosTemp = localTelemetry.temp_mos_c_x10;
  int16_t motorTemp = localTelemetry.temp_motor_c_x10;
  snprintf(line, sizeof(line), "MOS %dC MOT %dC", mosTemp / 10, motorTemp / 10);
  u8g2.drawStr(0, 36, line);

  snprintf(line, sizeof(line), "WH/KM %d.%d AH %d.%d",
           localTelemetry.wh_per_km_x10 / 10,
           localTelemetry.wh_per_km_x10 % 10,
           localTelemetry.amp_hours_x100 / 100,
           localTelemetry.amp_hours_x100 % 100 / 10);
  u8g2.drawStr(0, 48, line);

  uint32_t meters = localTelemetry.trip_m;
  uint32_t km = meters / 1000;
  uint32_t dec = (meters % 1000) / 10;
  snprintf(line, sizeof(line), "TRIP %lu.%02lukm", (unsigned long)km, (unsigned long)dec);
  u8g2.drawStr(0, 60, line);

  u8g2.sendBuffer();
}

static void displayTask(void *parameter) {
  RadioTelemetry localTelemetry;

  for (;;) {
    portENTER_CRITICAL(&telemetryMux);
    localTelemetry = telemetry;
    portEXIT_CRITICAL(&telemetryMux);

    drawBoardDisplay(localTelemetry);

    vTaskDelay(pdMS_TO_TICKS(BOARD_DISPLAY_REFRESH_MS));
  }
}
#endif

void setupDisplay() {
#if ENABLE_BOARD_OLED
  Wire.begin(BOARD_OLED_SDA, BOARD_OLED_SCL);
  u8g2.setI2CAddress(BOARD_OLED_ADDR << 1);
  u8g2.begin();
  xTaskCreatePinnedToCore(displayTask, "board_display", 4096, nullptr, 1, nullptr, 1);
#endif
}

#include "board_vesc.h"
#include "board_utils.h"

void setupVesc() {
#if ENABLE_VESC_UART
  Serial2.begin(115200, SERIAL_8N1, BOARD_VESC_UART_RX, BOARD_VESC_UART_TX);
  vesc.setSerialPort(&Serial2);
#endif
}

void updateVesc() {
#if ENABLE_VESC_UART
  uint32_t now = millis();
  if (now - lastVescPollMs < 100) {
    return;
  }

  lastVescPollMs = now;
  vescOk = vesc.getVescValues();
  if (vescOk) {
    vescData = vesc.data;
    vescSpeedKph = erpmToKph(vescData.rpm);
  } else {
    memset(&vescData, 0, sizeof(vescData));
    vescSpeedKph = 0.0f;
  }

  if (lastEnergyMs == 0) {
    lastEnergyMs = now;
  }

  float dtHours = (now - lastEnergyMs) / 3600000.0f;
  lastEnergyMs = now;

  if (vescOk && dtHours > 0.0f) {
    float current = vescData.avgInputCurrent;
    if (current > 0.0f) {
      energyWh += vescData.inpVoltage * current * dtHours;
    }
  }
#endif
}

# Wiring Map

All connections are based on `common/config.h` defaults. Adjust pins there if your hardware differs.

Remote (Arduino Nano)
- nRF24L01
  - VCC -> 3.3V (do not use 5V)
  - GND -> GND
  - CE  -> D9 (`REMOTE_RF24_CE_PIN`)
  - CSN -> D10 (`REMOTE_RF24_CSN_PIN`)
  - SCK -> D13 (SPI SCK)
  - MOSI -> D11 (SPI MOSI)
  - MISO -> D12 (SPI MISO)
- OLED 128x32 I2C
  - VCC -> 3.3V or 5V (module dependent)
  - GND -> GND
  - SDA -> A4 (I2C SDA)
  - SCL -> A5 (I2C SCL)
- Hall throttle
  - Signal -> A0 (`REMOTE_HALL_PIN`)
  - VCC -> 5V
  - GND -> GND
- Remote battery sense
  - Divider output -> A1 (`REMOTE_BATTERY_PIN`)
  - Divider input -> battery +
  - Divider GND -> battery -
- Trigger (deadman switch)
  - One side -> D4 (`REMOTE_TRIGGER_PIN`)
  - Other side -> GND

Board (ESP32 NodeMCU)
- nRF24L01
  - VCC -> 3.3V (do not use 5V)
  - GND -> GND
  - CE  -> GPIO27 (`BOARD_RF24_CE_PIN`)
  - CSN -> GPIO5 (`BOARD_RF24_CSN_PIN`)
  - SCK -> GPIO18 (`BOARD_SPI_SCK`)
  - MOSI -> GPIO23 (`BOARD_SPI_MOSI`)
  - MISO -> GPIO19 (`BOARD_SPI_MISO`)
- OLED 128x64 I2C
  - VCC -> 3.3V or 5V (module dependent)
  - GND -> GND
  - SDA -> GPIO21 (`BOARD_OLED_SDA`)
  - SCL -> GPIO22 (`BOARD_OLED_SCL`)
- VESC UART
  - ESP32 RX (GPIO16 `BOARD_VESC_UART_RX`) -> VESC TX
  - ESP32 TX (GPIO17 `BOARD_VESC_UART_TX`) -> VESC RX
  - GND -> VESC GND
- Wheel hall sensor
  - Signal -> GPIO34 (`BOARD_WHEEL_HALL_PIN`)
  - VCC -> 3.3V
  - GND -> GND
- Lights
  - Headlight control -> GPIO25 (`BOARD_LIGHT_HEAD_PIN`) -> MOSFET gate
  - Brake light control -> GPIO26 (`BOARD_LIGHT_BRAKE_PIN`) -> MOSFET gate
  - Use proper MOSFETs and flyback protection for your light load

Notes
- Add a 10uF+ capacitor across nRF24 VCC/GND close to the module.
- Ensure all grounds are shared (remote battery ground, board battery ground, VESC ground).
- If the hall sensor output is open collector, enable pull-up via `BOARD_WHEEL_HALL_PULLUP`.

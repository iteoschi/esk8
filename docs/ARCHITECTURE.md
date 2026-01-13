# Architecture

This project is split into three layers.

1) Shared configuration and protocol
- `common/config.h`: compile-time toggles, pins, radio settings, and default parameters.
- `common/radio_protocol.h`: shared radio structs (commands, telemetry, binding).

2) Receiver (Board / ESP32)
- `receiver/receiver.ino`: startup + main loop wiring of modules.
- Modules:
  - `receiver/board_radio.*`: radio RX, ack telemetry, binding, trip reset.
  - `receiver/board_vesc.*`: VESC UART polling + energy integration.
  - `receiver/board_wheel.*`: wheel hall interrupt + speed/odometer.
  - `receiver/board_control.*`: cruise + failsafe ramp + current command.
  - `receiver/board_lights.*`: headlight + brake light control.
  - `receiver/board_display.*`: board OLED task.
  - `receiver/board_persist.*`: NVS persistence.
  - `receiver/board_utils.*`: math helpers.
  - `receiver/board_state.*`: shared globals and state.

3) Transmitter (Remote / Arduino Nano)
- `transmitter/transmitter.ino`: startup + main loop wiring of modules.
- Modules:
  - `transmitter/remote_input.*`: hall read + throttle mapping + ramp.
  - `transmitter/remote_radio.*`: radio TX + binding.
  - `transmitter/remote_ui.*`: pages + settings menu.
  - `transmitter/remote_storage.*`: EEPROM persistence.
  - `transmitter/remote_state.*`: shared globals and state.

Typical data flow
- Remote sends `RadioCommand` every `RADIO_SEND_INTERVAL_MS`.
- Board replies with `RadioTelemetry` via ack payload.
- Board applies failsafe if no command within `RADIO_FAILSAFE_MS`.

Hardware wiring
- See `docs/WIRING.md` for the pin map and connection notes.

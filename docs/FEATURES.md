# Features

Core features (remote + board)
- nRF24L01 bidirectional link with telemetry ack payloads.
- Failsafe: board ramps throttle to zero on radio timeout.
- Cruise control: holds target speed when enabled and throttle is neutral.
- Smart lights: headlight toggle + brake lights on negative current.

Remote features (Arduino Nano)
- Multi-page OLED UI: Ride, Stats, Energy, Config.
- EEPROM settings with delayed writes to reduce wear.
- Throttle deadzone, max throttle/brake limits, accel/brake ramp times.
- Hall calibration (min/center/max capture).
- Trip reset trigger and binding trigger.

Board features (ESP32)
- VESC UART telemetry: voltage, current, temp, amp-hours.
- Wheel hall speed with odometer + trip.
- Energy tracking for Wh/km.
- Board OLED diagnostics task (FreeRTOS).
- NVS persistence for odometer/trip/max speed/energy and radio address.

Radio binding
- Optional binding flow to generate a unique address per remote.
- Board accepts binding only when speed is low and throttle is neutral.

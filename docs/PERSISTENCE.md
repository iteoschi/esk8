# Persistence

Remote (Arduino Nano)
- Stored in EEPROM with CRC and delayed writes.
- Saved items: hall calibration, deadzone, throttle/brake limits, ramp times, range, display rotation, binding address.

Board (ESP32)
- Stored in NVS (Preferences).
- Saved items: odometer, trip meters, max speed, energy Wh, binding address.
- Writes are rate-limited by time and distance thresholds to reduce flash wear.

Reset behavior
- RESET TRIP clears trip distance, energy, and max speed.
- RESET ALL restores default remote settings (does not wipe board stats).

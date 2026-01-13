# Settings

Remote settings (Settings mode)
- THR MAX: Max forward throttle percent (20-100).
- BRK MAX: Max braking percent (20-100).
- DEADZONE: Hall deadzone in raw ADC units.
- ACCEL S: Time to ramp from 0 to 100% throttle (0-5s).
- BRAKE S: Time to ramp from 0 to 100% braking (0-5s).
- RANGE KM: Typical range used to estimate remaining distance.
- ROTATE: Display rotation for left/right hand use.
- CAL MIN: Capture hall min (press with throttle at full brake).
- CAL CTR: Capture hall center (press with throttle neutral).
- CAL MAX: Capture hall max (press with throttle full forward).
- RESET TRIP: Sends a one-shot trip reset to the board.
- BIND: Starts radio binding (pairing).
- RESET ALL: Restore default settings.

Config page (quick toggles)
- LIGHTS: Headlight on/off.
- CRUISE: Enable cruise control logic on board.

Notes
- Settings are saved to EEPROM after a short delay to reduce wear.
- Trip reset clears trip distance, Wh/km, energy, and max speed.

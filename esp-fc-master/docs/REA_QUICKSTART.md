# 3-Step Calibration (R / E / A) — like Daspilot Plugfly

Your board doesn't have physical buttons, but esp-fc has the exact same
three calibrations Daspilot's R/E/A buttons trigger — you just run them
as commands over USB (serial terminal or Betaflight Configurator's CLI
tab, 115200 baud). Same order, same purpose.

---

## R — Radio (receiver) check
esp-fc doesn't need a "learn my sticks" step the way some boards do —
receiver endpoints are set once in Betaflight Configurator's **Receiver**
tab. Move all sticks to their extremes and confirm the channel bars in
Configurator move smoothly end to end and center correctly. If not,
adjust `rc_min`/`rc_max`/`rc_mid` there. One-time, unless you change
transmitters.

## E — ESC calibration (props OFF)
This is the fix for the uneven-thrust tilt-on-arm issue — same root
cause Daspilot's E button fixes, same fix here:
```
escpwmcal max
```
Power on your ESCs now (all together, same battery connect) — each
should beep its "max learned" tune.
```
escpwmcal min
```
ESCs confirm min throttle / arm beeps.
```
escpwmcal stop
```
Refuses to run if armed — can't trigger by accident mid-flight.

## A — Accelerometer calibration
Place the frame level on a flat surface (use a small level tool, not by
eye). In Betaflight Configurator's **Setup** tab, click **Calibrate
Accelerometer**. Redo any time you remount the FC at a different angle.

Also run:
```
cal gyro
```
with the drone still, on the same flat surface.

---

## That's the whole sequence
R (check) → E (props off) → A (level) — same 3 steps as the Daspilot
board, just typed instead of pressed. Do this before every first flight
on a new build, or after any crash where the FC may have shifted.

After R/E/A: follow the bench → hand-held → low-hover test order and the
baseline PID in QUICKSTART_7IN_PWM.md before flying further.

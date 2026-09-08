# Quick Start: 7"+ Long-Range Build, PWM ESCs, ESP32 DEVKIT

This is a step-by-step setup for YOUR build. No firmware logic was changed —
these are configuration/calibration steps using features already built into
this esp-fc fork. Connect over USB with a serial terminal (or the CLI tab in
Betaflight Configurator) at 115200 baud and run the commands in order.

## PROPS OFF for steps 1-3. Do not skip this.

### Step 1 — ESC throttle-endpoint sync (fixes uneven-thrust tilt on arm)
This is the #1 cause of a quad tilting right after liftoff on plain PWM ESCs:
one ESC "learns" a slightly different 1000-2000us range than the others, so
at the same stick position it makes less thrust. This fork has a built-in
calibration sequence for it:

```
escpwmcal max
```
FC now holds 2000us on every motor output. Power the ESCs on now (all at
once, same battery connection) — each should beep its "max throttle learned"
tune.

```
escpwmcal min
```
FC drops to 1000us. ESCs confirm min throttle / play arming beeps.

```
escpwmcal stop
```
Returns outputs to normal disarmed value.

This refuses to run (or aborts) if the craft is armed — you can't
accidentally trigger it mid-flight.

### Step 2 — Gyro calibration
Place the drone on a still, flat surface.
```
cal gyro
```

### Step 3 — Accelerometer level trim
In Betaflight Configurator: Setup tab -> place the frame level using a small
spirit level (not by eye) -> click "Calibrate Accelerometer". This sets the
FC's zero-tilt reference to match how it's actually mounted. Redo this any
time you change the flight controller's mounting angle in the frame.

### Step 4 — Confirm motor order & direction (props still off)
Use Configurator's Motor tab to spin each motor individually at low
throttle and confirm it's in the correct physical position and spinning the
correct direction for your prop layout.

### Step 5 — Set output protocol and idle
```
set output_motor_protocol pwm
set output_min_command 1000
set output_min_throttle 1070
save
```
`output_min_throttle` is the signal all 4 motors spin up together from on
arm — 1070 is a safe conservative starting idle for PWM. Raise it slightly
(e.g. 1080-1100) if motors don't all start spinning at the exact same
moment on arm, after Step 1 is done.

### Step 6 — Baseline PID for a 7"+ long-range frame
These are a conservative starting point for this frame class — heavier and
more inertia than a 5" racer, so gains are a bit softer than typical racing
defaults. Expect to fine-tune from here, not treat this as final:

```
set pid_roll_p 38
set pid_roll_i 80
set pid_roll_d 26
set pid_roll_f 72
set pid_pitch_p 42
set pid_pitch_i 85
set pid_pitch_d 28
set pid_pitch_f 76
set pid_yaw_p 40
set pid_yaw_i 80
set pid_yaw_d 0
set pid_yaw_f 72
set pid_level_p 40
set pid_level_i 0
set pid_level_d 0
save
```

## Test order (do not skip ahead)
1. Props off, armed on bench: confirm all 4 motors spin up together at the
   same stick position (this validates Step 1).
2. Props on, drone restrained/held firmly by hand or strapped down: arm at
   low throttle, confirm no immediate tilt or twitch.
3. Low, cautious hover over a soft surface (grass), finger over the
   disarm switch the whole time.
4. Only after a clean hover: start pushing the flight envelope, and adjust
   PID in small increments (5-10% at a time) from here if you still see
   wobble on a specific axis.

## What this does NOT do
This is calibration + a sane starting tune, not a fully automatic in-flight
tuning system. Continuous/automatic re-calibration *while flying* is not
something this firmware does (and isn't something you want — accel/gyro
calibration must happen on a still, known-level craft on the ground; doing
it while flying would corrupt the reference instead of fixing tilt). If
you still see wobble after steps 1-6 and a clean hover, it's a genuine PID
tuning question at that point — happy to help interpret blackbox logs.

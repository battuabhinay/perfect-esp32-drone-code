# How esp-fc works

esp-fc is the brain of your drone. It continuously senses movement, calculates
corrections, and adjusts the individual motor speeds to keep the drone
stable - the same job Betaflight does on STM32 hardware, just running on an
ESP32/ESP8266/RP2040 instead. The code for every block below lives in
`lib/Espfc/src/`.

## Basic working

```
        REMOTE / TRANSMITTER
                 |
                 v
          Receiver --> Input.cpp
                 |
                 v
        +--------------------------+
        |      esp-fc (Model)      |
        |                          |
        |  GyroSensor + AccelSensor|
        |            |             |
        |            v             |
        |     Control::Controller  |
        |        (PID)             |
        |            |             |
        |            v             |
        |     Output::Mixer        |
        +--------------------------+
          |     |     |     |
          v     v     v     v
         ESC   ESC   ESC   ESC     <- EscDriver (PWM / Oneshot / DSHOT)
          |     |     |     |
          v     v     v     v
         M1    M2    M3    M4
```

Source files for each block:

| Block | File |
|---|---|
| Receiver input | `lib/Espfc/src/Input.cpp` |
| Gyro | `lib/Espfc/src/Sensor/GyroSensor.cpp` |
| Accelerometer | `lib/Espfc/src/Sensor/AccelSensor.cpp` |
| PID controller | `lib/Espfc/src/Control/Controller.cpp` |
| Mixer | `lib/Espfc/src/Output/Mixer.cpp` |
| ESC output | `lib/EscDriver/src/*` |
| Loop that ties it together | `lib/Espfc/src/Espfc.cpp` (`Espfc::update()`) |

## 1. Gyroscope measures movement

`GyroSensor::read()` / `::filter()` pull raw rotation rate off the IMU (roll,
pitch, yaw angular velocity), filter it, and store it in
`model.state.gyro.adc`. If the drone starts rotating, this is what notices
first - every loop iteration, as fast as `gyro_sync_denom` allows (typically
1-8 kHz).

The accelerometer (`AccelSensor.cpp`) separately measures linear
acceleration/gravity direction, which feeds attitude (angle) estimation for
self-level modes.

## 2. Desired vs actual movement

When you move a stick, `Input.cpp` turns that into a target rate/angle in
`model.state.input.ch[]`. `Controller::update()` compares that against what
the gyro is actually reporting:

```
error = desired_rate - actual_rate
```

This happens per-axis (roll, pitch, yaw) in `Controller::innerLoop()`
(`lib/Espfc/src/Control/Controller.cpp`).

## 3. PID calculates the correction

The actual PID math lives in `lib/Espfc/src/Control/Pid.h`. Same three terms
as any flight controller:

- **P** (Proportional) - reacts to the error right now.
- **I** (Integral) - reacts to error that's built up over time (steady
  drift/wind).
- **D** (Derivative) - damps how fast the error is changing, to stop
  overshoot/oscillation.

Tune these with the CLI (`set roll_p = ...`, `pitch_i`, `yaw_d`, etc., see
`docs/cli.md`). Too much P is the classic cause of fast oscillation/shaking;
too much I causes slow wobble/bounce-back after a hard stick input.

## 4. Mixer decides each motor's speed

`Output::Mixer::updateMixer()` (`lib/Espfc/src/Output/Mixer.cpp`) takes:

```
throttle + roll_pid + pitch_pid + yaw_pid
```

and turns that into one output value per motor, using the mixer table for
your frame (`lib/Espfc/src/Output/Mixers.h`). For a standard X-frame quad
that's `FC_MIXER_QUADX` - pushing pitch forward doesn't raise all four motors
equally, it raises the rear pair and/or lowers the front pair so the frame
tips forward. Yaw twists diagonal motor pairs against each other; roll does
the same left/right.

This is also where `output.minThrottle`/`output.maxThrottle`, motor limits,
and airmode logic are applied before the numbers go out.

## 5. ESC controls the motors

`Mixer::writeOutput()` hands the final per-motor value to `EscDriver`
(`lib/EscDriver/src/`), which encodes it as whatever protocol you configured
(`output_protocol`: PWM, ONESHOT125, MULTISHOT, BRUSHED, DSHOT150/300/600)
and sends it to that motor's ESC. The ESC converts that signal into the
actual current/RPM the motor spins at.

## The full loop

```
Stick -> Receiver -> Input.cpp -> Controller (gyro + PID) -> Mixer -> EscDriver -> ESC -> Motor -> drone moves -> Gyro measures again
```

This repeats continuously at the configured gyro/PID loop rate
(`Espfc::update()` in `lib/Espfc/src/Espfc.cpp` - gated by
`model.state.gyro.timer`), which is the actual flight-control loop rate for
this firmware.

### Where the calibration/ESC-cal features added in this build fit in

- Gyro/accel auto-calibration (`GyroSensor.cpp` / `AccelSensor.cpp`) sits
  at step 1, before anything else runs - it keeps the "actual movement"
  measurement honest over time.
- `escpwmcal` (CLI command, implemented in `Mixer.cpp`/`Cli.cpp`) bypasses
  steps 2-4 entirely and drives step 5 directly with a fixed pulse, purely to
  teach analog ESCs matching endpoints. It's not part of the normal flight
  loop.

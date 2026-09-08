# Betaflight Feature Port Progress

## Phase 1: Core Flight Features
- [x] 1. Thrust Linearization - Compensate for non-linear throttle-to-thrust
- [x] 2. Dynamic Idle (dyn_idle) - RPM-based idle control
- [x] 3. Feedforward Averaging + Jitter Filter

## Phase 2: Flight Modes & Safety
- [x] 4. Horizon Mode - Mix of angle and acro
- [x] 5. Crash Flip - Auto-flip after crash
- [x] 6. Runaway Takeoff Prevention
- [x] 7. Launch Control - Controlled motor spool-up

## Phase 3: PID & Tuning
- [x] 8. Simplified PID Gains (Betaflight 4.3+)
- [x] 9. Acro Trainer - Training mode with angle limits

## Phase 4: Config & Integration
- [x] 10. Update ModelConfig with new fields
- [x] 11. Update ModelState with new state fields
- [x] 12. Update MSP commands for new features
- [x] 13. Update CLI with new parameters


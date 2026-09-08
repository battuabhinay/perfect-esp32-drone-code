#pragma once

#include "Model.h"
#include "Utils/Math.hpp"
#include <algorithm>
#include <cmath>

namespace Espfc::Control {

class RunawayTakeoff
{
public:
  RunawayTakeoff(Model& model): _model(model) {}

  void begin()
  {
    _model.state.mode.runawayTakeoffActive = false;
    _model.state.mode.runawayTakeoffStartTime = 0;
    _model.state.mode.runawayTakeoffAngle = 0;
  }

  int update()
  {
    if(!_model.config.runawayTakeoff.mode) return 0;
    if(!_model.isModeActive(MODE_ARMED)) return 0;

    const uint32_t now = millis();

    // Monitor gyro activity when throttle is applied but quad is not moving as expected
    const float gyroMagnitude = std::sqrt(
      _model.state.gyro.adc[AXIS_ROLL] * _model.state.gyro.adc[AXIS_ROLL] +
      _model.state.gyro.adc[AXIS_PITCH] * _model.state.gyro.adc[AXIS_PITCH] +
      _model.state.gyro.adc[AXIS_YAW] * _model.state.gyro.adc[AXIS_YAW]
    );

    const float gyroDeg = Utils::toDeg(gyroMagnitude);
    const float thresholdRad = Utils::toRad(_model.config.runawayTakeoff.threshold);
    const uint32_t delayMs = _model.config.runawayTakeoff.delayMs;

    if(gyroDeg > thresholdRad && _model.state.input.us[AXIS_THRUST] > _model.config.input.minCheck)
    {
      if(!_model.state.mode.runawayTakeoffActive)
      {
        _model.state.mode.runawayTakeoffActive = true;
        _model.state.mode.runawayTakeoffStartTime = now;
        _model.state.mode.runawayTakeoffAngle = gyroDeg;
      }

      if(now - _model.state.mode.runawayTakeoffStartTime > delayMs)
      {
        // Runaway detected - disarm
        _model.disarm(DISARM_REASON_RUNAWAY_TAKEOFF);
        _model.state.mode.runawayTakeoffActive = false;
        return 1;
      }
    }
    else
    {
      _model.state.mode.runawayTakeoffActive = false;
    }

    return 0;
  }

private:
  Model& _model;
};

}


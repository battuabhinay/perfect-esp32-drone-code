#pragma once

#include "Model.h"
#include "Utils/Math.hpp"
#include <algorithm>
#include <cmath>

namespace Espfc::Control {

class AcroTrainer
{
public:
  AcroTrainer(Model& model): _model(model) {}

  void begin()
  {
    _model.state.mode.acroTrainerActive = false;
    _model.state.mode.acroTrainerLastRecovery = 0;
  }

  float update(int axis, float setpoint)
  {
    if(!_model.config.acroTrainer.angleLimit) return setpoint;
    if(!_model.isModeActive(MODE_ACRO_TRAINER))
    {
      if(_model.config.acroTrainer.mode == 1) return setpoint; // switch only
    }

    if(axis >= AXIS_COUNT_RPY) return setpoint; // only R/P/Y

    const float angleLimitRad = Utils::toRad(_model.config.acroTrainer.angleLimit);
    const float currentAngle = _model.state.attitude.euler[axis];
    const float gain = _model.config.acroTrainer.gain * 0.01f;

    // Check if approaching angle limit
    if(std::abs(currentAngle) > angleLimitRad * 0.8f)
    {
      // Calculate recovery rate to push back within limits
      const float overshoot = (std::abs(currentAngle) - angleLimitRad * 0.8f) / (angleLimitRad * 0.2f);
      const float recoveryRate = Utils::toRad(180.0f) * overshoot * overshoot * gain;

      // Apply recovery in opposite direction
      if(currentAngle > 0)
      {
        setpoint = std::min(setpoint, -recoveryRate);
      }
      else
      {
        setpoint = std::max(setpoint, recoveryRate);
      }

      _model.state.mode.acroTrainerActive = true;
      _model.state.mode.acroTrainerLastRecovery = millis();
    }
    else
    {
      // Deactivate after recovery
      if(_model.state.mode.acroTrainerActive)
      {
        uint32_t elapsed = millis() - _model.state.mode.acroTrainerLastRecovery;
        if(elapsed > 500) _model.state.mode.acroTrainerActive = false;
      }
    }

    return setpoint;
  }

private:
  Model& _model;
};

}


#pragma once

#include "Model.h"
#include "Utils/Math.hpp"
#include <algorithm>

namespace Espfc::Control {

class HorizonMode
{
public:
  HorizonMode(Model& model): _model(model) {}

  void begin()
  {
    _model.state.mode.horizonStrength = 1.0f;
    _model.state.mode.horizonLastStickTime = 0;
  }

  float update(int axis, float input, float angleRate)
  {
    if(!_model.isModeActive(MODE_HORIZON)) return angleRate;

    const HorizonConfig& config = _model.config.horizon;
    const float stickMagnitude = std::max(std::abs(_model.state.input.ch[AXIS_ROLL]),
                                          std::abs(_model.state.input.ch[AXIS_PITCH]));

    if(stickMagnitude > 0.1f)
    {
      _model.state.mode.horizonLastStickTime = millis();
    }

    float targetStrength = 1.0f;
    const float angleLimitRad = Utils::toRad(config.limitDegrees);
    const float currentAngle = std::max(std::abs(_model.state.attitude.euler[AXIS_ROLL]),
                                         std::abs(_model.state.attitude.euler[AXIS_PITCH]));

    if(currentAngle > angleLimitRad * 0.5f)
    {
      targetStrength = 0.0f;
    }
    else if(stickMagnitude > 0.8f)
    {
      targetStrength = Utils::map(stickMagnitude, 0.8f, 1.0f, 1.0f, 0.2f);
    }

    const uint32_t now = millis();
    const uint32_t timeSinceStick = now - _model.state.mode.horizonLastStickTime;

    if(targetStrength < _model.state.mode.horizonStrength)
    {
      _model.state.mode.horizonStrength = std::max(targetStrength,
        _model.state.mode.horizonStrength - 0.1f);
    }
    else if(timeSinceStick > config.delayMs)
    {
      _model.state.mode.horizonStrength = std::min(targetStrength,
        _model.state.mode.horizonStrength + 0.02f);
    }

    if(axis == AXIS_ROLL || axis == AXIS_PITCH)
    {
      const float levelSetpoint = -_model.state.attitude.euler[axis] *
                                  (config.levelStrength * 0.01f) * 5.0f;
      return angleRate * (1.0f - _model.state.mode.horizonStrength) +
             levelSetpoint * _model.state.mode.horizonStrength;
    }

    return angleRate;
  }

private:
  Model& _model;
};

}


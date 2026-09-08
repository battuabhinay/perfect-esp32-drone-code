#pragma once

#include "Model.h"
#include "Utils/Math.hpp"
#include <algorithm>
#include <cmath>

namespace Espfc::Control {

class LaunchControl
{
public:
  LaunchControl(Model& model): _model(model) {}

  void begin()
  {
    _model.state.mode.launchControlActive = false;
    _model.state.mode.launchControlStartTime = 0;
    _model.state.mode.launchControlThrottle = 0;
  }

  int update()
  {
    if(!_model.config.launchControl.mode) return 0;
    if(!_model.isModeActive(MODE_LAUNCH_CONTROL)) return 0;
    if(!_model.isModeActive(MODE_ARMED)) return 0;

    const uint32_t now = millis();
    const LaunchControlConfig& config = _model.config.launchControl;

    if(!_model.state.mode.launchControlActive)
    {
      // Check trigger conditions
      bool triggerThrottle = _model.state.input.us[AXIS_THRUST] > 1020;
      float angle = std::max(std::abs(_model.state.attitude.euler[AXIS_ROLL]),
                              std::abs(_model.state.attitude.euler[AXIS_PITCH]));
      bool triggerAngle = Utils::toDeg(angle) > 10.0f;

      bool shouldActivate = false;
      switch(config.triggerMode)
      {
        case 0: shouldActivate = triggerThrottle && triggerAngle; break;
        case 1: shouldActivate = triggerThrottle; break;
        case 2: shouldActivate = triggerAngle; break;
        default: shouldActivate = triggerThrottle && triggerAngle; break;
      }

      if(shouldActivate)
      {
        _model.state.mode.launchControlActive = true;
        _model.state.mode.launchControlStartTime = now;
        _model.state.mode.launchControlThrottle = _model.state.mixer.minThrottle;
      }
    }

    if(_model.state.mode.launchControlActive)
    {
      uint32_t elapsed = now - _model.state.mode.launchControlStartTime;
      uint32_t rampTimeMs = config.throttleRampTime * 1000;

      if(elapsed < rampTimeMs)
      {
        // Ramp up throttle
        float progress = (float)elapsed / (float)rampTimeMs;
        float rampFactor = Utils::map(progress, 0.0f, 1.0f, 0.0f, 1.0f);
        rampFactor = std::min(rampFactor * (config.throttleRamp * 0.01f + 0.8f), 1.0f);

        const float targetThrottle = _model.state.input.ch[AXIS_THRUST];
        _model.state.mode.launchControlThrottle = Utils::map3(
          rampFactor, 0.0f, 0.5f, 1.0f,
          _model.state.mixer.minThrottle * (1.0f / 1000.0f),
          _model.state.mixer.minThrottle * (2.0f / 1000.0f),
          targetThrottle
        );

        // Override throttle output
        _model.state.setpoint.rate[AXIS_THRUST] = _model.state.mode.launchControlThrottle;

        // Limit angle for stable launch
        if(config.angleLimit < 180)
        {
          float angleLimitRad = Utils::toRad(config.angleLimit);
          for(size_t i = 0; i < AXIS_COUNT_RP; i++)
          {
            if(std::abs(_model.state.attitude.euler[i]) > angleLimitRad)
            {
              _model.state.setpoint.rate[i] = 0;
            }
          }
        }
      }
      else
      {
        // Launch phase complete, return to normal control
        _model.state.mode.launchControlActive = false;
      }

      return 1;
    }

    return 0;
  }

private:
  Model& _model;
};

}


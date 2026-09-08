#pragma once

#include "Model.h"
#include "Utils/Math.hpp"
#include <algorithm>
#include <cmath>

namespace Espfc::Control {

class CrashFlip
{
public:
  CrashFlip(Model& model): _model(model) {}

  void begin()
  {
    _model.state.mode.crashFlipActive = false;
    _model.state.mode.crashFlipPhase = 0;
    _model.state.mode.crashFlipStartTime = 0;
  }

  int update()
  {
    if(!_model.config.crashFlip.mode) return 0;
    if(!_model.isModeActive(MODE_CRASH_FLIP)) return 0;

    uint32_t now = millis();

    switch(_model.state.mode.crashFlipPhase)
    {
      case 0: // Idle - waiting for crash detection
        if(_model.isModeActive(MODE_ARMED) && !_model.areMotorsRunning())
        {
          _model.state.mode.crashFlipPhase = 1;
          _model.state.mode.crashFlipStartTime = now;
        }
        break;

      case 1: // Start flip - disarm then re-arm with flip
        if(now - _model.state.mode.crashFlipStartTime > 200)
        {
          _model.state.mode.crashFlipPhase = 2;
          _model.state.mode.crashFlipStartTime = now;
        }
        break;

      case 2: // Flip phase - apply flip power
        {
          uint32_t elapsed = now - _model.state.mode.crashFlipStartTime;
          float flipPower = _model.config.crashFlip.power * 0.01f;

          if(elapsed < 1000)
          {
            // Apply pitch flip (or roll based on input)
            float stickPitch = _model.state.input.ch[AXIS_PITCH];
            float stickRoll = _model.state.input.ch[AXIS_ROLL];

            _model.state.output.ch[AXIS_PITCH] = stickPitch * flipPower;
            _model.state.output.ch[AXIS_ROLL] = stickRoll * flipPower;
            _model.state.output.ch[AXIS_YAW] = 0;
            _model.state.output.ch[AXIS_THRUST] = flipPower;
          }
          else
          {
            _model.state.mode.crashFlipPhase = 3;
          }
        }
        break;

      case 3: // Done
      default:
        break;
    }

    return 1;
  }

  bool isActive() const
  {
    return _model.state.mode.crashFlipActive;
  }

private:
  Model& _model;
};

}


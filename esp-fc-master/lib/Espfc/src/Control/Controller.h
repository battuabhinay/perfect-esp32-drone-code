#pragma once

#include "Control/Altitude.hpp"
#include "Control/Rates.h"
#include "Control/HorizonMode.hpp"
#include "Control/CrashFlip.hpp"
#include "Control/RunawayTakeoff.hpp"
#include "Control/LaunchControl.hpp"
#include "Control/AcroTrainer.hpp"
#include "Model.h"

namespace Espfc::Control {

class Controller
{
public:
  Controller(Model& model);
  int begin();
  int update();

  void outerLoopRobot();
  void innerLoopRobot();
  void outerLoop();
  void innerLoop();

  inline float getTpaFactor() const;
  inline void resetIterm();
  float calculateSetpointRate(int axis, float input) const;
  float calcualteAltHoldSetpoint() const;

  // Betaflight ported features
  float applyThrustLinearization(float thrust) const;
  float applyDynamicIdle(int axis, float output) const;
  float applyFeedforwardAveraging(int axis, float ff);
  float applyFeedforwardJitter(int axis, float ff);
  float applySimplifiedPids(int axis, float pidSum) const;

private:
  void beginAltHold();
  void beginInnerLoop(size_t axis);
  void beginOuterLoop(size_t axis);

  Model& _model;
  Rates _rates;
  Utils::Filter _speedFilter;

  // Betaflight ported features
  HorizonMode _horizonMode;
  CrashFlip _crashFlip;
  RunawayTakeoff _runawayTakeoff;
  LaunchControl _launchControl;
  AcroTrainer _acroTrainer;

  // Feedforward state
  float _ffHistory[3][8];  // per-axis feedforward history for averaging
  uint8_t _ffIndex[3];
  float _ffPrev[3];
};

} // namespace Espfc::Control

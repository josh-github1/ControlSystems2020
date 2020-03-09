#pragma once

#include "L3_Application/Robotics/Common/MagneticEncoder.hpp"
#include "L3_Application/Robotics/Common/Drv8801.hpp"
#include "utility/units.hpp"

namespace sjsu
{
namespace robotics
{
class ChiHaiServo
{
 private:
  static constexpr float kResolution = 0.001f;
  bool ApproximatelyEqual(units::angle::degree_t expected,
                          units::angle::degree_t actual,
                          float resolution)
  {
    return (-resolution < (actual.to<float>() - expected.to<float>())) &&
           ((actual.to<float>() - expected.to<float>()) < resolution);
  }

 public:
  explicit constexpr ChiHaiServo(
      sjsu::robotics::MagneticEncoder magnetic_encoder,
      sjsu::robotics::Drv8801 drv)
      : magnetic_encoder_(magnetic_encoder), drv_(drv)
  {
  }
  void Initialize()
  {
    magnetic_encoder_.Initialize();
    drv_.Initialize();
  }
  void SetAngle(units::angle::degree_t angle)
  {
    units::angle::degree_t current_degree = magnetic_encoder_.GetAngle();
    // do some math to figure out whether we should turn clockwise or counter
    // clockwise to get desired angle faster
    if (current_degree - angle > 0_deg)
    {
      // haha do math here to see if we should go forward or back
    }
    // can i use something else than a while loop here?
    while (!ApproximatelyEqual(current_degree, angle, kResolution))
    {
      drv_.TurnBackward();
    }
    drv_.Stop();
  }
  void Stop()
  {
    drv_.Stop();
  }

 private:
  sjsu::robotics::MagneticEncoder magnetic_encoder_;
  sjsu::robotics::Drv8801 drv_;
};
}  // namespace robotics
}  // namespace sjsu

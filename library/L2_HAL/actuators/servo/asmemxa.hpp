#pragma once
#include "L2_HAL/actuators/servo/servo.hpp"

namespace sjsu
{
class asmemxa : public Servo
{
 public:
  explicit constexpr asmemxa(const Pwm & pwm) : Servo(pwm) {}

  void Initialize(
      units::frequency::hertz_t frequency = 50_kHz) override
  {
    Servo::Initialize(frequency);
    Servo::SetPulseBounds(500us, 2500us);
    Servo::SetAngleBounds(-90_deg, 90_deg);
  }
};
}  // namespace sjsu
#pragma once

#include "utility/log.hpp"
#include "L1_Peripheral/pwm.hpp"
#include "L1_Peripheral/gpio.hpp"

namespace sjsu
{
namespace robotics
{
class HubMotor
{
 public:
  static constexpr float kPositiveSpeedChange = 0.05f;
  static constexpr float kNegativeSpeedChange = -0.05f;
  enum Direction : uint8_t
  {
    kForward  = 0,
    kBackward = 1
  };
  explicit constexpr HubMotor(sjsu::Pwm & brake, sjsu::Gpio & direction_pin)
      : brake_(brake), direction_pin_(direction_pin)
  {
  }

  void Initialize() const
  {
    brake_.Initialize(20_kHz);
    direction_pin_.SetAsOutput();
  }

  void SetDirection(Direction direction)
  {
    if (direction == Direction::kForward)
    {
      LOG_INFO("Forward time");
      direction_pin_.SetHigh();
    }
    else
    {
      LOG_INFO("Back time");
      direction_pin_.SetLow();
    }
  }

  // used for testing purposes only. e.g. does the board turn the motor?
  void Start() const
  {
    for (float i = 0.0f; i <= 1.0f; i += kPositiveSpeedChange)
    {
      brake_.SetDutyCycle(i);
    }
  }

  void Stop() const
  {
    for (float i = 0.0f; i <= 1.0f; i += kNegativeSpeedChange)
    {
      brake_.SetDutyCycle(i);
    }
  }

  void SetSpeed(float duty_cycle) const
  {
    float current_duty_cycle = brake_.GetDutyCycle();
    float speed_change = current_duty_cycle > duty_cycle ? kNegativeSpeedChange
                                                         : kPositiveSpeedChange;
    for (float i = 0.0f; i <= 1.0f; i += speed_change)
    {
      brake_.SetDutyCycle(i);
    }
  }

  // Not sure about this function. Will need to test it out in with smruthi
  void SetSpeedAndDirection(float duty_cycle, Direction direction) const
  {
    if (direction == Direction::kForward)
    {
      direction_pin_.SetHigh();
    }
    else
    {
      direction_pin_.SetLow();
    }
    SetSpeed(duty_cycle);
  }

 private:
  sjsu::Pwm & brake_;
  sjsu::Gpio & direction_pin_;
};
}  // namespace robotics
}  // namespace sjsu

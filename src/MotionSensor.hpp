#ifndef MOTION_SENSOR_HPP
#define MOTION_SENSOR_HPP

#include "stm32f103x6.h"

class MotionSensor{
  private:
    GPIO_TypeDef* port;
    uint16_t pinMask;

  public:
    MotionSensor(GPIO_TypeDef*, uint16_t);
    bool isTriggered() const;
};

#endif
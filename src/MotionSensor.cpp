#include "MotionSensor.hpp"

MotionSensor::MotionSensor(GPIO_TypeDef* port, uint16_t pinMask) : port(port), pinMask(pinMask){}

bool MotionSensor::isTriggered() const{
  return (port->IDR & pinMask) != 0;
}
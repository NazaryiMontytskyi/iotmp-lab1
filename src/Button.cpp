#include "Button.hpp"

Button::Button(GPIO_TypeDef* port, uint16_t pinMask, bool activeLow) :
port(port), pinMask(pinMask), activeLow(activeLow) {}

bool Button::isPressed() const{
  bool pinState = (port->IDR & pinMask) != 0;
  return activeLow ? !pinState : pinState;
}
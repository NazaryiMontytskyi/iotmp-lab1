#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "stm32f103x6.h"

class Button{
  private:
   GPIO_TypeDef* port;
   uint16_t pinMask;
   bool activeLow;

  public:
    Button(GPIO_TypeDef*, uint16_t, bool activeLow = true);

    bool isPressed() const;
};

#endif
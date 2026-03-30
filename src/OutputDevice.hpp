#ifndef OUTPUT_DEVICE_HPP
#define OUTPUT_DEVICE_HPP

#include "stm32f103x6.h"

class OutputDevice{
  private:
    GPIO_TypeDef* port;
    uint32_t setMask;
    uint32_t resetMask;

  public:
    OutputDevice(GPIO_TypeDef* port, uint32_t pinMask);

    void on();
    void off();
    void set(bool state);
};

#endif
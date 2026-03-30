#ifndef RGB_LED_HPP
#define RGB_LED_HPP

#include "OutputDevice.hpp"

class RgbLed{
  private:
    OutputDevice redPin;
    OutputDevice greenPin;
    OutputDevice bluePin;

    public:
      RgbLed(GPIO_TypeDef* rPort, uint16_t rPin,
      GPIO_TypeDef* gPort, uint16_t gPin,
      GPIO_TypeDef* bPort, uint16_t bPin);

      void setColor(bool, bool, bool);
      void turnOff();
};

#endif
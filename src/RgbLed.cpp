#include "RgbLed.hpp"

RgbLed::RgbLed(GPIO_TypeDef* rPort, uint16_t rPin, GPIO_TypeDef* gPort, uint16_t gPin, GPIO_TypeDef* bPort, uint16_t bPin, bool activeHigh):
  redPin(rPort, rPin, activeHigh), greenPin(gPort, gPin, activeHigh), bluePin(bPort, bPin, activeHigh) {}

void RgbLed::setColor(bool red, bool green, bool blue){
  redPin.set(red);
  greenPin.set(green);
  bluePin.set(blue);
}

void RgbLed::turnOff(){
  setColor(false, false, false);
}

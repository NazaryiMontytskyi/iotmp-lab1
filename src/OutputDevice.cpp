#include "OutputDevice.hpp"

OutputDevice::OutputDevice(GPIO_TypeDef* port, uint32_t pinMask, bool activeHigh) :
port(port), setMask(pinMask), resetMask(pinMask), activeHigh(activeHigh){}

void OutputDevice::on(){
  if(activeHigh){
    port->BSRR = setMask;
  } else{
    port->BRR = resetMask;
  }
}

void OutputDevice::off(){
  if(activeHigh){
    port->BRR = resetMask;
  } else{
    port->BSRR = setMask;
  }
}

void OutputDevice::set(bool state){
  if(state){
    on();
  } else{
    off();
  }
}

#include "OutputDevice.hpp"

OutputDevice::OutputDevice(GPIO_TypeDef* port, uint32_t pinMask) : port(port), setMask(pinMask), resetMask(pinMask){}

void OutputDevice::on(){
  port->BSRR = setMask;
}

void OutputDevice::off(){
  port->BRR = resetMask;
}

void OutputDevice::set(bool state){
  if(state){
    on();
  } else{
    off();
  }
}
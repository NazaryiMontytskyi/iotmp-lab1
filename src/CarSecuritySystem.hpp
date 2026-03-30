#ifndef CAR_SECURITY_SYSTEM_HPP
#define CAR_SECURITY_SYSTEM_HPP

#include "stm32f103x6.h"
#include "Button.hpp"
#include "MotionSensor.hpp"
#include "RgbLed.hpp"
#include "OutputDevice.hpp"

constexpr uint32_t DEBOUNCE_DELAY_MS = 40;
constexpr uint32_t DOUBLE_CLICK_WINDOW_MS = 400;

constexpr uint32_t RELAY_PULSE_MS = 300;
constexpr uint32_t BUZZER_OPEN_MS = 150;
constexpr uint32_t BUZZER_CLOSE_MS = 400;
constexpr uint32_t WARNING_DURATION_MS = 5000;

class CarSecuritySystem {
public:
    enum State { OPENED, CLOSED, WARNING, ALARM, TRUNK_ACTION };

    CarSecuritySystem();

    void init();
    void update();
    void handleExtiInterrupt(uint16_t pin);
    void processDebouncedInput();

    uint32_t millis() const { return systemTicks; }
    void incrementTick() { systemTicks++; }

private:
    State currentState;
    State previousState;
    uint32_t stateStartTime;
    volatile uint32_t systemTicks;
    volatile int16_t pendingExtiPin;

    RgbLed statusLed;
    OutputDevice buzzer;
    OutputDevice lockRelay;
    OutputDevice trunkRelay;

    Button btnLock;
    Button btnUnlock;
    Button btnTrunk;
    MotionSensor pirSensor;

    void changeState(State newState);

    void initHardwareRegisters();
    void initClocks();
    void initGPIO();
    void initEXTI();
    void initTimers();
};

extern CarSecuritySystem carSystem;

#endif

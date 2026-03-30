#include "CarSecuritySystem.hpp"

CarSecuritySystem carSystem;

CarSecuritySystem::CarSecuritySystem()
    : currentState(OPENED), previousState(OPENED), stateStartTime(0), systemTicks(0), pendingExtiPin(-1),
      statusLed(GPIOB, GPIO_BSRR_BS5, GPIOB, GPIO_BSRR_BS6, GPIOB, GPIO_BSRR_BS7, false),
      buzzer(GPIOB, GPIO_BSRR_BS4, false),
      lockRelay(GPIOB, GPIO_BSRR_BS0, false),
      trunkRelay(GPIOB, GPIO_BSRR_BS1, false),
      btnLock(GPIOA, GPIO_IDR_IDR0, true),
      btnUnlock(GPIOA, GPIO_IDR_IDR1, true),
      btnTrunk(GPIOA, GPIO_IDR_IDR2, true),
      pirSensor(GPIOA, GPIO_IDR_IDR7)
{}

void CarSecuritySystem::init() {
    initHardwareRegisters();
    changeState(OPENED);
}

void CarSecuritySystem::initHardwareRegisters() {
    initClocks();
    initGPIO();
    initEXTI();
    initTimers();
}

void CarSecuritySystem::initClocks() {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN | RCC_APB2ENR_TIM1EN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN | RCC_APB1ENR_TIM3EN;
    AFIO->MAPR = (AFIO->MAPR & ~AFIO_MAPR_SWJ_CFG) | AFIO_MAPR_SWJ_CFG_JTAGDISABLE;
}

void CarSecuritySystem::initGPIO() {
    GPIOA->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0 | GPIO_CRL_MODE1 | GPIO_CRL_CNF1 | GPIO_CRL_MODE2 | GPIO_CRL_CNF2);
    GPIOA->CRL |= (GPIO_CRL_CNF0_1 | GPIO_CRL_CNF1_1 | GPIO_CRL_CNF2_1);
    GPIOA->ODR |= (GPIO_ODR_ODR0 | GPIO_ODR_ODR1 | GPIO_ODR_ODR2);

    GPIOA->CRL &= ~(GPIO_CRL_MODE7 | GPIO_CRL_CNF7);
    GPIOA->CRL |= GPIO_CRL_CNF7_1;
    GPIOA->ODR &= ~GPIO_ODR_ODR7;

    GPIOB->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0 | GPIO_CRL_MODE1 | GPIO_CRL_CNF1);
    GPIOB->CRL |= (GPIO_CRL_MODE0_1 | GPIO_CRL_MODE1_1);

    GPIOB->CRL &= ~(GPIO_CRL_MODE4 | GPIO_CRL_CNF4 | GPIO_CRL_MODE5 | GPIO_CRL_CNF5 | GPIO_CRL_MODE6 | GPIO_CRL_CNF6 | GPIO_CRL_MODE7 | GPIO_CRL_CNF7);
    GPIOB->CRL |= (GPIO_CRL_MODE4_1 | GPIO_CRL_MODE5_1 | GPIO_CRL_MODE6_1 | GPIO_CRL_MODE7_1);
}

void CarSecuritySystem::initEXTI() {
    AFIO->EXTICR[0] &= ~(AFIO_EXTICR1_EXTI0 | AFIO_EXTICR1_EXTI1 | AFIO_EXTICR1_EXTI2);
    AFIO->EXTICR[1] &= ~AFIO_EXTICR2_EXTI7;

    EXTI->IMR |= (EXTI_IMR_MR0 | EXTI_IMR_MR1 | EXTI_IMR_MR2 | EXTI_IMR_MR7);

    EXTI->FTSR |= (EXTI_FTSR_TR0 | EXTI_FTSR_TR1 | EXTI_FTSR_TR2);
    EXTI->RTSR |= EXTI_RTSR_TR7;

    NVIC_EnableIRQ(EXTI0_IRQn);
    NVIC_EnableIRQ(EXTI1_IRQn);
    NVIC_EnableIRQ(EXTI2_IRQn);
    NVIC_EnableIRQ(EXTI9_5_IRQn);
}

void CarSecuritySystem::initTimers() {
    TIM1->PSC = 8000 - 1;
    TIM1->ARR = 1 - 1;
    TIM1->DIER |= TIM_DIER_UIE;
    TIM1->CR1 |= TIM_CR1_CEN;
    NVIC_EnableIRQ(TIM1_UP_IRQn);

    TIM2->PSC = 8000 - 1;
    TIM2->ARR = DEBOUNCE_DELAY_MS - 1;
    TIM2->CR1 |= TIM_CR1_OPM;
    TIM2->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM2_IRQn);

    TIM3->PSC = 8000 - 1;
    TIM3->ARR = DOUBLE_CLICK_WINDOW_MS - 1;
    TIM3->CR1 |= TIM_CR1_OPM;
}

void CarSecuritySystem::changeState(State newState) {
    previousState = currentState;
    currentState = newState;
    stateStartTime = millis();

    statusLed.turnOff();
    buzzer.off();
    lockRelay.off();
    trunkRelay.off();
}

void CarSecuritySystem::update() {
    uint32_t timeInState = millis() - stateStartTime;

    switch (currentState) {
        case OPENED: {
            lockRelay.set(timeInState < RELAY_PULSE_MS);
            buzzer.set(timeInState < BUZZER_OPEN_MS);
            bool greenOn = (timeInState % 1000) < 200;
            statusLed.setColor(false, greenOn, false);
            break;
        }
        case CLOSED: {
            lockRelay.set(timeInState < RELAY_PULSE_MS);
            buzzer.set(timeInState < BUZZER_CLOSE_MS);
            if (timeInState < 600) {
                bool redOn = (timeInState % 200) < 100;
                statusLed.setColor(redOn, false, false);
            } else {
                statusLed.turnOff();
            }
            break;
        }
        case WARNING: {
            if (timeInState >= WARNING_DURATION_MS) {
                changeState(CLOSED);
                break;
            }
            if (pirSensor.isTriggered()) {
                changeState(ALARM);
                break;
            }
            uint32_t period = 1000 - ((timeInState * 800) / WARNING_DURATION_MS);
            if (period < 200) period = 200;
            bool redOn = (timeInState % period) < (period / 2);
            statusLed.setColor(redOn, false, false);
            break;
        }
        case ALARM: {
            bool redOn = (timeInState % 125) < 62;
            bool buzOn = (timeInState % 300) < 150;
            statusLed.setColor(redOn, false, false);
            buzzer.set(buzOn);
            break;
        }
        case TRUNK_ACTION: {
            trunkRelay.set(timeInState < RELAY_PULSE_MS);
            if (timeInState < 600) {
                bool blueOn = (timeInState % 200) < 100;
                statusLed.setColor(false, false, blueOn);
            } else {
                statusLed.turnOff();
            }

            buzzer.set((timeInState < 400) && ((timeInState % 200) < 100));

            if (timeInState >= 600) {
                changeState(previousState);
            }
            break;
        }
    }
}


void CarSecuritySystem::handleExtiInterrupt(uint16_t pin) {
    pendingExtiPin = pin;
    EXTI->IMR &= ~(EXTI_IMR_MR0 | EXTI_IMR_MR1 | EXTI_IMR_MR2 | EXTI_IMR_MR7);

    TIM2->CNT = 0;
    TIM2->CR1 |= TIM_CR1_CEN;
}

void CarSecuritySystem::processDebouncedInput() {
    if (pendingExtiPin == 0 && btnLock.isPressed()) {
        changeState(CLOSED);
    }
    else if (pendingExtiPin == 1 && btnUnlock.isPressed()) {
        changeState(OPENED);
    }
    else if (pendingExtiPin == 2 && btnTrunk.isPressed()) {
        if (TIM3->CR1 & TIM_CR1_CEN) {
            TIM3->CR1 &= ~TIM_CR1_CEN;
            changeState(TRUNK_ACTION);
        } else {
            TIM3->CNT = 0;
            TIM3->CR1 |= TIM_CR1_CEN;
        }
    }
    else if (pendingExtiPin == 7 && pirSensor.isTriggered()) {
        if (currentState == CLOSED) changeState(WARNING);
        else if (currentState == WARNING) changeState(ALARM);
    }

    pendingExtiPin = -1;
    EXTI->IMR |= (EXTI_IMR_MR0 | EXTI_IMR_MR1 | EXTI_IMR_MR2 | EXTI_IMR_MR7);
}

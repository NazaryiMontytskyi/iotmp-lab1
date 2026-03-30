#include "stm32f103x6.h"
#include "CarSecuritySystem.hpp"

extern "C" {

void TIM1_UP_IRQHandler(void) {
    if (TIM1->SR & TIM_SR_UIF) {
        TIM1->SR &= ~TIM_SR_UIF;
        carSystem.incrementTick();
    }
}

void TIM2_IRQHandler(void) {
    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR &= ~TIM_SR_UIF;
        carSystem.processDebouncedInput();
    }
}

void EXTI0_IRQHandler(void) {
    if (EXTI->PR & EXTI_PR_PR0) {
        EXTI->PR = EXTI_PR_PR0;
        carSystem.handleExtiInterrupt(0);
    }
}

void EXTI1_IRQHandler(void) {
    if (EXTI->PR & EXTI_PR_PR1) {
        EXTI->PR = EXTI_PR_PR1;
        carSystem.handleExtiInterrupt(1);
    }
}

void EXTI2_IRQHandler(void) {
    if (EXTI->PR & EXTI_PR_PR2) {
        EXTI->PR = EXTI_PR_PR2;
        carSystem.handleExtiInterrupt(2);
    }
}

void EXTI9_5_IRQHandler(void) {
    if (EXTI->PR & EXTI_PR_PR7) {
        EXTI->PR = EXTI_PR_PR7;
        carSystem.handleExtiInterrupt(7);
    }
}

}

int main(void) {
    carSystem.init();

    while (1) {
        carSystem.update();
    }
}
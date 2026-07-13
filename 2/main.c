/* Copyright 2026 Robert Krook
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "stm32l552xx.h"

void pll_configure_110mhz(void) {
    /* Step 1 & 2 - Disable the PLL and wait for it to stop */
    if (RCC->CR & RCC_CR_PLLON) {
        RCC->CR &= ~RCC_CR_PLLON;
        while (RCC->CR & RCC_CR_PLLRDY);
    }

    /* Step 3 - Configure the PLL */
    RCC->PLLCFGR &= ~( RCC_PLLCFGR_PLLSRC
                     | RCC_PLLCFGR_PLLM
                     | RCC_PLLCFGR_PLLN
                     | RCC_PLLCFGR_PLLR);

    RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_0
                  | (0  << RCC_PLLCFGR_PLLM_Pos)
                  | (55 << RCC_PLLCFGR_PLLN_Pos)
                  | (0  << RCC_PLLCFGR_PLLR_Pos);

    /* Step 4 - Enable the PLL */
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY));

    /* Step 5 - Enable the desired output */
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLREN;

    /* Set the number of flash wait states before switching the system clock */
    FLASH->ACR = (FLASH->ACR & ~FLASH_ACR_LATENCY) | (5 << FLASH_ACR_LATENCY_Pos);
    while ((FLASH->ACR & FLASH_ACR_LATENCY) != (5 << FLASH_ACR_LATENCY_Pos));

    /* Select the PLL as the system clock */
    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | (3U << RCC_CFGR_SW_Pos);
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS);
}

void enable_gpioa(void) {
    volatile uint32_t dummy;

    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    dummy = RCC->AHB2ENR;   /* read-back so the clock settles before we touch the port */
    (void)dummy;
}

void configure_gpioa(void) {
    GPIOA->MODER &= ~GPIO_MODER_MODE9;     /* clear both mode bits for pin 9 */
    GPIOA->MODER |=  GPIO_MODER_MODE9_0;   /* 01 = general-purpose output    */
}

void toggle_led(void) {
    GPIOA->ODR ^= GPIO_ODR_OD9;
}

volatile uint32_t ticks = 0;

void systick_handler(void) {
    ticks++;
}

void systick_delay_ms(uint32_t ms) {
    uint32_t start = ticks;
    uint32_t end   = start + ms;

    if (end < start) { /* overflow */
        while (ticks > start);
    }
    while (ticks < end);
}

void configure_lpuart1(uint32_t clk_hz, uint32_t baud_rate) {
    RCC->APB1ENR1 |= RCC_APB1ENR1_PWREN;
    PWR->CR2      |= PWR_CR2_IOSV;

    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOGEN;

    GPIOG->MODER &= ~(GPIO_MODER_MODE7 | GPIO_MODER_MODE8);
    GPIOG->MODER |=  (GPIO_MODER_MODE7_1 | GPIO_MODER_MODE8_1);

    GPIOG->PUPDR &= ~(GPIO_PUPDR_PUPD7 | GPIO_PUPDR_PUPD8);
    GPIOG->PUPDR |=  (GPIO_PUPDR_PUPD7_0 | GPIO_PUPDR_PUPD8_0);

    /* Pin 7 configured from AFRL register, and pin 8 configured from AFRH */
    GPIOG->AFR[0] &= ~GPIO_AFRL_AFSEL7;
    GPIOG->AFR[0] |=  (8U << GPIO_AFRL_AFSEL7_Pos);
    GPIOG->AFR[1] &= ~GPIO_AFRH_AFSEL8;
    GPIOG->AFR[1] |=  (8U << GPIO_AFRH_AFSEL8_Pos);

    RCC->APB1ENR2 |= RCC_APB1ENR2_LPUART1EN;

    RCC->CCIPR1 &= ~RCC_CCIPR1_LPUART1SEL;
    RCC->CCIPR1 |= RCC_CCIPR1_LPUART1SEL_0;

    /* Deviates from the blog post here: 256U * clk_hz overflows uint32_t once
     * clk_hz reaches 110 MHz, silently producing the wrong BRR value. Widen
     * the multiplication to 64 bits to avoid that. */
    LPUART1->BRR = (uint32_t)((256ULL * clk_hz) / baud_rate);
    LPUART1->CR1 |= (USART_CR1_UE | USART_CR1_TE | USART_CR1_RE);
}

void uart_putc(uint8_t c) {
    while (!(LPUART1->ISR & USART_ISR_TXE));
    LPUART1->TDR = c;
    while (!(LPUART1->ISR & USART_ISR_TC));
}

uint8_t uart_getc(void) {
    while (!(LPUART1->ISR & USART_ISR_RXNE));
    return (uint8_t)LPUART1->RDR;
}

void uart_puts(const char *s) {
    while (*s) {
        uart_putc((uint8_t)*s++);
    }
}

void main(void) {
    pll_configure_110mhz();
    SysTick_Config(110000);
    configure_lpuart1(110000000, 115200);
    __enable_irq();

    enable_gpioa();
    configure_gpioa();

    while (1) {
        uart_puts("toggling the LED\r\n");
        toggle_led();
        systick_delay_ms(500);
    }
}

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
 
#include <stdint.h>

#define VECTOR_SIZE_WORDS 116 // the vector table can accommodate 116 entries for this device

extern uint32_t _estack; // This symbol is supplied by the linker script, remember?
extern uint32_t _sdata, _sidata, _edata, _sbss, _ebss; // symbols defined by the linker

void reset_handler(void);
void systick_handler(void);
void main(void);

void default_handler(void) {
    while (1) {}
}

uint32_t isr_vector[VECTOR_SIZE_WORDS] __attribute__((section(".isr_vector"))) = {
    (uint32_t)&_estack,
    (uint32_t)&reset_handler,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    (uint32_t)&systick_handler,
    // add more handlers as wanted
};

void reset_handler(void) {
    // Copy .data from FLASH to SRAM
    uint32_t data_size = (uint32_t)&_edata - (uint32_t)&_sdata;
    uint8_t *flash_data = (uint8_t *)&_sidata;
    uint8_t *sram_data  = (uint8_t *)&_sdata;

    for (uint32_t i = 0; i < data_size; i++)
    {
        sram_data[i] = flash_data[i];
    }

    // Zero-fill .bss section in SRAM
    uint32_t bss_size = (uint32_t)&_ebss - (uint32_t)&_sbss;
    uint8_t *bss = (uint8_t *)&_sbss;

    for (uint32_t i = 0; i < bss_size; i++)
    {
        bss[i] = 0;
    }

    main();
}

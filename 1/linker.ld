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

MEMORY
{
    ISR       (rw):  ORIGIN = 0x08000000, LENGTH = 2K
    FLASH     (rx):  ORIGIN = 0x08000800, LENGTH = 510K
    SRAM1     (rwx): ORIGIN = 0x20000000, LENGTH = 256K
}

PROVIDE(_estack = ORIGIN(SRAM1) + LENGTH(SRAM1));

SECTIONS
{
    .isr_vector :
    {
        KEEP(*(.isr_vector)) /* The code does not refer to isr_vector, so we must ask to keep it */
                             /* Otherwise the optimiser might remove the section all together    */
    } > ISR

    .text :
    {
        . = ALIGN(4);
        *(.text)
        *(.text.*)

        __exidx_start = .;
        KEEP(*(.ARM.exidx*))
        __exidx_end = .;

        . = ALIGN(4);
        _etext = .;
    } > FLASH

    .rodata :
    {
        . = ALIGN(4);
        *(.rodata)
        *(.rodata.*)
        . = ALIGN(4);

    } > FLASH

    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        *(.data.*)
        . = ALIGN(4);
        _edata = .;
    } > SRAM1 AT> FLASH

    _sidata = LOADADDR(.data);

    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        __bss_start__ = _sbss;
        *(.bss)
        *(.bss.*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
        __bss_end__ = _ebss;
    } > SRAM1

    . = ALIGN(8);
    _end = .;
    PROVIDE(end = .);
}

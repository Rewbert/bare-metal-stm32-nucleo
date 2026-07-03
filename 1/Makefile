# Copyright 2026 Robert Krook

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at

#     http://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

CROSS ?= arm-none-eabi-
CC    := $(CROSS)gcc
SIZE  := $(CROSS)size

# Cortex-M33 core, no operating system and no standard library.
CFLAGS   := -mcpu=cortex-m33 -mthumb -ffreestanding -nostdlib -Wall -Wextra -O0 -g
LDFLAGS  := -T linker.ld
CPPFLAGS := -Ivendor/CMSIS/Device/ST/STM32L5/Include -Ivendor/CMSIS/CMSIS/Core/Include

# --- OpenOCD (ST's fork, built but not installed) ---
OPENOCD_DIR     ?= $(HOME)/Database/Projects/OpenOCD
OPENOCD         := $(OPENOCD_DIR)/src/openocd
OPENOCD_SCRIPTS := $(OPENOCD_DIR)/tcl
OPENOCD_CFG     := -s $(OPENOCD_SCRIPTS) -f interface/stlink.cfg -f target/stm32l5x.cfg

# --- Sources ---
TARGET := blink
SRCS   := startup.c main.c
OBJS   := $(SRCS:.c=.o)

.PHONY: all flash clean

all: $(TARGET).elf

# compile each C source into an object file
%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

# link the object files into the final ELF using our linker script.
# -lgcc pulls in the compiler support library: -nostdlib drops it too, but we
# still need it for the 64-bit division the baud rate calculation performs.
$(TARGET).elf: $(OBJS) linker.ld
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) -o $@
	$(SIZE) $@

# Flash the ELF onto the board
flash: $(TARGET).elf
	$(OPENOCD) $(OPENOCD_CFG) -c "program $(TARGET).elf verify reset exit"

clean:
	rm -f $(OBJS) $(TARGET).elf
